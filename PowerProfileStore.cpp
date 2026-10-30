#include <PowerProfileStore.h>
#include "DeviceSerial.h"

void PowerProfileStore::store(const PowerProfileRecord& record, float rssi, float snr) {
  int target = -1;
  int empty = -1;
  int oldest = -1;
  for (int i = 0; i < POWER_PROFILE_STORE_SLOTS; i++) {
    PowerProfileSlot& s = _slots[i];
    if (s.arrival == 0) {
      if (empty < 0) empty = i;
      continue;
    }
    if (memcmp(s.record.serialnumberarray, record.serialnumberarray, sizeof(record.serialnumberarray)) == 0) {
      target = i;
      break;
    }
    if (oldest < 0 || s.arrival < _slots[oldest].arrival) oldest = i;
  }
  if (target < 0) target = (empty >= 0) ? empty : oldest;

  PowerProfileSlot& s = _slots[target];
  s.record = record;
  s.rssi = rssi;
  s.snr = snr;
  s.receivedMillis = millis();
  s.arrival = ++_arrivalCounter;
  s.pending = true;
}

int PowerProfileStore::pendingCount() const {
  int count = 0;
  for (int i = 0; i < POWER_PROFILE_STORE_SLOTS; i++) {
    if (_slots[i].arrival != 0 && _slots[i].pending) count++;
  }
  return count;
}

int PowerProfileStore::pushPendingToSerial(HardwareSerial& serial) {
  int written = 0;
  for (int i = 0; i < POWER_PROFILE_STORE_SLOTS; i++) {
    PowerProfileSlot& s = _slots[i];
    if (s.arrival == 0 || !s.pending) continue;
    const PowerProfileRecord& r = s.record;
    serial.print("PowerProfileDeserializer");
    serial.print(F("#"));
    serial.print(DeviceSerial::toString(r.serialnumberarray));  // the efuse MAC, as the factory records it
    serial.print(F("#")); serial.print(r.version);
    serial.print(F("#")); serial.print(r.reason);
    serial.print(F("#")); serial.print(r.chemistry);
    serial.print(F("#")); serial.print(r.cellsInSeries);
    serial.print(F("#")); serial.print(r.capacity_mAh);
    serial.print(F("#")); serial.print(r.chargeEfficiency_pct);
    serial.print(F("#")); serial.print(r.restFullVoltage_mV);
    serial.print(F("#")); serial.print(r.restEmptyVoltage_mV);
    serial.print(F("#")); serial.print(r.absorptionVoltage_mV);
    serial.print(F("#")); serial.print(r.cutoffVoltage_mV);
    serial.print(F("#")); serial.print(r.batteryTempSource);
    serial.print(F("#")); serial.print(r.batteryCurrentLsb_uA);
    serial.print(F("#")); serial.print(r.panelCurrentLsb_uA);
    serial.print(F("#")); serial.print(r.sleepCurrent_uA);
    serial.print(F("#")); serial.print(r.unmeasuredBaseline_uA);
    serial.print(F("#")); serial.print(s.rssi, 1);
    serial.print(F("#")); serial.print(s.snr, 1);
    serial.print(F("#")); serial.println((millis() - s.receivedMillis) / 1000UL);
    s.pending = false;
    written++;
  }
  return written;
}
