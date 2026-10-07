#ifndef DEVICEIDENTITYRECORD_H
#define DEVICEIDENTITYRECORD_H

#include <stdint.h>

// Device identity - the product definition a device was commissioned/flashed with (name, firmware
// label, PCBs, power source, battery) plus the build stamp of the code actually running. Added
// 2026-10-07 so the Teleonome (via Annabelle) can see which firmware/hardware each device runs.
//
// Deliberately NOT part of VitalSignsRecord: these strings would more than double every vital
// signs packet. Instead it is sent rarely (see DeviceIdentityTracker.h): once after every real
// reset, once a day, and right after SetProductDefinition.
//
// labelBuild is the firmwareBuild that was running when the firmware label was saved. If it
// differs from firmwareBuild the device has been flashed since, by some path that didn't update
// the label, so the label is stale - receivers show it as "not current".
//
// Packed: 4+8+1+4+4+4+1+20+20+40+16+24+1 = 147 bytes. Receivers dispatch on packet size, so this
// must stay unique relative to every other LoRa struct - Annabelle.ino static_asserts that.

#define DEVICE_IDENTITY_VERSION 1

#define DEVICE_IDENTITY_REASON_RESET 1         // first one after a real reset
#define DEVICE_IDENTITY_REASON_LABEL_CHANGED 2 // SetProductDefinition was received
#define DEVICE_IDENTITY_REASON_DAILY 3         // daily refresh

#pragma pack(push, 1)
struct DeviceIdentityRecord {
  long     totpcode = 0;            // TOTP auth, like every other uplink record
  uint8_t  serialnumberarray[8];    // device identity (DS18B20 ROM address)
  uint8_t  version = DEVICE_IDENTITY_VERSION;
  uint32_t firmwareBuild = 0;       // build stamp (YYMMDDhh) of the running code
  uint32_t labelBuild = 0;          // build stamp running when the label was saved, 0 = unknown
  uint32_t commissionDate = 0;      // epoch seconds, 0 = not set
  uint8_t  reason = 0;              // DEVICE_IDENTITY_REASON_*
  char     name[20];                // product definition name, e.g. "Daffodil V8"
  char     firmware[20];            // firmware label, e.g. "Daffodil v51"
  char     pcbs[40];                // e.g. "Daffodil Build 8,Wally Build 17"
  char     powerSource[16];         // e.g. "10W_USB"
  char     battery[24];             // e.g. "LiFePO4_600mAhr"
  uint8_t  checksum = 0;            // XOR over all other bytes
};
#pragma pack(pop)

#if defined(ESP32)
static_assert(sizeof(DeviceIdentityRecord) == 147, "DeviceIdentityRecord must be 147 bytes - receivers dispatch on packet size");
#endif

#endif
