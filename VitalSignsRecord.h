#ifndef VITALSIGNSRECORD_H
#define VITALSIGNSRECORD_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Device vital signs - reset, sleep, power and LoRa-TX telemetry, sent by every LoRa device
// (Daffodil, Langley, Chinampa) after its data pulse and relayed by Annabelle to the Teleonome.
// Replaces the old on-demand DiagnosticRecord (TX_CURRENT / I2C_STATUS), retired 2026-10-05 -
// see VitalSigns_Design.pdf (Projects/Annabelle) for the full design.
//
// Counters are RUNNING TOTALS since lastResetTime, so a dropped packet only loses detail, never
// an event: the Teleonome works out the change between any two records it receives, and a reset
// shows up as resetCount going up (with the totals restarting from zero).
//
// Packed and fixed-width so sizeof is exactly the sum of the fields: 97 bytes (v4). Receivers
// dispatch on packet size, so this must stay unique relative to every other LoRa struct
// (DigitalStablesData 244, ChinampaData 248, LangleyData, CommaRecord 31, RequestCommand,
// WeatherForecastUpdate, GraveyardShiftUpdate 19, PowerProfileRecord 39, ...) - Annabelle.ino
// static_asserts that. Version 2 (69 bytes, no battery ledger) and version 3 (89 bytes, no WiFi
// totals) records are still accepted by Annabelle during changeovers: VitalSignsRecordV2/V3 below
// are their frozen layouts.

#define VITAL_SIGNS_VERSION 4   // 4: WiFi totals (seconds on, battery charge out while on) - 2026-10-14
                                // 3: battery ledger (charge in/out, panel energy, sampler coverage) + battery temperature
                                // 2: lastAwakeSec in seconds (version 1 sent milliseconds, capped at 65.5 s)

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
  uint16_t loraTxFailCount = 0;     // running total of failed transmissions; from 2026-10-15 (Daffodil)
                                    // also sends abandoned on a busy channel - see VitalSignsTracker::noteTxFailed()
  uint16_t seq = 0;                 // +1 per record sent -> receiver computes loss rate
  uint8_t  i2cDeviceMask = 0;       // device-specific "sensor found" bits (Daffodil: buildI2CStatusMask())
  // --- battery ledger (v3) - RUNNING TOTALS since lastResetTime, integrated on core 0 the whole
  //     time the device is awake (VitalSignsTracker::startLedger), from 68 ms-averaged current
  //     reads. Deep sleep is NOT covered (the ESP32 can't sample) - the Cerebellum adds it from
  //     sleptSecondsTotal x PowerProfileRecord.sleepCurrent_uA. ---
  uint32_t chargeIn_cmAh = 0;       // into the battery, 0.01 mAh units
  uint32_t dischargeOut_cmAh = 0;   // out of the battery, 0.01 mAh units
  uint32_t panelIn_dmWh = 0;        // energy from the panel/USB input, 0.1 mWh units; 0 if no panel sensor
  uint32_t ledgerMeasuredSec = 0;   // awake time the sampler actually covered (vs awakeSecondsTotal)
  uint16_t ledgerMaxGapMs = 0;      // longest gap between two samples since the last record (saturates)
  int16_t  batteryTemp_dC = INT16_MIN; // battery temperature, 0.1 C; INT16_MIN = no reading this cycle
  // --- WiFi (v4) - RUNNING TOTALS since lastResetTime from the same core-0 ledger, counted while
  //     the device's WiFi is on (VitalSignsTracker::setLedgerTag) - what WiFi costs the battery. ---
  uint32_t wifiOnSec = 0;           // seconds the ledger ran with WiFi on
  uint32_t wifiOut_cmAh = 0;        // battery charge out while WiFi was on, 0.01 mAh units
  uint8_t  checksum = 0;            // XOR over all other bytes
};

// Frozen version-2 layout (69 bytes): identical to the first 68 bytes of VitalSignsRecord, then
// the checksum. Annabelle receives these from devices not yet reflashed and upgrades them with
// vitalSignsFromV2() (ledger fields zero, batteryTemp_dC = no reading, version stays 2).
struct VitalSignsRecordV2 {
  uint8_t  fields[68];
  uint8_t  checksum = 0;
};

// Frozen version-3 layout (89 bytes): the first 88 bytes of VitalSignsRecord, then the checksum.
// Upgraded with vitalSignsFromV3() (WiFi totals zero, version stays 3).
struct VitalSignsRecordV3 {
  uint8_t  fields[88];
  uint8_t  checksum = 0;
};
#pragma pack(pop)

#if defined(ESP32)
static_assert(sizeof(VitalSignsRecord) == 97, "VitalSignsRecord must be 97 bytes - receivers dispatch on packet size");
static_assert(sizeof(VitalSignsRecordV3) == 89, "VitalSignsRecordV3 must stay 89 bytes - it is the version 3 wire format");
static_assert(offsetof(VitalSignsRecord, wifiOnSec) == 88, "v4 must only append fields after the v3 ones");
static_assert(sizeof(VitalSignsRecordV2) == 69, "VitalSignsRecordV2 must stay 69 bytes - it is the version 2 wire format");
static_assert(offsetof(VitalSignsRecord, chargeIn_cmAh) == 68, "v3 must only append fields after the v2 ones");
#endif

inline VitalSignsRecord vitalSignsFromV2(const VitalSignsRecordV2 &v2) {
  VitalSignsRecord r;
  memcpy((uint8_t *)&r, v2.fields, sizeof(v2.fields));
  r.chargeIn_cmAh = 0;
  r.dischargeOut_cmAh = 0;
  r.panelIn_dmWh = 0;
  r.ledgerMeasuredSec = 0;
  r.ledgerMaxGapMs = 0;
  r.batteryTemp_dC = INT16_MIN;
  r.wifiOnSec = 0;
  r.wifiOut_cmAh = 0;
  r.checksum = v2.checksum;
  return r;
}

inline VitalSignsRecord vitalSignsFromV3(const VitalSignsRecordV3 &v3) {
  VitalSignsRecord r;
  memcpy((uint8_t *)&r, v3.fields, sizeof(v3.fields));
  r.wifiOnSec = 0;
  r.wifiOut_cmAh = 0;
  r.checksum = v3.checksum;
  return r;
}

#endif
