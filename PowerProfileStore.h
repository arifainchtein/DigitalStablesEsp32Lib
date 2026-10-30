#ifndef POWERPROFILESTORE_H
#define POWERPROFILESTORE_H

#include "Arduino.h"
#include <PowerProfileRecord.h>

// Annabelle's relay table for PowerProfileRecord - same idea as DeviceIdentityStore: the LATEST
// record per device (serial number), held in RAM until the Pi collects it with AsyncData.
// Records are rare (after a reset, then daily), so a small table is plenty; when full, the oldest
// is replaced. 16 slots x ~56 bytes = ~0.9 KB.
//
// One line per record, read by PowerProfileDeserializer.java (Teleonome framework):
// PowerProfileDeserializer#<serialHex>#<version>#<reason>#<chemistry>#<cellsInSeries>
//   #<capacity_mAh>#<chargeEfficiency_pct>#<restFullVoltage_mV>#<restEmptyVoltage_mV>
//   #<absorptionVoltage_mV>#<cutoffVoltage_mV>#<batteryTempSource>#<batteryCurrentLsb_uA>
//   #<panelCurrentLsb_uA>#<sleepCurrent_uA>#<unmeasuredBaseline_uA>#<rssi>#<snr>
//   #<receivedAgeSeconds>
// = 20 tokens. serialHex uses the same per-byte HEX (no leading zeros) convention as the other
// serializers.
#define POWER_PROFILE_STORE_SLOTS 16

struct PowerProfileSlot {
  PowerProfileRecord record;
  float rssi;
  float snr;
  uint32_t receivedMillis;
  uint32_t arrival;   // store order, for oldest-first replacement; 0 = slot never used
  bool pending;       // not yet relayed to the Pi
};

class PowerProfileStore {
public:
  void store(const PowerProfileRecord& record, float rssi, float snr);
  int pendingCount() const;
  // One PowerProfileDeserializer line per pending record, then marks them relayed.
  int pushPendingToSerial(HardwareSerial& serial);

private:
  PowerProfileSlot _slots[POWER_PROFILE_STORE_SLOTS] = {};
  uint32_t _arrivalCounter = 0;
};

#endif
