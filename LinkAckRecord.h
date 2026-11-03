#ifndef LINKACKRECORD_H
#define LINKACKRECORD_H

#include <stdint.h>
#include <string.h>

// Answer to Paula's signal-survey ping (2026-11-03, DEVICE_COMMAND_PING in DeviceCommandRecord.h).
//
// Paula, carried around the farm, pings; every listening device answers with one of these:
//   - probeRssi/probeSnrQ: how strongly it heard the ping, i.e. how a device placed where Paula is
//     standing would be heard by it. Paula pings at a Daffodil's 5 dBm by default, so for Annabelle's
//     answer this IS the answer to "would a Daffodil here reach the gateway?".
//   - latitude/longitude: where the device says it is (its configured position; 0/0 = not set), so
//     the map can place it - Annabelle's forecast carries none.
//   - gatewayRssi/gatewaySnrQ/gatewayAgeSec (not Annabelle): how well this device hears Annabelle's
//     weather forecast - with probeRssi, whether it could be the relay for that spot.
// Paula measures the answer's own RSSI/SNR too (the other direction).
//
// Answers are spread out so they don't collide: Annabelle first, the rest in a slot picked from
// their serial (linkAckDelayMs). Paula listens LINK_ACK_LISTEN_MS after pinging.
//
// No code: an answer only reports, it makes nobody do anything. The checksum catches damage.
//
// Packed: 1+1+4+2+8+16+1+2+1+2+1+2+4+4+2+1 = 52 bytes. Receivers dispatch on packet size, so this must
// stay unique relative to every other LoRa struct - Annabelle.ino static_asserts that.

#define LINK_ACK_MAGIC 0xA7

#define LINK_ACK_KIND_ANNABELLE 1
#define LINK_ACK_KIND_DAFFODIL  2

#define LINK_ACK_GATEWAY_UNKNOWN_AGE 0xFFFF   // never heard Annabelle (or it is Annabelle)

#define LINK_ACK_FIRST_SLOT_MS 150    // Annabelle
#define LINK_ACK_SLOT0_MS      600    // first slot for everyone else
#define LINK_ACK_SLOT_MS       380    // > airtime of 52 bytes at SF9/125 kHz (~330 ms)
#define LINK_ACK_SLOTS         10
#define LINK_ACK_LISTEN_MS     5500   // last slot 600 + 9*380 = 4020 ms, + airtime + margin

#pragma pack(push, 1)
struct LinkAckRecord {
  uint8_t  magic = LINK_ACK_MAGIC;
  uint8_t  version = 1;
  uint32_t probeTime = 0;             // the ping's senderTime - which ping this answers
  uint16_t probeSeq = 0;              // the ping's number (its reserved[0..1])
  uint8_t  serialnumberarray[8] = {0};
  char     devicename[16] = {0};
  uint8_t  kind = 0;                  // LINK_ACK_KIND_*
  int16_t  probeRssi = 0;             // dBm, the ping as heard here
  int8_t   probeSnrQ = 0;             // SNR x 4
  int16_t  gatewayRssi = 0;           // dBm, Annabelle's last forecast as heard here (0 = unknown)
  int8_t   gatewaySnrQ = 0;
  uint16_t gatewayAgeSec = LINK_ACK_GATEWAY_UNKNOWN_AGE;  // how long ago that forecast was
  float    latitude = 0;              // degrees, the device's configured position
  float    longitude = 0;
  uint8_t  reserved[2] = {0};
  uint8_t  checksum = 0;
};
#pragma pack(pop)

#if defined(ESP32)
static_assert(sizeof(LinkAckRecord) == 52, "LinkAckRecord must be 52 bytes - receivers dispatch on packet size");
#endif

// XOR of every byte before the checksum (it is the last byte).
inline uint8_t linkAckChecksum(const LinkAckRecord &r) {
  const uint8_t *p = (const uint8_t *)&r;
  uint8_t c = 0;
  for (size_t i = 0; i < sizeof(LinkAckRecord) - 1; i++) c ^= p[i];
  return c;
}

inline bool linkAckValid(const LinkAckRecord &r) {
  return r.magic == LINK_ACK_MAGIC && linkAckChecksum(r) == r.checksum;
}

// When to answer, counted from hearing the ping: Annabelle first, the rest in one of LINK_ACK_SLOTS
// slots from their serial (FNV-1a), so two devices only collide when their serials share a slot.
inline uint32_t linkAckDelayMs(const uint8_t *serial, bool isAnnabelle) {
  if (isAnnabelle) return LINK_ACK_FIRST_SLOT_MS;
  uint32_t h = 2166136261u;
  for (int i = 0; i < 8; i++) { h ^= serial[i]; h *= 16777619u; }
  return LINK_ACK_SLOT0_MS + (h % LINK_ACK_SLOTS) * LINK_ACK_SLOT_MS;
}

// Sets the magic and the checksum - call last, after filling every field.
inline void linkAckSeal(LinkAckRecord &r) {
  r.magic = LINK_ACK_MAGIC;
  r.checksum = linkAckChecksum(r);
}

#endif
