#ifndef VITALSIGNSSTORE_H
#define VITALSIGNSSTORE_H

#include "Arduino.h"
#include <VitalSignsRecord.h>
#include <VitalSignsSerializer.h>

// Annabelle's relay table for VitalSignsRecord: the LATEST record per device (serial number),
// held in RAM until the Pi collects it with AsyncData. No queue and no flash - records carry
// running totals, so only the newest one per device matters.
//
// Sized for the whole fleet, not one poll's worth: while the Pi is down every device's latest
// record has to wait here. 64 slots x ~88 bytes = ~5.6 KB. When full, the oldest record is
// replaced (never the incoming one dropped).
#define VITAL_SIGNS_STORE_SLOTS 64

struct VitalSignsSlot {
  VitalSignsRecord record;
  float rssi;
  float snr;
  uint32_t receivedMillis;  // millis() when it arrived - relayed as an age, see VitalSignsSerializer.h
  uint32_t arrival;   // store order, for oldest-first replacement; 0 = slot never used
  bool pending;       // not yet relayed to the Pi
};

class VitalSignsStore {
public:
  void store(const VitalSignsRecord& record, float rssi, float snr);
  int pendingCount() const;
  // One VitalSignsDeserializer line per pending record, then marks them relayed.
  // Returns how many lines were written.
  int pushPendingToSerial(HardwareSerial& serial);

private:
  VitalSignsSlot _slots[VITAL_SIGNS_STORE_SLOTS] = {};
  uint32_t _arrivalCounter = 0;
  VitalSignsSerializer _serializer;
};

#endif
