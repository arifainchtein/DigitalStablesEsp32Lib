#ifndef LORARELAY_H
#define LORARELAY_H
// LoRa relay (2026-11-11) - one copy for every device that can relay: Paula (test relay), Langley.
// Started as Paula's relay mode (2026-11-10). A relay resends the data packets it hears so Annabelle
// gets a second chance at devices she can't hear well (Chinampa: -112 dBm, no margin).
//
//   Relayed: DigitalStablesData, ChinampaData, LangleyData, VitalSignsRecord, LinkHealthRecord -
//   only packets that check out as received (checksum / magic).
//   Marked: sentbyarray = the relay's name (DigitalStablesData, ChinampaData, LangleyData have one;
//   DSD's and Langley's checksum covers it, so it is recomputed). Annabelle shows "via <name>".
//   Never relayed:
//     - a packet already carrying a sentby (someone relayed it) - two relays can't bounce it,
//     - one heard at strongDbm or stronger (default -80): the device doesn't need help,
//     - one relayed in the last LORA_RELAY_SEEN_MS (hash of its bytes, leaving out sentby, checksum
//       and the receiver's rssi/snr, which change between copies).
//   When: 150-650 ms after hearing it (random, so two relays don't collide), and not while the
//   channel is busy (the sketch's busy check, up to 3 waits of 300 ms, then anyway).
//
// The sketch owns the radio: begin() takes its send and busy functions. From loop(): note() for every
// packet received (the bytes as received, before rssi/snr are written into them), service() every pass.
#include <Arduino.h>
#include <stddef.h>
#include <string.h>
#include "DigitalStablesData.h"
#include "ChinampaData.h"
#include "LangleyData.h"
#include "VitalSignsRecord.h"
#include "LinkHealthRecord.h"

#define LORA_RELAY_QUEUE 4
#define LORA_RELAY_SEEN 32
#define LORA_RELAY_SEEN_MS (10UL * 60UL * 1000UL)
#define LORA_RELAY_MAX 248
#define LORA_RELAY_STRONG_DEFAULT -80
#define LORA_RELAY_NAMES 8

// What note() did with a packet - also what the ticker shows (lastAction).
enum LoRaRelayResult : uint8_t {
  LORA_RELAY_NOT_ON = 0,   // relay off, or not a packet type that is relayed / didn't check out
  LORA_RELAY_QUEUED,       // will be resent
  LORA_RELAY_SENT,         // (lastAction only) resent
  LORA_RELAY_STRONG,       // heard strongly - not needed
  LORA_RELAY_ALREADY,      // already carries a sentby
  LORA_RELAY_DUP,          // relayed recently
  LORA_RELAY_FULL          // queue full
};

typedef bool (*LoRaRelaySendFn)(const uint8_t *bytes, size_t len);  // transmit, back to receive; true = sent
typedef bool (*LoRaRelayBusyFn)();                                   // someone is transmitting now

class LoRaRelay {
public:
  bool on = false;
  int strongDbm = LORA_RELAY_STRONG_DEFAULT;  // 0 = relay everything
  uint32_t heard = 0, sent = 0, dup = 0, dropped = 0, strong = 0;
  // The ticker: the last packet relayed, or not relayed for being strong.
  char lastName[17] = "";
  uint8_t lastAction = 0;  // LORA_RELAY_SENT / LORA_RELAY_STRONG
  int lastRssi = 0;
  uint32_t lastAt = 0;     // millis(), 0 = none yet

  // sentByName: up to 8 characters, stamped into sentbyarray.
  void begin(const char *sentByName, LoRaRelaySendFn send, LoRaRelayBusyFn busy) {
    memset(_sentBy, 0, sizeof(_sentBy));
    strncpy(_sentBy, sentByName, sizeof(_sentBy));
    _send = send;
    _busy = busy;
  }

  void setOn(bool v) {
    on = v;
    if (!on) for (int i = 0; i < LORA_RELAY_QUEUE; i++) _q[i].len = 0;
  }

  int queued() const {
    int n = 0;
    for (int i = 0; i < LORA_RELAY_QUEUE; i++) if (_q[i].len) n++;
    return n;
  }

  // A packet as received. rssi: its packet RSSI.
  LoRaRelayResult note(const uint8_t *p, size_t len, int rssi) {
    int type = typeOf(len);
    if (type < 0 || !checksOut(p, len, type)) return LORA_RELAY_NOT_ON;
    char name[17];
    nameOf(p, len, type, name);  // also learns serial -> name from data packets, so on with relay off too
    if (!on || len > LORA_RELAY_MAX) return LORA_RELAY_NOT_ON;
    if (sentByOffset(type) >= 0) {
      const uint8_t *sb = p + sentByOffset(type);
      for (int i = 0; i < 8; i++) if (sb[i]) return LORA_RELAY_ALREADY;
    }
    heard++;
    if (strongDbm < 0 && rssi >= strongDbm) {
      strong++;
      tick(name, LORA_RELAY_STRONG, rssi);
      return LORA_RELAY_STRONG;
    }
    uint32_t h = hash(p, len, type), now = millis();
    for (int i = 0; i < LORA_RELAY_SEEN; i++) {
      if (_seenMs[i] && _seenHash[i] == h && now - _seenMs[i] < LORA_RELAY_SEEN_MS) { dup++; return LORA_RELAY_DUP; }
    }
    _seenHash[_seenNext] = h;
    _seenMs[_seenNext] = now ? now : 1;
    _seenNext = (_seenNext + 1) % LORA_RELAY_SEEN;
    for (int i = 0; i < LORA_RELAY_QUEUE; i++) {
      Slot &s = _q[i];
      if (s.len) continue;
      s.len = len;
      s.waits = 0;
      s.dueMs = now + random(150, 650);
      s.rssi = rssi;
      strncpy(s.name, name, sizeof(s.name) - 1);
      s.name[sizeof(s.name) - 1] = 0;
      memcpy(s.bytes, p, len);
      stamp(s.bytes, len, type);
      return LORA_RELAY_QUEUED;
    }
    dropped++;
    return LORA_RELAY_FULL;
  }

  // From loop(): sends the queued packets whose time has come. rxPending: a received packet is
  // waiting to be read - that first.
  void service(bool rxPending) {
    if (!_send) return;
    for (int i = 0; i < LORA_RELAY_QUEUE; i++) {
      Slot &s = _q[i];
      if (!s.len || (int32_t)(millis() - s.dueMs) < 0) continue;
      if (rxPending) return;
      if (s.waits < 3 && _busy && _busy()) {
        s.waits++;
        s.dueMs = millis() + 300;
        continue;
      }
      if (_send(s.bytes, s.len)) {
        sent++;
        tick(s.name, LORA_RELAY_SENT, s.rssi);
      }
      s.len = 0;
    }
  }

  // {"on":..,"heard":..,"sent":..,"dup":..,"dropped":..,"queued":..,"strong":..,"strongDbm":..
  //  [,"last":"<name>","lastAgeSec":..,"lastAction":"relayed"|"strong","lastRssi":..]}
  void printJson(Print &out) {
    out.printf("{\"on\":%s,\"heard\":%lu,\"sent\":%lu,\"dup\":%lu,\"dropped\":%lu,\"queued\":%d,\"strong\":%lu,\"strongDbm\":%d",
               on ? "true" : "false", (unsigned long)heard, (unsigned long)sent, (unsigned long)dup,
               (unsigned long)dropped, queued(), (unsigned long)strong, strongDbm);
    if (lastAt) {
      out.printf(",\"last\":\"%s\",\"lastAgeSec\":%lu,\"lastAction\":\"%s\",\"lastRssi\":%d", lastName,
                 (unsigned long)((millis() - lastAt) / 1000), lastAction == LORA_RELAY_STRONG ? "strong" : "relayed", lastRssi);
    }
    out.print("}");
  }

  // Relay#1 / Relay#0 / Relay#1#<dBm> (0 = relay everything, -120..-1 accepted); "" keeps it.
  void applyCommand(const String &onArg, const String &dbmArg) {
    if (onArg.length()) setOn(onArg.toInt() > 0);
    String d = dbmArg;
    d.trim();
    if (d.length()) {
      int v = d.toInt();
      if (v == 0 || (v >= -120 && v < 0)) strongDbm = v;
    }
  }

private:
  struct Slot { uint16_t len; uint8_t waits; uint32_t dueMs; int rssi; char name[17]; uint8_t bytes[LORA_RELAY_MAX]; };
  Slot _q[LORA_RELAY_QUEUE] = {};
  uint32_t _seenHash[LORA_RELAY_SEEN] = {};
  uint32_t _seenMs[LORA_RELAY_SEEN] = {};
  uint8_t _seenNext = 0;
  char _sentBy[8] = {};
  LoRaRelaySendFn _send = nullptr;
  LoRaRelayBusyFn _busy = nullptr;
  // serial -> name, from data packets, so VitalSigns/LinkHealth (serial only) get a name on the ticker
  uint8_t _nameSerial[LORA_RELAY_NAMES][8] = {};
  char _names[LORA_RELAY_NAMES][17] = {};
  uint8_t _nameNext = 0;

  enum { T_DSD, T_CHINAMPA, T_LANGLEY, T_VITAL, T_LINK };

  static int typeOf(size_t len) {
    if (len == sizeof(DigitalStablesData)) return T_DSD;
    if (len == sizeof(ChinampaData)) return T_CHINAMPA;
    if (len == sizeof(LangleyData)) return T_LANGLEY;
    if (len == sizeof(VitalSignsRecord)) return T_VITAL;
    if (len == sizeof(LinkHealthRecord)) return T_LINK;
    return -1;
  }

  static uint8_t xorAllBut(const uint8_t *p, size_t len, size_t skip) {
    uint8_t c = 0;
    for (size_t i = 0; i < len; i++) if (i != skip) c ^= p[i];
    return c;
  }

  static bool checksOut(const uint8_t *p, size_t len, int type) {
    switch (type) {
      case T_DSD: return xorAllBut(p, len, offsetof(DigitalStablesData, checksum)) == p[offsetof(DigitalStablesData, checksum)];
      case T_LANGLEY: return xorAllBut(p, len, offsetof(LangleyData, checksum)) == p[offsetof(LangleyData, checksum)];
      case T_VITAL: return xorAllBut(p, len, offsetof(VitalSignsRecord, checksum)) == p[offsetof(VitalSignsRecord, checksum)];
      case T_LINK: {
        LinkHealthRecord r;
        memcpy(&r, p, sizeof(r));
        return linkHealthValid(r);
      }
      default: return true;  // ChinampaData: no checksum anyone uses (Annabelle checks its name)
    }
  }

  static int sentByOffset(int type) {
    switch (type) {
      case T_DSD: return offsetof(DigitalStablesData, sentbyarray);
      case T_CHINAMPA: return offsetof(ChinampaData, sentbyarray);
      case T_LANGLEY: return offsetof(LangleyData, sentbyarray);
      default: return -1;
    }
  }

  static bool inRange(size_t i, size_t from, size_t n) { return i >= from && i < from + n; }

  static uint32_t hash(const uint8_t *p, size_t len, int type) {
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < len; i++) {
      if (type == T_DSD && (i == offsetof(DigitalStablesData, checksum) || inRange(i, offsetof(DigitalStablesData, sentbyarray), 8) ||
                            inRange(i, offsetof(DigitalStablesData, rssi), offsetof(DigitalStablesData, snr) + sizeof(float) - offsetof(DigitalStablesData, rssi)))) continue;
      if (type == T_LANGLEY && (i == offsetof(LangleyData, checksum) || inRange(i, offsetof(LangleyData, sentbyarray), 8) ||
                                inRange(i, offsetof(LangleyData, rssi), offsetof(LangleyData, snr) + sizeof(float) - offsetof(LangleyData, rssi)))) continue;
      if (type == T_CHINAMPA && inRange(i, offsetof(ChinampaData, sentbyarray), 8)) continue;
      h ^= p[i];
      h *= 16777619u;
    }
    return h;
  }

  void stamp(uint8_t *b, size_t len, int type) {
    int at = sentByOffset(type);
    if (at < 0) return;  // VitalSigns / LinkHealth: no sentby field, sent unchanged
    memcpy(b + at, _sentBy, 8);
    if (type == T_DSD) b[offsetof(DigitalStablesData, checksum)] = xorAllBut(b, len, offsetof(DigitalStablesData, checksum));
    if (type == T_LANGLEY) b[offsetof(LangleyData, checksum)] = xorAllBut(b, len, offsetof(LangleyData, checksum));
  }

  static void copyName(char *out, const char *in, size_t n) {
    size_t k = 0;
    for (size_t i = 0; i < n && in[i] && k < 16; i++) if (in[i] >= 32 && in[i] < 127 && in[i] != '"' && in[i] != '\\') out[k++] = in[i];
    out[k] = 0;
  }

  void learn(const uint8_t *serial, const char *name) {
    for (int i = 0; i < LORA_RELAY_NAMES; i++) if (memcmp(_nameSerial[i], serial, 8) == 0) { strcpy(_names[i], name); return; }
    memcpy(_nameSerial[_nameNext], serial, 8);
    strcpy(_names[_nameNext], name);
    _nameNext = (_nameNext + 1) % LORA_RELAY_NAMES;
  }

  void nameOf(const uint8_t *p, size_t len, int type, char *out) {
    out[0] = 0;
    const uint8_t *serial = nullptr;
    switch (type) {
      case T_DSD: copyName(out, (const char *)p + offsetof(DigitalStablesData, devicename), sizeof(((DigitalStablesData *)0)->devicename));
                  serial = p + offsetof(DigitalStablesData, serialnumberarray); break;
      case T_CHINAMPA: copyName(out, (const char *)p + offsetof(ChinampaData, devicename), sizeof(((ChinampaData *)0)->devicename));
                  serial = p + offsetof(ChinampaData, serialnumberarray); break;
      case T_LANGLEY: copyName(out, (const char *)p + offsetof(LangleyData, devicename), sizeof(((LangleyData *)0)->devicename));
                  serial = p + offsetof(LangleyData, serialnumberarray); break;
      case T_VITAL: serial = p + offsetof(VitalSignsRecord, serialnumberarray); break;
      case T_LINK: serial = p + offsetof(LinkHealthRecord, serialnumberarray); break;
    }
    if (out[0]) {
      if (serial) learn(serial, out);
      return;
    }
    if (serial) {
      for (int i = 0; i < LORA_RELAY_NAMES; i++) if (_names[i][0] && memcmp(_nameSerial[i], serial, 8) == 0) { strcpy(out, _names[i]); return; }
      snprintf(out, 17, "%02X%02X%02X", serial[0], serial[1], serial[2]);
    } else {
      strcpy(out, "packet");
    }
  }

  void tick(const char *name, uint8_t action, int rssi) {
    strncpy(lastName, name, sizeof(lastName) - 1);
    lastName[sizeof(lastName) - 1] = 0;
    lastAction = action;
    lastRssi = rssi;
    lastAt = millis();
    if (!lastAt) lastAt = 1;
  }
};
#endif
