#ifndef LINKHEALTHRECORD_H
#define LINKHEALTHRECORD_H

#include <stdint.h>
#include <string.h>

// What a device knows about its own LoRa link (2026-11-10, first sender: Chinampa). Sent after
// the VitalSignsRecord, so the gateway can tell link pathologies apart without anyone visiting:
//   - sends:      how many data packets the schedule called for, and what became of each one, so
//                 losses inside the firmware (skipped slots, busy channel, TX failure) are not
//                 mistaken for losses in the air. Annabelle compares dataSent with the data
//                 packets she actually received -> delivery ratio.
//   - noise:      the channel's RSSI sampled once a second while idle - local interference.
//   - gateway:    how this device hears Annabelle (her weather forecast). With how Annabelle
//                 hears this device, the two directions of one link: a bad antenna loses about
//                 the same dB both ways, a bad transmitter only on the way up.
//   - neighbours: how this device hears the devices next to it - the reference for "is it this
//                 device or this place?".
// Design: the "LoRa Link-Health Diagnosis" doc.
//
// Counters (sends, gatewayHeard, neighbour heard, rxCrcErrors) are running totals since boot, like
// VitalSignsRecord: a lost record loses no data, the receiver diffs consecutive records (seq,
// unsigned 16-bit arithmetic, so wrap-around is fine). The noise figures cover only the window
// since the previous record.
//
// Packed: 1+1+4+8+2+4 + 5*2 + 4 + 2+1+2+2 + 3*7 + 2+2 + 1+1+2 + 1 = 71 bytes. Receivers dispatch on
// packet size, so this must stay unique relative to every other LoRa struct - add it to
// Annabelle.ino's loraPacketSizes static_assert when Annabelle learns to read it.

#define LINK_HEALTH_MAGIC   0xA8
#define LINK_HEALTH_VERSION 1
#define LINK_HEALTH_NEIGHBOURS 3
#define LINK_HEALTH_NO_RSSI    0       // rssi field never measured
#define LINK_HEALTH_NO_NOISE   127     // int8 noise field: no sample in this window
#define LINK_HEALTH_UNKNOWN_AGE 0xFFFF // gatewayAgeSec: never heard Annabelle

#pragma pack(push, 1)
struct LinkHealthNeighbour {
  uint16_t tag = 0;          // last two bytes of the neighbour's serial (0 = slot unused)
  int16_t  rssi = 0;         // dBm, last packet heard from it
  int8_t   snrQ = 0;         // SNR x 4
  uint16_t heard = 0;        // packets heard from it since boot
};

struct LinkHealthRecord {
  uint8_t  magic = LINK_HEALTH_MAGIC;
  uint8_t  version = LINK_HEALTH_VERSION;
  long     totpcode = 0;               // TOTP auth, like every other uplink record
  uint8_t  serialnumberarray[8] = {0}; // device identity (efuse MAC, DeviceSerial)
  uint16_t seq = 0;                    // +1 per record
  uint32_t uptimeSec = 0;              // since boot - lets the receiver spot a reboot (counters restart)
  // --- data-packet sends, running totals since boot ---
  uint16_t dataScheduled = 0;          // sends the schedule called for (attempts + missed slots)
  uint16_t dataSent = 0;               // transmitted (endPacket ok)
  uint16_t busyExhausted = 0;          // channel busy on every retry: given up (LINK_HEALTH_BUSY) or, on
                                       // devices that then send anyway (Chinampa), sent regardless
  uint16_t txFailed = 0;               // endPacket returned false
  uint16_t slotsMissed = 0;            // the loop came round too late and a send slot passed
  // --- channel, this window ---
  int8_t   noiseMin = LINK_HEALTH_NO_NOISE;     // dBm, idle-channel RSSI, sampled once a second
  int8_t   noiseMedian = LINK_HEALTH_NO_NOISE;  // dBm (2 dB resolution)
  int8_t   noiseMax = LINK_HEALTH_NO_NOISE;     // dBm - a packet arriving during a sample shows here
  int8_t   busyMaxRssi = LINK_HEALTH_NO_NOISE;  // dBm, strongest reading that made a send back off
  // --- the gateway (Annabelle) as heard here ---
  int16_t  gatewayRssi = LINK_HEALTH_NO_RSSI;   // dBm, last forecast packet
  int8_t   gatewaySnrQ = 0;                     // SNR x 4
  uint16_t gatewayHeard = 0;                    // forecast packets heard since boot
  uint16_t gatewayAgeSec = LINK_HEALTH_UNKNOWN_AGE;
  // --- neighbours, most recently heard first ---
  LinkHealthNeighbour neighbours[LINK_HEALTH_NEIGHBOURS];
  // --- receive side, running totals since boot ---
  uint16_t rxCrcErrors = 0;            // DIO0 fired, but the packet failed its CRC
  uint16_t rxPackets = 0;              // good packets of any type
  // --- radio settings, so the receiver can compare devices fairly ---
  int8_t   txPowerDbm = 0;
  uint8_t  spreadingFactor = 0;
  uint16_t dataPacketBytes = 0;        // size of this device's data packet (airtime)
  uint8_t  checksum = 0;               // XOR over all other bytes
};
#pragma pack(pop)

#if defined(ESP32)
static_assert(sizeof(LinkHealthNeighbour) == 7, "LinkHealthNeighbour must be 7 bytes");
static_assert(sizeof(LinkHealthRecord) == 71, "LinkHealthRecord must be 71 bytes - receivers dispatch on packet size");
#endif

// XOR of every byte before the checksum (it is the last byte).
inline uint8_t linkHealthChecksum(const LinkHealthRecord &r) {
  const uint8_t *p = (const uint8_t *)&r;
  uint8_t c = 0;
  for (size_t i = 0; i < sizeof(LinkHealthRecord) - 1; i++) c ^= p[i];
  return c;
}

inline bool linkHealthValid(const LinkHealthRecord &r) {
  return r.magic == LINK_HEALTH_MAGIC && linkHealthChecksum(r) == r.checksum;
}

// Sets the magic and the checksum - call last, after filling every field (totpcode included).
inline void linkHealthSeal(LinkHealthRecord &r) {
  r.magic = LINK_HEALTH_MAGIC;
  r.checksum = linkHealthChecksum(r);
}

#endif
