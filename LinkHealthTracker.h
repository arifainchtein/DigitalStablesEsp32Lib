#ifndef LINKHEALTHTRACKER_H
#define LINKHEALTHTRACKER_H

#include <Arduino.h>
#include <new>
#include <LinkHealthRecord.h>

// Collects what goes into a LinkHealthRecord (2026-11-10), the same way on every device, so the
// numbers mean the same thing whichever product sends them. The device calls the note*() hooks
// from its own LoRa code and build() when it is time to send.
//
// All state is in one plain struct, LinkHealthState, so a device that deep-sleeps can keep it in
// RTC memory (RTC_DATA_ATTR LinkHealthState s; LinkHealthTracker t(s);) and its running totals
// survive the sleep. Times are passed in as seconds from a clock that survives that sleep too
// (an always-on device can use millis() / 1000).
//
// Hooks, and where a device calls them:
//   noteSendScheduled()      every time its schedule calls for a data packet
//   noteSlotsMissed(n)       its loop came round n send slots late (each also counts as scheduled)
//   noteSendResult(r)        what became of that send (SENT / BUSY = given up / FAILED)
//   noteBusyExhausted()      busy on every retry, and the device sends anyway (then also SENT or FAILED)
//   noteBusy(rssi)           each busy-channel back-off, with the reading that caused it
//   noteNoiseSample(rssi)    once a second while idle in receive (LoRa.rssi())
//   noteRxPacket()           a good packet of any type (parsePacket() > 0)
//   noteRxCrcError()         DIO0 fired with no good packet - clear the DIO0 flag after your own TX
//                            first, or every TX-done counts as a CRC error
//   noteGateway(...)         a packet from Annabelle (her weather forecast)
//   noteNeighbour(...)       a packet from another device, by its serial

#define LINK_HEALTH_NOISE_MIN_DBM -150   // histogram floor; 2 dB buckets
#define LINK_HEALTH_NOISE_BUCKETS 60     // up to -30 dBm

enum LinkHealthSendResult { LINK_HEALTH_SENT, LINK_HEALTH_BUSY, LINK_HEALTH_FAILED };

// Plain bytes only (no constructors), so it is safe in RTC_DATA_ATTR: a C++ constructor would run
// again on every deep-sleep wake and wipe the totals. The running totals are a LinkHealthRecord
// kept as raw bytes - LinkHealthTracker::totals() reads them.
struct LinkHealthState {
  uint8_t  totalsBytes[sizeof(LinkHealthRecord)];        // running totals + the gateway figures
  uint16_t noiseHist[LINK_HEALTH_NOISE_BUCKETS];         // this window's idle-channel samples
  uint16_t noiseSamples;
  int8_t   noiseMin, noiseMax, busyMax;                  // this window
  uint8_t  neighbourSerial[LINK_HEALTH_NEIGHBOURS][8];
  uint32_t neighbourLastSec[LINK_HEALTH_NEIGHBOURS];     // 0 = slot unused
  uint32_t gatewayLastSec;                               // 0 = never heard Annabelle
  bool     initialised;
};

class LinkHealthTracker {
public:
  explicit LinkHealthTracker(LinkHealthState &state) : s(state) {}

  // Call once at boot. Keeps the totals if the state was already initialised (an RTC-memory
  // state after a deep-sleep wake); a real reset (RTC memory cleared) starts from zero.
  void begin(int8_t txPowerDbm, uint8_t spreadingFactor, uint16_t dataPacketBytes) {
    if (!s.initialised) {
      memset(&s, 0, sizeof(s));
      new (s.totalsBytes) LinkHealthRecord();  // its defaults (magic, version, no-noise markers)
      resetWindow();
      s.initialised = true;
    }
    T().txPowerDbm = txPowerDbm;
    T().spreadingFactor = spreadingFactor;
    T().dataPacketBytes = dataPacketBytes;
  }

  // The running totals (for a device's own log lines).
  const LinkHealthRecord &totals() const { return T(); }

  void noteSendScheduled() { T().dataScheduled++; }

  void noteSlotsMissed(uint16_t n) {
    T().slotsMissed += n;
    T().dataScheduled += n;
  }

  void noteSendResult(LinkHealthSendResult r) {
    if (r == LINK_HEALTH_SENT) T().dataSent++;
    else if (r == LINK_HEALTH_BUSY) T().busyExhausted++;
    else T().txFailed++;
  }

  void noteBusyExhausted() { T().busyExhausted++; }

  void noteBusy(float rssi) {
    int8_t v = clampDbm(rssi);
    if (s.busyMax == LINK_HEALTH_NO_NOISE || v > s.busyMax) s.busyMax = v;
  }

  void noteNoiseSample(int rssi) {
    int8_t v = clampDbm(rssi);
    int b = (v - LINK_HEALTH_NOISE_MIN_DBM) / 2;
    if (b < 0) b = 0;
    if (b >= LINK_HEALTH_NOISE_BUCKETS) b = LINK_HEALTH_NOISE_BUCKETS - 1;
    if (s.noiseHist[b] < 0xFFFF) s.noiseHist[b]++;
    if (s.noiseSamples < 0xFFFF) s.noiseSamples++;
    if (s.noiseMin == LINK_HEALTH_NO_NOISE || v < s.noiseMin) s.noiseMin = v;
    if (s.noiseMax == LINK_HEALTH_NO_NOISE || v > s.noiseMax) s.noiseMax = v;
  }

  void noteRxPacket() { T().rxPackets++; }
  void noteRxCrcError() { T().rxCrcErrors++; }

  void noteGateway(int rssi, float snr, uint32_t nowSec) {
    T().gatewayRssi = (int16_t)rssi;
    T().gatewaySnrQ = snrQ(snr);
    T().gatewayHeard++;
    s.gatewayLastSec = nowSec ? nowSec : 1;
  }

  void noteNeighbour(const uint8_t *serial, int rssi, float snr, uint32_t nowSec) {
    int slot = -1, oldest = 0;
    for (int i = 0; i < LINK_HEALTH_NEIGHBOURS; i++) {
      if (s.neighbourLastSec[i] && memcmp(s.neighbourSerial[i], serial, 8) == 0) { slot = i; break; }
      if (s.neighbourLastSec[i] < s.neighbourLastSec[oldest]) oldest = i;
    }
    if (slot < 0) {  // a new neighbour takes the empty or least recently heard slot
      slot = oldest;
      memcpy(s.neighbourSerial[slot], serial, 8);
      T().neighbours[slot] = LinkHealthNeighbour();
      T().neighbours[slot].tag = (uint16_t)((serial[6] << 8) | serial[7]);
      if (T().neighbours[slot].tag == 0) T().neighbours[slot].tag = 1;  // 0 means unused
    }
    LinkHealthNeighbour &n = T().neighbours[slot];
    n.rssi = (int16_t)rssi;
    n.snrQ = snrQ(snr);
    n.heard++;
    s.neighbourLastSec[slot] = nowSec ? nowSec : 1;
  }

  // For the LinkAckRecord answer to a ping: how this device hears Annabelle. False = never heard.
  bool gateway(int16_t &rssi, int8_t &snrQOut, uint16_t &ageSec, uint32_t nowSec) const {
    if (!s.gatewayLastSec) return false;
    rssi = T().gatewayRssi;
    snrQOut = T().gatewaySnrQ;
    ageSec = gatewayAge(nowSec);
    return true;
  }

  // The record to send: totals + this window. Starts a new window. The caller fills totpcode and
  // the serial, then linkHealthSeal()s it.
  LinkHealthRecord build(uint32_t nowSec, uint32_t uptimeSec) {
    LinkHealthRecord r = T();
    r.seq = ++T().seq;
    r.uptimeSec = uptimeSec;
    r.noiseMin = s.noiseMin;
    r.noiseMax = s.noiseMax;
    r.noiseMedian = noiseMedian();
    r.busyMaxRssi = s.busyMax;
    r.gatewayAgeSec = s.gatewayLastSec ? gatewayAge(nowSec) : LINK_HEALTH_UNKNOWN_AGE;
    // neighbours most recently heard first
    int order[LINK_HEALTH_NEIGHBOURS];
    for (int i = 0; i < LINK_HEALTH_NEIGHBOURS; i++) order[i] = i;
    for (int i = 0; i < LINK_HEALTH_NEIGHBOURS; i++)
      for (int j = i + 1; j < LINK_HEALTH_NEIGHBOURS; j++)
        if (s.neighbourLastSec[order[j]] > s.neighbourLastSec[order[i]]) { int t = order[i]; order[i] = order[j]; order[j] = t; }
    for (int i = 0; i < LINK_HEALTH_NEIGHBOURS; i++) {
      r.neighbours[i] = s.neighbourLastSec[order[i]] ? T().neighbours[order[i]] : LinkHealthNeighbour();
    }
    resetWindow();
    return r;
  }

  // One JSON line (serial command GetLinkHealth): the totals and the window so far, without
  // starting a new window.
  void printJson(Print &out, uint32_t nowSec) const {
    const LinkHealthRecord &t = T();
    out.printf("{\"seq\":%u,\"scheduled\":%u,\"sent\":%u,\"busy\":%u,\"failed\":%u,\"missed\":%u,",
               t.seq, t.dataScheduled, t.dataSent, t.busyExhausted, t.txFailed, t.slotsMissed);
    out.printf("\"noiseMin\":%d,\"noiseMedian\":%d,\"noiseMax\":%d,\"noiseSamples\":%u,\"busyMax\":%d,",
               s.noiseMin, noiseMedian(), s.noiseMax, s.noiseSamples, s.busyMax);
    if (s.gatewayLastSec) {
      out.printf("\"gatewayRssi\":%d,\"gatewaySnr\":%.2f,\"gatewayHeard\":%u,\"gatewayAgeSec\":%u,",
                 t.gatewayRssi, t.gatewaySnrQ / 4.0, t.gatewayHeard, gatewayAge(nowSec));
    }
    out.print("\"neighbours\":[");
    bool first = true;
    for (int i = 0; i < LINK_HEALTH_NEIGHBOURS; i++) {
      if (!s.neighbourLastSec[i]) continue;
      out.printf("%s{\"serial\":\"", first ? "" : ",");
      for (int k = 0; k < 8; k++) out.printf("%02X", s.neighbourSerial[i][k]);
      out.printf("\",\"rssi\":%d,\"snr\":%.2f,\"heard\":%u,\"ageSec\":%lu}", t.neighbours[i].rssi, t.neighbours[i].snrQ / 4.0,
                 t.neighbours[i].heard, (unsigned long)(nowSec - s.neighbourLastSec[i]));
      first = false;
    }
    out.printf("],\"rxPackets\":%u,\"rxCrcErrors\":%u,\"txPowerDbm\":%d,\"sf\":%u,\"dataPacketBytes\":%u}\n",
               t.rxPackets, t.rxCrcErrors, t.txPowerDbm, t.spreadingFactor, t.dataPacketBytes);
  }

private:
  LinkHealthState &s;

  LinkHealthRecord &T() { return *reinterpret_cast<LinkHealthRecord *>(s.totalsBytes); }
  const LinkHealthRecord &T() const { return *reinterpret_cast<const LinkHealthRecord *>(s.totalsBytes); }

  void resetWindow() {
    memset(s.noiseHist, 0, sizeof(s.noiseHist));
    s.noiseSamples = 0;
    s.noiseMin = s.noiseMax = s.busyMax = LINK_HEALTH_NO_NOISE;
  }

  int8_t noiseMedian() const {
    if (!s.noiseSamples) return LINK_HEALTH_NO_NOISE;
    uint32_t half = (s.noiseSamples + 1) / 2, seen = 0;
    for (int b = 0; b < LINK_HEALTH_NOISE_BUCKETS; b++) {
      seen += s.noiseHist[b];
      if (seen >= half) return (int8_t)(LINK_HEALTH_NOISE_MIN_DBM + b * 2 + 1);  // bucket centre
    }
    return s.noiseMax;
  }

  uint16_t gatewayAge(uint32_t nowSec) const {
    uint32_t age = nowSec >= s.gatewayLastSec ? nowSec - s.gatewayLastSec : 0;
    return age >= LINK_HEALTH_UNKNOWN_AGE ? LINK_HEALTH_UNKNOWN_AGE - 1 : (uint16_t)age;
  }

  static int8_t clampDbm(float dbm) {
    if (dbm < -127) return -127;
    if (dbm > 126) return 126;   // 127 = LINK_HEALTH_NO_NOISE
    return (int8_t)lroundf(dbm);
  }

  static int8_t snrQ(float snr) {
    long q = lroundf(snr * 4);
    return (int8_t)(q < -128 ? -128 : q > 127 ? 127 : q);
  }
};

#endif
