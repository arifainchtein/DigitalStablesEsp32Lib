#include <VitalSignsStore.h>

void VitalSignsStore::store(const VitalSignsRecord& record, float rssi, float snr) {
  int target = -1;
  int empty = -1;
  int oldest = -1;
  for (int i = 0; i < VITAL_SIGNS_STORE_SLOTS; i++) {
    VitalSignsSlot& s = _slots[i];
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

  VitalSignsSlot& s = _slots[target];
  s.record = record;
  s.rssi = rssi;
  s.snr = snr;
  s.receivedMillis = millis();
  s.arrival = ++_arrivalCounter;
  s.pending = true;
}

int VitalSignsStore::pendingCount() const {
  int count = 0;
  for (int i = 0; i < VITAL_SIGNS_STORE_SLOTS; i++) {
    if (_slots[i].arrival != 0 && _slots[i].pending) count++;
  }
  return count;
}

int VitalSignsStore::pushPendingToSerial(HardwareSerial& serial) {
  int written = 0;
  for (int i = 0; i < VITAL_SIGNS_STORE_SLOTS; i++) {
    VitalSignsSlot& s = _slots[i];
    if (s.arrival == 0 || !s.pending) continue;
    _serializer.pushToSerial(serial, s.record, s.rssi, s.snr, (millis() - s.receivedMillis) / 1000UL);
    s.pending = false;
    written++;
  }
  return written;
}
