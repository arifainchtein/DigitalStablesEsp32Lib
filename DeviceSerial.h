#ifndef DEVICE_SERIAL_H
#define DEVICE_SERIAL_H

// A device's serial number is its ESP32 efuse MAC (decided 2026-10-30): the same value the factory
// records when it flashes serialnumberextractor and asks GetSerialNumber, which prints
// printf("%012llx", ESP.getEfuseMac()). Every device puts it in its packets' serialnumberarray
// with fill(), and every place that turns a serialnumberarray into text uses toString(), so the
// factory, the validation VM, Annabelle, the Teleonome and devices.digitalstables.com all see the
// same 12 lowercase hex characters (it used to be a DS18B20 ID on some devices, and the byte-by-byte
// String(x, HEX) dropped leading zeros).

#include <Arduino.h>
#include <esp_mac.h>

namespace DeviceSerial {

// The 6-byte efuse MAC in the order esp_efuse_mac_get_default gives it; bytes 6 and 7 stay 0.
inline void fill(uint8_t *serialnumberarray8) {
  memset(serialnumberarray8, 0, 8);
  esp_efuse_mac_get_default(serialnumberarray8);
}

// The factory's form: getEfuseMac() reads those bytes as a little-endian number, so %012llx prints
// them last byte first. Two lowercase hex digits per byte, always 12 characters.
inline String toString(const uint8_t *serialnumberarray8) {
  char buf[13];
  snprintf(buf, sizeof(buf), "%02x%02x%02x%02x%02x%02x",
           serialnumberarray8[5], serialnumberarray8[4], serialnumberarray8[3],
           serialnumberarray8[2], serialnumberarray8[1], serialnumberarray8[0]);
  return String(buf);
}

// False for an all-zero MAC (a packet with no serial filled in).
inline bool isValid(const uint8_t *serialnumberarray8) {
  for (int i = 0; i < 6; i++) {
    if (serialnumberarray8[i] != 0) return true;
  }
  return false;
}

}  // namespace DeviceSerial

#endif
