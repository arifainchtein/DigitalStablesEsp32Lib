#ifndef DEVICEIDENTITYTRACKER_H
#define DEVICEIDENTITYTRACKER_H

#include "Arduino.h"
#include <DeviceIdentityRecord.h>
#include <Esp32SecretManager.h>

// Device-side scheduling for DeviceIdentityRecord (Daffodil; Langley/Chinampa can adopt it later).
//
// A record is due when any of these is true:
//   - a real reset happened and its identity record hasn't been delivered yet
//   - SetProductDefinition was received since the last delivery (Esp32SecretManager sets this)
//   - the last delivery was DEVICE_IDENTITY_INTERVAL_SEC ago or more (or the clock went backwards)
// The pending reason and last delivery time live in NVS (namespace "productdef", next to the
// product definition itself), so a deep-sleeping device keeps trying on each pulse until one
// gets through, and NVS is written at most a few times a day.
//
// Typical use:
//   setup():         deviceIdentity.begin(vitalSigns.wasRealReset());   // after recordBoot()
//   after VitalSigns: if (deviceIdentity.isDue(nowEpoch)) {
//                       DeviceIdentityRecord r = deviceIdentity.buildRecord(secretManager, serial, FIRMWARE_BUILD);
//                       if (sendMessage(r, false) == LORA_OK) deviceIdentity.markSent(nowEpoch);
//                     }

#define DEVICE_IDENTITY_INTERVAL_SEC 86400UL

class DeviceIdentityTracker {
public:
  void begin(bool realReset);
  bool isDue(uint32_t nowEpoch);
  // Fills everything except totpcode and checksum (the sketch's sendMessage() stamps those).
  DeviceIdentityRecord buildRecord(Esp32SecretManager &secretManager, const uint8_t serialnumber[8], uint32_t firmwareBuild);
  void markSent(uint32_t nowEpoch);

private:
  void loadState();
  bool _loaded = false;
  uint8_t _pendingReason = 0;
  uint32_t _lastSent = 0;
};

#endif
