#ifndef DEVICECOMMANDRECORD_H
#define DEVICECOMMANDRECORD_H

#include <stdint.h>
#include <string.h>

// A command addressed to ONE device by serial number (2026-10-14), sent by Annabelle (its web page)
// or Paula (PaulaDeployer). First use: turn a Daffodil's WiFi on for a while, or off, so its web
// page can be reached even when the battery is below the automatic WiFi start (3.35 V for 30 s).
//
// Delivery: an awake device hears it straight away; a sleeping one only listens briefly after its
// VitalSigns packet, so senders hold it and resend after that packet (DeviceCommandQueue.h).
//
// Security, same rule as the WeatherForecastUpdate time sync: totpcode is the code for senderTime,
// and the device accepts a senderTime only if it is later than the last one it accepted (no
// replays) and close to its own clock.
//
// Packed: 4+4+8+1+1+1+4+1 = 24 bytes. Receivers dispatch on packet size, so this must stay unique
// relative to every other LoRa struct - Annabelle.ino static_asserts that.

#define DEVICE_COMMAND_WIFI_ON  1   // minutes = how long (0 = the device's default)
#define DEVICE_COMMAND_WIFI_OFF 2
// Signal survey ping (2026-11-03, Paula's Nearby Devices map): "who can hear me from here?". Sent by
// Paula to every device at once - serialnumberarray all 0xFF - with reserved[0..1] = Paula's ping
// number (little endian). Every listening device that accepts it answers with a LinkAckRecord
// (LinkAckRecord.h). Same code/clock checks as the other commands, but nothing is saved: the replay
// check is in RAM (a repeated ping only costs an extra answer). Firmware older than this ignores it,
// since the serial is not its own.
#define DEVICE_COMMAND_PING     3

#define DEVICE_COMMAND_SENDER_ANNABELLE 1
#define DEVICE_COMMAND_SENDER_PAULA     2

// True for a ping addressed to everyone.
inline bool isBroadcastPing(const uint8_t *serial, uint8_t command) {
  if (command != DEVICE_COMMAND_PING) return false;
  for (int i = 0; i < 8; i++) if (serial[i] != 0xFF) return false;
  return true;
}

#pragma pack(push, 1)
struct DeviceCommandRecord {
  long     totpcode = 0;
  uint32_t senderTime = 0;            // sender's getTimeForCodeGeneration(), stamped just before transmit
  uint8_t  serialnumberarray[8] = {0};  // the target device
  uint8_t  command = 0;               // DEVICE_COMMAND_*
  uint8_t  minutes = 0;
  uint8_t  sender = 0;                // DEVICE_COMMAND_SENDER_*
  uint8_t  reserved[4] = {0};
  uint8_t  checksum = 0;
};
#pragma pack(pop)

#if defined(ESP32)
static_assert(sizeof(DeviceCommandRecord) == 24, "DeviceCommandRecord must be 24 bytes - receivers dispatch on packet size");
#endif

// XOR of every byte before the checksum (it is the last byte).
inline uint8_t deviceCommandChecksum(const DeviceCommandRecord &r) {
  const uint8_t *p = (const uint8_t *)&r;
  uint8_t c = 0;
  for (size_t i = 0; i < sizeof(DeviceCommandRecord) - 1; i++) c ^= p[i];
  return c;
}

#endif
