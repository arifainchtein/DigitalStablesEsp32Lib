#ifndef POWERPROFILERECORD_H
#define POWERPROFILERECORD_H

#include <stdint.h>

// Power profile - the PHYSICAL parameters of a product's battery, charger and current sensing,
// product-neutral, so the Cerebellum can turn VitalSignsRecord's battery ledger into SOC for any
// product (Daffodil, Langley, Gloria, ...) without per-product code or constants.
//
// These are hardware facts: each product sketch fills them from its own #defines, and a battery
// or panel change means a reflash. Sent like DeviceIdentityRecord - after a real reset and once a
// day - so the Cerebellum always holds the profile of the firmware actually running.
//
// Packed: 4+8+1+1+1+1+4+1+2+2+2+2+1+2+2+2+2+1 = 39 bytes. Receivers dispatch on packet size, so
// this must stay unique relative to every other LoRa struct - Annabelle.ino static_asserts that.

#define POWER_PROFILE_VERSION 1

#define BATTERY_CHEMISTRY_UNKNOWN   0
#define BATTERY_CHEMISTRY_LIFEPO4   1
#define BATTERY_CHEMISTRY_LEAD_ACID 2   // flooded / AGM / gel
#define BATTERY_CHEMISTRY_LI_ION    3   // NMC / LCO
#define BATTERY_CHEMISTRY_SUPERCAP  4

#define BATTERY_TEMP_SOURCE_NONE    0   // Cerebellum never temperature-compensates
#define BATTERY_TEMP_SOURCE_BATTERY 1   // sensor on the battery itself (e.g. Langley's thermistor terminal)
#define BATTERY_TEMP_SOURCE_BOARD   2   // board/enclosure sensor used as a proxy

#pragma pack(push, 1)
struct PowerProfileRecord {
  long     totpcode = 0;
  uint8_t  serialnumberarray[8];
  uint8_t  version = POWER_PROFILE_VERSION;
  uint8_t  reason = 0;                 // DEVICE_IDENTITY_REASON_* (same send schedule as DeviceIdentityRecord)
  // --- battery ---
  uint8_t  chemistry = 0;              // BATTERY_CHEMISTRY_* - selects the voltage->SOC curve shape
  uint8_t  cellsInSeries = 0;          // Daffodil 1 (3.2 V), 12 V LiFePO4 4, 12 V lead-acid 6
  uint32_t capacity_mAh = 0;           // usable capacity (Daffodil 600, Langley 2,500-24,000)
  uint8_t  chargeEfficiency_pct = 0;   // coulombic efficiency, 0 = Cerebellum default for the chemistry
  uint16_t restFullVoltage_mV = 0;     // pack voltage at rest = 100%, 0 = default for chemistry x cells
  uint16_t restEmptyVoltage_mV = 0;    // pack voltage at rest = 0%,  0 = default for chemistry x cells
  uint16_t absorptionVoltage_mV = 0;   // charger's full-charge voltage (Langley AGM jumper 14300,
                                       // Daffodil 3600); held while charge current tapers = 100%. 0 = unknown
  uint16_t cutoffVoltage_mV = 0;       // product's own forced-sleep / COMMA threshold
  uint8_t  batteryTempSource = 0;      // BATTERY_TEMP_SOURCE_* - pairs with VitalSignsRecord.batteryTemp_dC
  // --- measurement ---
  uint16_t batteryCurrentLsb_uA = 0;   // smallest current step the battery sensor resolves
                                       //   (Daffodil 50 mOhm = 200, Valentino 2 mOhm = 5000)
  uint16_t panelCurrentLsb_uA = 0;     // same for the panel/input sensor, 0 = no panel sensor
  uint16_t sleepCurrent_uA = 0;        // whole-board deep-sleep draw, bench-measured (0 = not measured yet)
  uint16_t unmeasuredBaseline_uA = 0;  // steady awake load below batteryCurrentLsb (invisible to the ledger)
  uint8_t  checksum = 0;
};
#pragma pack(pop)

#if defined(ESP32)
static_assert(sizeof(PowerProfileRecord) == 39, "PowerProfileRecord must be 39 bytes - receivers dispatch on packet size");
#endif

#endif
