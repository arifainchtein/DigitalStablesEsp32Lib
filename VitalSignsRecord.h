#ifndef VITALSIGNSRECORD_H
#define VITALSIGNSRECORD_H

#include <stdint.h>

// Device vital signs - reset, sleep, power and LoRa-TX telemetry, sent by every LoRa device
// (Daffodil, Langley, Chinampa) after its data pulse and relayed by Annabelle to the Teleonome.
// Replaces the old on-demand DiagnosticRecord (TX_CURRENT / I2C_STATUS), retired 2026-10-05 -
// see VitalSigns_Design.pdf (Projects/Annabelle) for the full design.
//
// Counters are RUNNING TOTALS since lastResetTime, so a dropped packet only loses detail, never
// an event: the Teleonome works out the change between any two records it receives, and a reset
// shows up as resetCount going up (with the totals restarting from zero).
//
// Packed and fixed-width so sizeof is exactly the sum of the fields: 69 bytes. Receivers dispatch
// on packet size, so this must stay unique relative to every other LoRa struct (DigitalStablesData
// 244, ChinampaData 248, LangleyData, CommaRecord 31, RequestCommand, WeatherForecastUpdate,
// GraveyardShiftUpdate 19, ...) - Annabelle.ino static_asserts that.

#define VITAL_SIGNS_VERSION 2   // 2: lastAwakeSec in seconds (version 1 sent milliseconds, capped at 65.5 s)

#pragma pack(push, 1)
struct VitalSignsRecord {
  long     totpcode = 0;            // TOTP auth, like every other uplink record
  uint8_t  serialnumberarray[8];    // device identity (DS18B20 ROM address)
  uint8_t  version = VITAL_SIGNS_VERSION;
  uint32_t firmwareBuild = 0;       // build stamp YYMMDDhh, see VitalSignsTracker::buildStamp()
  // --- resets (NVS - survive power loss) ---
  uint16_t resetCount = 0;          // REAL resets only (reason != DEEPSLEEP)
  uint8_t  lastResetReason = 0;     // esp_reset_reason_t of the last real reset
  uint32_t lastResetTime = 0;       // RTC epoch of that boot
  // --- sleep (running totals since lastResetTime) ---
  uint32_t wakeCount = 0;           // every deep-sleep wake, whatever happened next
  uint16_t earlyWakeCount = 0;      // woke before the intended time and went back (TPL5010 path)
  uint16_t commaWakeCount = 0;      // woke, battery too low (COMMA recheck), back to sleep
  uint32_t awakeSecondsTotal = 0;   // total time awake (for a device that never sleeps: uptime)
  uint32_t sleptSecondsTotal = 0;   // total sleep requested (sum of sleepTime)
  uint8_t  lastWakeCause = 0;       // esp_sleep_wakeup_cause_t of this wake
  int16_t  lastWakeDriftSec = 0;    // actual wake (RTC) minus intended wake time
  uint16_t lastAwakeSec = 0;        // duration of the previous full awake cycle, rounded (saturates at ~18 h)
  // --- power ---
  uint16_t wakeVoltage_mV = 0;      // battery right after wake, before WiFi/LoRa load (0 = n/a)
  uint16_t minVoltageSinceReport_mV = 0;  // lowest battery voltage seen since the last record (0 = n/a)
  // --- last LoRa TX summary (replaces DIAGNOSTIC_TYPE_TX_CURRENT) ---
  uint16_t txDurationMs = 0;        // key-up -> TX done
  int16_t  txBatteryPre_mA = 0;     // drawn from the battery just before key-up (+ = discharging)
  int16_t  txBatteryPeak_mA = 0;    // highest battery draw during TX
  int16_t  txBatteryPost_mA = 0;    // just after TX
  int16_t  txPanel_mA = -1;         // panel current during TX, -1 if the device has no panel sensor
  uint16_t txV50i_mV = 0;           // V50_I (panel side) at key-up, 0 if not measured
  uint16_t txMinVoltage_mV = 0;     // lowest battery voltage during TX (sag), 0 if not measured
  // --- radio / peripherals ---
  uint16_t loraTxFailCount = 0;     // running total of failed transmissions
  uint16_t seq = 0;                 // +1 per record sent -> receiver computes loss rate
  uint8_t  i2cDeviceMask = 0;       // device-specific "sensor found" bits (Daffodil: buildI2CStatusMask())
  uint8_t  checksum = 0;            // XOR over all other bytes
};
#pragma pack(pop)

#if defined(ESP32)
static_assert(sizeof(VitalSignsRecord) == 69, "VitalSignsRecord must be 69 bytes - receivers dispatch on packet size");
#endif

#endif
