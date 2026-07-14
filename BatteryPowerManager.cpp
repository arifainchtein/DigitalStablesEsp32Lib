#include <BatteryPowerManager.h>
#include <GeneralFunctions.h>

BatteryPowerManager::BatteryPowerManager(BatteryChemistry chemistry, float capacityMah, SolarInfo& solarInfo)
  : _chemistry(chemistry), _capacityMah(capacityMah), _solarInfo(solarInfo) {}

float BatteryPowerManager::sleepingVoltage() const {
  if (_chemistry == CHEM_LIFEPO4) return 12.0;  // ~3.0V/cell average
  return 12.0;  // generic lead-acid (flooded/AGM/gel) - see BatteryChemistry comment
}

float BatteryPowerManager::commaVoltage() const {
  if (_chemistry == CHEM_LIFEPO4) return 11.6;  // ~2.9V/cell average, margin above BMS undervoltage cutoff
  return 11.6;  // generic lead-acid (flooded/AGM/gel)
}

uint8_t BatteryPowerManager::stateOfCharge(float busVoltage) const {
  if (_chemistry == CHEM_LIFEPO4) {
    return GeneralFunctions::getBatteryStateOfCharge(busVoltage / 4.0);  // 4S pack, per-cell curve
  }
  return GeneralFunctions::getLeadAcidStateOfCharge(busVoltage);
}

float BatteryPowerManager::estimatedRuntimeHours(float busVoltage, float dischargeMa) const {
  if (dischargeMa <= 0) return 0.0;
  uint8_t soc = stateOfCharge(busVoltage);
  float remainingMah = (soc / 100.0) * _capacityMah;
  return remainingMah / dischargeMa;
}

unsigned long BatteryPowerManager::calculateSleepSeconds(RTCInfoRecord& currentTimerRecord, HourlySolarPowerData& hourlySolarPowerData) {
  // Daytime (even if cloudy): short naps so pulse sampling stays close to real-time.
  if (hourlySolarPowerData.efficiency > 0.3) {
    return 60UL;
  }

  DailySolarData dailySolarData = _solarInfo.getDailySolarData(currentTimerRecord);

  int currentMinutes = currentTimerRecord.hour * 60 + currentTimerRecord.minute;
  int minutesToSunrise = (int)dailySolarData.sunrise - currentMinutes;
  if (minutesToSunrise < 0) minutesToSunrise += 24 * 60;
  if (minutesToSunrise < 1) minutesToSunrise = 1;

  // Pre-dawn window: wake every 90s to catch the charging ramp without missing it.
  if (minutesToSunrise <= 90) return 90UL;

  int dayLength = max((int)dailySolarData.sunset - (int)dailySolarData.sunrise, 1);
  int totalNightMinutes = max(24 * 60 - dayLength, 1);

  unsigned long sleepSec = (unsigned long)(450.0 * totalNightMinutes / minutesToSunrise);
  if (sleepSec < 90UL) sleepSec = 90UL;

  return sleepSec;
}
