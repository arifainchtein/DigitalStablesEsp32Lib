#ifndef DEVICEIDENTITYSTORE_H
#define DEVICEIDENTITYSTORE_H

#include "Arduino.h"
#include <DeviceIdentityRecord.h>

// Annabelle's relay table for DeviceIdentityRecord - same idea as VitalSignsStore: the LATEST
// record per device (serial number), held in RAM until the Pi collects it with AsyncData.
// Records are rare (daily per device), so a small table is plenty; when full, the oldest is
// replaced. 16 slots x ~170 bytes = ~2.7 KB.
//
// One line per record, read by DeviceIdentityDeserializer.java (Teleonome framework):
// DeviceIdentityDeserializer#<serialHex>#<version>#<firmwareBuild>#<labelBuild>#<commissionDate>
//   #<reason>#<name>#<firmware>#<pcbs>#<powerSource>#<battery>#<rssi>#<snr>#<receivedAgeSeconds>
// = 15 tokens. '#', '|' and control characters in the strings are replaced with '_' (they
// delimit the AsyncData lines). serialHex uses the same per-byte HEX (no leading zeros)
// convention as VitalSignsSerializer.
#define DEVICE_IDENTITY_STORE_SLOTS 16

struct DeviceIdentitySlot {
  DeviceIdentityRecord record;
  float rssi;
  float snr;
  uint32_t receivedMillis;
  uint32_t arrival;   // store order, for oldest-first replacement; 0 = slot never used
  bool pending;       // not yet relayed to the Pi
};

class DeviceIdentityStore {
public:
  void store(const DeviceIdentityRecord& record, float rssi, float snr);
  int pendingCount() const;
  // One DeviceIdentityDeserializer line per pending record, then marks them relayed.
  int pushPendingToSerial(HardwareSerial& serial);

private:
  static void printField(HardwareSerial& serial, const char* value, size_t size);
  DeviceIdentitySlot _slots[DEVICE_IDENTITY_STORE_SLOTS] = {};
  uint32_t _arrivalCounter = 0;
};

#endif
