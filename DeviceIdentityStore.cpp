#include <DeviceIdentityStore.h>

void DeviceIdentityStore::store(const DeviceIdentityRecord& record, float rssi, float snr) {
  int target = -1;
  int empty = -1;
  int oldest = -1;
  for (int i = 0; i < DEVICE_IDENTITY_STORE_SLOTS; i++) {
    DeviceIdentitySlot& s = _slots[i];
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

  DeviceIdentitySlot& s = _slots[target];
  s.record = record;
  s.rssi = rssi;
  s.snr = snr;
  s.receivedMillis = millis();
  s.arrival = ++_arrivalCounter;
  s.pending = true;
}

int DeviceIdentityStore::pendingCount() const {
  int count = 0;
  for (int i = 0; i < DEVICE_IDENTITY_STORE_SLOTS; i++) {
    if (_slots[i].arrival != 0 && _slots[i].pending) count++;
  }
  return count;
}

// Prints at most size chars (the record's char arrays aren't guaranteed to be terminated if a
// sender filled them completely), with the line delimiters replaced.
void DeviceIdentityStore::printField(HardwareSerial& serial, const char* value, size_t size) {
  for (size_t i = 0; i < size && value[i] != 0; i++) {
    char c = value[i];
    serial.print((c == '#' || c == '|' || c < 32 || c > 126) ? '_' : c);
  }
}

int DeviceIdentityStore::pushPendingToSerial(HardwareSerial& serial) {
  int written = 0;
  for (int i = 0; i < DEVICE_IDENTITY_STORE_SLOTS; i++) {
    DeviceIdentitySlot& s = _slots[i];
    if (s.arrival == 0 || !s.pending) continue;
    const DeviceIdentityRecord& r = s.record;
    serial.print("DeviceIdentityDeserializer");
    serial.print(F("#"));
    for (int b = 0; b < (int)sizeof(r.serialnumberarray); b++) {
      serial.print(r.serialnumberarray[b], HEX);
    }
    serial.print(F("#")); serial.print(r.version);
    serial.print(F("#")); serial.print(r.firmwareBuild);
    serial.print(F("#")); serial.print(r.labelBuild);
    serial.print(F("#")); serial.print(r.commissionDate);
    serial.print(F("#")); serial.print(r.reason);
    serial.print(F("#")); printField(serial, r.name, sizeof(r.name));
    serial.print(F("#")); printField(serial, r.firmware, sizeof(r.firmware));
    serial.print(F("#")); printField(serial, r.pcbs, sizeof(r.pcbs));
    serial.print(F("#")); printField(serial, r.powerSource, sizeof(r.powerSource));
    serial.print(F("#")); printField(serial, r.battery, sizeof(r.battery));
    serial.print(F("#")); serial.print(s.rssi, 1);
    serial.print(F("#")); serial.print(s.snr, 1);
    serial.print(F("#")); serial.println((millis() - s.receivedMillis) / 1000UL);
    s.pending = false;
    written++;
  }
  return written;
}
