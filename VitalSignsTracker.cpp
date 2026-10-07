#include <VitalSignsTracker.h>
#include <Preferences.h>
#include <esp_system.h>
#include <esp_sleep.h>
#include <esp_attr.h>

// Running totals - survive deep sleep, re-initialised by the bootloader on every real reset
// (the same moment resetCount goes up, so the Teleonome knows the totals restarted).
RTC_DATA_ATTR static uint16_t rtc_vs_resetCount = 0;      // cached copy of the NVS values
RTC_DATA_ATTR static uint8_t  rtc_vs_lastResetReason = 0;
RTC_DATA_ATTR static uint32_t rtc_vs_lastResetTime = 0;
RTC_DATA_ATTR static bool     rtc_vs_resetReportPending = false;
RTC_DATA_ATTR static uint32_t rtc_vs_wakeCount = 0;
RTC_DATA_ATTR static uint16_t rtc_vs_earlyWakeCount = 0;
RTC_DATA_ATTR static uint16_t rtc_vs_commaWakeCount = 0;
RTC_DATA_ATTR static uint64_t rtc_vs_awakeMsTotal = 0;
RTC_DATA_ATTR static uint32_t rtc_vs_sleptSecondsTotal = 0;
RTC_DATA_ATTR static uint32_t rtc_vs_intendedWakeEpoch = 0;  // 0 = unknown
RTC_DATA_ATTR static int16_t  rtc_vs_lastWakeDriftSec = 0;
RTC_DATA_ATTR static uint16_t rtc_vs_lastAwakeSec = 0;
RTC_DATA_ATTR static uint16_t rtc_vs_wakeVoltage_mV = 0;
RTC_DATA_ATTR static uint16_t rtc_vs_minVoltage_mV = 0;      // 0 = nothing seen since last record
RTC_DATA_ATTR static uint16_t rtc_vs_txDurationMs = 0;
RTC_DATA_ATTR static int16_t  rtc_vs_txPre_mA = 0;
RTC_DATA_ATTR static int16_t  rtc_vs_txPeak_mA = 0;
RTC_DATA_ATTR static int16_t  rtc_vs_txPost_mA = 0;
RTC_DATA_ATTR static int16_t  rtc_vs_txPanel_mA = -1;
RTC_DATA_ATTR static uint16_t rtc_vs_txV50i_mV = 0;
RTC_DATA_ATTR static uint16_t rtc_vs_txMin_mV = 0;
RTC_DATA_ATTR static uint16_t rtc_vs_txFailCount = 0;
RTC_DATA_ATTR static uint16_t rtc_vs_seq = 0;

static uint16_t toMilliVolts(float volts) {
  if (volts <= 0) return 0;
  float mv = volts * 1000.0f;
  return mv > 65535.0f ? 65535 : (uint16_t)mv;
}

uint32_t VitalSignsTracker::buildStamp(const char *date, const char *time) {
  static const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  uint32_t month = 0;
  for (uint32_t i = 0; i < 12; i++) {
    if (strncmp(date, months + i * 3, 3) == 0) { month = i + 1; break; }
  }
  uint32_t day = (uint32_t)atoi(date + 4);
  uint32_t year = (uint32_t)atoi(date + 7) % 100;
  uint32_t hour = (uint32_t)atoi(time);
  return ((year * 100 + month) * 100 + day) * 100 + hour;
}

void VitalSignsTracker::captureBoot() {
  esp_reset_reason_t reason = esp_reset_reason();
  _resetReason = (uint8_t)reason;
  _wakeCause = (uint8_t)esp_sleep_get_wakeup_cause();
  _realReset = (reason != ESP_RST_DEEPSLEEP);
}

void VitalSignsTracker::recordBoot(uint32_t nowEpoch) {
  if (_bootRecorded) return;
  _bootRecorded = true;
  if (_realReset) {
    Preferences prefs;
    prefs.begin("vitals", false);
    uint16_t count = prefs.getUShort("resetCount", 0) + 1;
    prefs.putUShort("resetCount", count);
    prefs.putUChar("lastReason", _resetReason);
    prefs.putULong("lastTime", nowEpoch);
    prefs.end();

    rtc_vs_resetCount = count;
    rtc_vs_lastResetReason = _resetReason;
    rtc_vs_lastResetTime = nowEpoch;
    rtc_vs_resetReportPending = true;
    rtc_vs_wakeCount = 0;
    rtc_vs_earlyWakeCount = 0;
    rtc_vs_commaWakeCount = 0;
    rtc_vs_awakeMsTotal = 0;
    rtc_vs_sleptSecondsTotal = 0;
    rtc_vs_intendedWakeEpoch = 0;
    rtc_vs_lastWakeDriftSec = 0;
    rtc_vs_lastAwakeSec = 0;
    rtc_vs_minVoltage_mV = 0;
    rtc_vs_txFailCount = 0;
    rtc_vs_seq = 0;
  } else {
    rtc_vs_wakeCount++;
    if (nowEpoch > 0 && rtc_vs_intendedWakeEpoch > 0) {
      int32_t drift = (int32_t)nowEpoch - (int32_t)rtc_vs_intendedWakeEpoch;
      if (drift > 32767) drift = 32767;
      if (drift < -32768) drift = -32768;
      rtc_vs_lastWakeDriftSec = (int16_t)drift;
    }
  }
}

void VitalSignsTracker::recordEarlyWake() {
  rtc_vs_earlyWakeCount++;
}

void VitalSignsTracker::recordCommaWake(float batteryVoltage, uint32_t nowEpoch, uint32_t sleepSeconds) {
  rtc_vs_commaWakeCount++;
  noteBatteryVoltage(batteryVoltage);
  rtc_vs_intendedWakeEpoch = nowEpoch > 0 ? nowEpoch + sleepSeconds : 0;
}

void VitalSignsTracker::recordWakeVoltage(float batteryVoltage) {
  rtc_vs_wakeVoltage_mV = toMilliVolts(batteryVoltage);
  noteBatteryVoltage(batteryVoltage);
}

void VitalSignsTracker::noteBatteryVoltage(float batteryVoltage) {
  uint16_t mv = toMilliVolts(batteryVoltage);
  if (mv == 0) return;
  if (rtc_vs_minVoltage_mV == 0 || mv < rtc_vs_minVoltage_mV) rtc_vs_minVoltage_mV = mv;
}

void VitalSignsTracker::txSamplerTask(void *arg) {
  VitalSignsTracker *t = (VitalSignsTracker *)arg;
  while (t->_samplerRun) {
    int16_t mA;
    uint16_t mV;
    if (t->_sampleFn(mA, mV)) {
      if (mA > t->_peakMa) t->_peakMa = mA;
      if (mV > 0 && (t->_minMv == 0 || mV < t->_minMv)) t->_minMv = mV;
    }
    vTaskDelay(1);
  }
  t->_samplerDone = true;
  vTaskDelete(NULL);
}

void VitalSignsTracker::beginTx(VitalSignsSampleFn sampleFn, int16_t panel_mA, uint16_t v50i_mV) {
  _sampleFn = sampleFn;
  rtc_vs_txPanel_mA = panel_mA;
  rtc_vs_txV50i_mV = v50i_mV;
  _peakMa = 0;
  _minMv = 0;
  rtc_vs_txPre_mA = 0;
  if (_sampleFn) {
    int16_t mA;
    uint16_t mV;
    if (_sampleFn(mA, mV)) {
      rtc_vs_txPre_mA = mA;
      _peakMa = mA;
      _minMv = mV;
    }
    _samplerRun = true;
    _samplerDone = false;
    if (xTaskCreatePinnedToCore(txSamplerTask, "vsTxSampler", 3072, this, 1, NULL, 0) != pdPASS) {
      _samplerRun = false;
      _samplerDone = true;
    }
  }
  _txStartMs = millis();
}

void VitalSignsTracker::endTx(bool ok) {
  unsigned long duration = millis() - _txStartMs;
  rtc_vs_txDurationMs = duration > 65535 ? 65535 : (uint16_t)duration;
  if (_sampleFn) {
    _samplerRun = false;
    unsigned long waitStart = millis();
    while (!_samplerDone && millis() - waitStart < 100) delay(1);
    int16_t mA;
    uint16_t mV;
    rtc_vs_txPost_mA = _sampleFn(mA, mV) ? mA : 0;
    rtc_vs_txPeak_mA = _peakMa;
    rtc_vs_txMin_mV = _minMv;
    if (_minMv > 0 && (rtc_vs_minVoltage_mV == 0 || _minMv < rtc_vs_minVoltage_mV)) rtc_vs_minVoltage_mV = _minMv;
  }
  if (!ok) rtc_vs_txFailCount++;
}

void VitalSignsTracker::recordSleep(uint32_t nowEpoch, uint32_t sleepSeconds) {
  unsigned long awake = millis();
  unsigned long awakeSec = (awake + 500) / 1000;
  rtc_vs_lastAwakeSec = awakeSec > 65535 ? 65535 : (uint16_t)awakeSec;
  rtc_vs_awakeMsTotal += awake;
  rtc_vs_sleptSecondsTotal += sleepSeconds;
  rtc_vs_intendedWakeEpoch = nowEpoch > 0 ? nowEpoch + sleepSeconds : 0;
  _sleepRecorded = true;
}

bool VitalSignsTracker::resetReportPending() const {
  return rtc_vs_resetReportPending;
}

VitalSignsRecord VitalSignsTracker::buildRecord(const uint8_t serialnumber[8], uint32_t firmwareBuild, uint8_t i2cDeviceMask) {
  VitalSignsRecord r;
  memcpy(r.serialnumberarray, serialnumber, sizeof(r.serialnumberarray));
  r.version = VITAL_SIGNS_VERSION;
  r.firmwareBuild = firmwareBuild;
  r.resetCount = rtc_vs_resetCount;
  r.lastResetReason = rtc_vs_lastResetReason;
  r.lastResetTime = rtc_vs_lastResetTime;
  r.wakeCount = rtc_vs_wakeCount;
  r.earlyWakeCount = rtc_vs_earlyWakeCount;
  r.commaWakeCount = rtc_vs_commaWakeCount;
  // Before recordSleep() the current wake is still running - count it so a device that never
  // sleeps reports its uptime here.
  r.awakeSecondsTotal = (uint32_t)((rtc_vs_awakeMsTotal + (_sleepRecorded ? 0 : millis())) / 1000);
  r.sleptSecondsTotal = rtc_vs_sleptSecondsTotal;
  r.lastWakeCause = _wakeCause;
  r.lastWakeDriftSec = rtc_vs_lastWakeDriftSec;
  r.lastAwakeSec = rtc_vs_lastAwakeSec;
  r.wakeVoltage_mV = rtc_vs_wakeVoltage_mV;
  r.minVoltageSinceReport_mV = rtc_vs_minVoltage_mV;
  r.txDurationMs = rtc_vs_txDurationMs;
  r.txBatteryPre_mA = rtc_vs_txPre_mA;
  r.txBatteryPeak_mA = rtc_vs_txPeak_mA;
  r.txBatteryPost_mA = rtc_vs_txPost_mA;
  r.txPanel_mA = rtc_vs_txPanel_mA;
  r.txV50i_mV = rtc_vs_txV50i_mV;
  r.txMinVoltage_mV = rtc_vs_txMin_mV;
  r.loraTxFailCount = rtc_vs_txFailCount;
  r.seq = ++rtc_vs_seq;
  r.i2cDeviceMask = i2cDeviceMask;
  rtc_vs_minVoltage_mV = 0;
  return r;
}

void VitalSignsTracker::markSent() {
  rtc_vs_resetReportPending = false;
}
