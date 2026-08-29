#ifndef LANGLEYDATA_H
#define LANGLEYDATA_H
struct LangleyData{

  char devicename[16];
  // Was char[4], one byte too small for the project's own 4-character shortname convention
  // (e.g. "LWES" in Langley.ino's SetDeviceSensorConfig comment) - toCharArray(buf, sizeof(buf))
  // truncates to sizeof-1 chars, so 4-char shortnames were silently landing as 3 chars + null.
  // Bumped to fit the full 4 characters - see conversation 2026-07-26.
  char deviceshortname[5];
  // Unique per-device ID, taken from the onboard DS18B20's factory-programmed OneWire ROM
  // address (see boardTempSensor.getAddress() in Langley.ino) - same convention as
  // serialnumberarray on ChinampaData/DigitalStablesData/etc. Lets a new unit (e.g. a future
  // "Langley_East") register itself automatically, and gives devicename a second, corruption-
  // resistant field to be cross-checked against - see conversation 2026-07-20.
  uint8_t serialnumberarray[8];
  long secondsTime=0L;
  float externalBatteryVoltage=0.0;
  // Energizer's own battery (separate from Langley's own system battery above) - read from
  // the shield's INA219 @ 0x44, see Langley.ino's readSensorData().
  float energizerBatteryVoltage=0.0;
  float energizerBatteryCurrent=0.0;  // mA, computed from shunt voltage - see comment in readSensorData()
  float temperature=0.0;
  float fenceVoltage=0.0;     // most recent single pulse, kV - for live display only
  // Aggregated over the pulses seen since the previous LoRa send (see loraSendIntervalSeconds
  // in Langley.ino) - sent instead of/alongside fenceVoltage since several pulses (~1/sec)
  // happen between each transmission.
  float fenceVoltageMin=0.0;
  float fenceVoltageMax=0.0;
  float fenceVoltageAvg=0.0;
  uint8_t pulseCount=0;       // number of pulses aggregated into the min/max/avg above
  float rssi=0;
  float snr=0;
  // operatingStatus uses the OPERATING_STATUS_* constants from CommaRecord.h, shared with
  // every other solar Digital Stables device (Daffodil etc): 1=SLEEP 2=NO_LED 3=FULL_MODE
  // 4=CLOUDY 5=COMMA. (Older Langley firmware used a separate 0=Comma/1=WPS/2=Active scheme -
  // this struct has been updated to the shared taxonomy, see Langley.ino's updateOperatingMode().)
  float operatingStatus=0;
  float wpsFrequencySeconds=1800;
  float maxWPSVoltage=3.75;
  float minWPSVoltage=3.25;
  long secondsSinceLastPulse;
  // Valentino onboard power monitors: U8 (solar input, 0x40) and U9 (battery, 0x41).
  // Shunt is 2mΩ on both; current computed as getShuntVoltage_mV() / 0.002.
  float solarVoltage = 0.0f;      // V  - solar panel input bus (U8)
  float solarCurrentMa = 0.0f;    // mA - solar input current (positive = panel producing)
  float batteryVoltage = 0.0f;    // V  - battery bus (U9)
  float batteryCurrentMa = 0.0f;  // mA - battery current (positive = charging)
  // Estimated hours of runtime remaining at the current discharge rate - same approach as
  // Daffodil.ino's estimatedRuntime (state-of-charge from GeneralFunctions x battery capacity,
  // divided by present discharge current). 0 when charging or unknown. See readSensorData().
  float estimatedRuntime = 0.0f;
  // 0 = lead-acid (flooded/AGM/gel), 1 = LiFePO4 - matches the BatteryChemistry enum in
  // BatteryPowerManager.h (DigitalStablesEsp32Lib). Read from hardware at boot, not user-set -
  // see detectBatteryChemistry() in Langley.ino.
  uint8_t batteryChemistry = 0;
  // Voltage of the PCF8563 RTC's backup coin cell (CR1220), through Valentino's R41(1M)/R46(2M)
  // divider on GPIO36 - see RTC_BATT_VOLT in Langley.ino. -1 = ADC pinned near VCC (floating pin /
  // divider not installed / cell missing), not a real reading.
  float rtcBatVolt = 0.0f;
  // Coulomb-counted discharge since the last sunset, in mAh - resets when daytime solar
  // efficiency resumes (>0.3). Predictive: each sleep's contribution is estimated from the
  // battery current measured right before entering that sleep (goToSleep() in Langley.ino),
  // since the ESP32 can't sample anything during deep sleep itself. Added for the overnight
  // battery-sizing calibration run - see conversation 2026-07-14.
  float overnightMah = 0.0f;

  // --- Fence topology mapping - see conversation 2026-07-26 ---
  // deviceshortname of the unit immediately upstream toward the energizer (the id-0 node).
  // Empty string = this unit IS the id-0/energizer node. Set via Langley.ino's SetParent#
  // serial command, persisted separately from devicename/deviceshortname - see
  // Esp32SecretManager::{save,get}TopologyConfig(). The webapp/Annabelle reconstruct the whole
  // tree from every unit's (deviceshortname, parentShortname) pair - no compass/branch code is
  // computed or stored on-device beyond the single branchLabel byte below.
  char parentShortname[5];
  // Single-character branch label, only ever set on a node that is one of *several* children of
  // the same parent (a real Y-split) - the installer picks one letter there to distinguish that
  // child from its siblings. A node with only one child needs no label at all: consumers derive
  // it by inheriting the parent's label unchanged. This avoids needing lat/lon to compute a
  // compass bearing, and avoids the earlier compass-code scheme's depth cap - see conversation
  // 2026-07-26.
  char branchLabel[2];
  // Location, degrees - float (not double), matching DigitalStablesData.h's latitude/longitude/
  // altitude fields. ~1m precision at these magnitudes, plenty for topology mapping. Loaded once
  // at boot from the same lat/lon SolarInfo already uses (see Langley.ino's setup()) - not
  // independently configured.
  float latitude;
  float longitude;
  float altitude;

  uint8_t checksum=0; // XOR checksum over every other byte, see calculateChecksum() in Langley.ino
};
#endif