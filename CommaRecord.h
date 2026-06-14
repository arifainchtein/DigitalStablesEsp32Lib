#ifndef COMMA_RECORD_H
#define COMMA_RECORD_H

// Operating status codes shared across all solar-powered Digital Stables devices.
#define OPERATING_STATUS_SLEEP    1  // sleeping between LoRa pulses
#define OPERATING_STATUS_NO_LED   2  // awake but LEDs off (low battery)
#define OPERATING_STATUS_FULL_MODE 3 // normal daytime operation
#define OPERATING_STATUS_CLOUDY   4  // overcast — reduced charging, sleep between pulses
#define OPERATING_STATUS_COMMA    5  // battery critically low — permanent deep sleep

#define COMMA_LOG_FILE        "/comma_log.dat"
#define COMMA_LOG_MAX_RECORDS  650  // 650 × 31 bytes = ~20 KB — covers 1 full winter week

// Single COMMA-mode voltage reading, used for both LittleFS storage and LoRa transmission.
// Transport fields (totpcode, index, total, checksum) are 0 when stored; populated before send.
// sizeof = 4+4+8+8+4+1+1+1 = 31 bytes on ESP32 — must remain unique vs other LoRa packet sizes.
#pragma pack(push, 1)
struct CommaRecord {
  uint32_t time;            // Unix-seconds of this reading
  float    voltage;         // battery voltage at this reading
  char     devicename[8];   // device short name (null-terminated, max 7 chars)
  uint8_t  serialnumber[8]; // device serial number
  long     totpcode;        // TOTP auth code (0 when stored, set before LoRa send)
  uint8_t  index;           // 0-based position in batch (set before LoRa send)
  uint8_t  total;           // total records in batch, saturated at 255 (set before LoRa send)
  uint8_t  checksum;        // XOR over all bytes except this field (set before LoRa send)
};
#pragma pack(pop)

#endif
