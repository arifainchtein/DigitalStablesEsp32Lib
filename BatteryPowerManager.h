#ifndef LIBRARIES_DIGITALSTABLES_BATTERYPOWERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_BATTERYPOWERMANAGER_H_
#include <Arduino.h>
#include "RTCInfoRecord.h"
#include <SolarInfo.h>
#include <SolarPowerData.h>

// Battery chemistry + sleep-timing logic shared across every Valentino-base-PCB device
// (Langley, and later Gloria/Tucci) - a 12V lead-acid or 4S LiFePO4 pack plus one INA219 on the
// battery bus. Not a fit for Daffodil's existing PowerManager class, which is built around a
// supercapacitor + fixed LED power budget, not a battery chemistry model.
//
// Only 2 states, not one per lead-acid sub-chemistry (flooded/AGM/gel): the sub-chemistry
// distinction that actually matters - charge voltage - is already handled independently by the
// Valentino PCB's JP7/JP8 jumpers (13.7/14.3/15.0V), not by firmware. The firmware-side low-
// voltage cutoffs below only varied by ~0.1-0.2V across those three anyway, well within what
// load/temperature/wiring already move a single reading around by - not worth a 3-way split.
enum BatteryChemistry {
  CHEM_LEAD_ACID,  // flooded/AGM/gel - firmware treats all three the same (see above)
  CHEM_LIFEPO4     // 4S LiFePO4 (12.8V nominal)
};

class BatteryPowerManager {
public:
  BatteryPowerManager(BatteryChemistry chemistry, float capacityMah, SolarInfo& solarInfo);

  float sleepingVoltage() const;  // below this: stop normal operation, reduced-power SLEEP state
  float commaVoltage() const;     // below this: COMMA - battery critically low

  // State of charge %, dispatched to the chemistry-appropriate curve - see
  // GeneralFunctions::getLeadAcidStateOfCharge()/getBatteryStateOfCharge().
  uint8_t stateOfCharge(float busVoltage) const;

  // Estimated hours remaining at the given discharge rate. dischargeMa should be a positive
  // magnitude (already abs()'d by the caller); returns 0 if dischargeMa <= 0.
  float estimatedRuntimeHours(float busVoltage, float dischargeMa) const;

  // Same shape as Daffodil's PowerManager::calculateOptimalSleepTime(), minus the capacitor/LED
  // power-budget math: 60s while there's still meaningful daytime solar (efficiency > 0.3, this
  // covers "cloudy but still day"), otherwise scales with time-to-sunrise (longer through the
  // night, floored at 90s near dawn/dusk).
  unsigned long calculateSleepSeconds(RTCInfoRecord& currentTimerRecord, HourlySolarPowerData& hourlySolarPowerData);

private:
  BatteryChemistry _chemistry;
  float _capacityMah;
  SolarInfo& _solarInfo;
};
#endif
