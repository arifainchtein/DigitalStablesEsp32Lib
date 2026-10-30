#include <VitalSignsTracker.h>
#include <Preferences.h>
#include <esp_system.h>
#include <esp_sleep.h>
#include <esp_attr.h>
#include <esp_timer.h>

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
// Battery ledger - full-precision running totals, converted to record units only in buildRecord()
// so no rounding is lost between records. Units: uA x ms (0.01 mAh = 36,000,000 uA.ms) and, for
// panel energy, uW x ms = nJ (0.1 mWh = 360,000,000 nJ).
RTC_DATA_ATTR static uint64_t rtc_vs_chargeIn_uAms = 0;
RTC_DATA_ATTR static uint64_t rtc_vs_dischargeOut_uAms = 0;
RTC_DATA_ATTR static uint64_t rtc_vs_panelIn_nJ = 0;
RTC_DATA_ATTR static uint64_t rtc_vs_ledgerMeasuredMs = 0;
RTC_DATA_ATTR static uint16_t rtc_vs_ledgerMaxGapMs = 0;   // since the last record
RTC_DATA_ATTR static int16_t  rtc_vs_batteryTemp_dC = INT16_MIN;
// tagged sub-ledger (setLedgerTag) - same units as the main totals
RTC_DATA_ATTR static uint64_t rtc_vs_tagMs = 0;
RTC_DATA_ATTR static uint64_t rtc_vs_tagDischargeOut_uAms = 0;
RTC_DATA_ATTR static uint64_t rtc_vs_tagChargeIn_uAms = 0;

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
    rtc_vs_chargeIn_uAms = 0;
    rtc_vs_dischargeOut_uAms = 0;
    rtc_vs_panelIn_nJ = 0;
    rtc_vs_ledgerMeasuredMs = 0;
    rtc_vs_ledgerMaxGapMs = 0;
    rtc_vs_batteryTemp_dC = INT16_MIN;
    rtc_vs_tagMs = 0;
    rtc_vs_tagDischargeOut_uAms = 0;
    rtc_vs_tagChargeIn_uAms = 0;
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
  if (_ledgerRun) {
    // The ledger already samples on core 0 - switch it to fast mode for a true TX peak instead of
    // starting a second task on the same sensors.
    rtc_vs_txPanel_mA = panel_mA;
    rtc_vs_txV50i_mV = v50i_mV;
    LedgerSample s;
    rtc_vs_txPre_mA = latestSample(s) ? (int16_t)constrain(s.battery_uA / 1000, -32768, 32767) : 0;
    portENTER_CRITICAL(&_ledgerMux);
    _peakMa = rtc_vs_txPre_mA;
    _minMv = 0;
    portEXIT_CRITICAL(&_ledgerMux);
    if (_ledgerModeFn) _ledgerModeFn(true);
    _txActive = true;
    _txStartMs = millis();
    return;
  }
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
  if (_txActive) {
    _txActive = false;
    if (_ledgerModeFn) _ledgerModeFn(false);
    LedgerSample s;
    rtc_vs_txPost_mA = latestSample(s) ? (int16_t)constrain(s.battery_uA / 1000, -32768, 32767) : 0;
    portENTER_CRITICAL(&_ledgerMux);
    rtc_vs_txPeak_mA = _peakMa;
    rtc_vs_txMin_mV = _minMv;
    portEXIT_CRITICAL(&_ledgerMux);
    if (rtc_vs_txMin_mV > 0 && (rtc_vs_minVoltage_mV == 0 || rtc_vs_txMin_mV < rtc_vs_minVoltage_mV)) rtc_vs_minVoltage_mV = rtc_vs_txMin_mV;
  } else if (_sampleFn) {
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
  stopLedger();   // no sampling across deep sleep - the Cerebellum covers it with sleepCurrent_uA
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
  portENTER_CRITICAL(&_ledgerMux);
  r.chargeIn_cmAh = (uint32_t)(rtc_vs_chargeIn_uAms / 36000000ULL);
  r.dischargeOut_cmAh = (uint32_t)(rtc_vs_dischargeOut_uAms / 36000000ULL);
  r.panelIn_dmWh = (uint32_t)(rtc_vs_panelIn_nJ / 360000000ULL);
  r.wifiOnSec = (uint32_t)(rtc_vs_tagMs / 1000ULL);                        // v4: the tagged sub-ledger
  r.wifiOut_cmAh = (uint32_t)(rtc_vs_tagDischargeOut_uAms / 36000000ULL);
  r.ledgerMeasuredSec = (uint32_t)(rtc_vs_ledgerMeasuredMs / 1000);
  r.ledgerMaxGapMs = rtc_vs_ledgerMaxGapMs;
  rtc_vs_ledgerMaxGapMs = 0;
  portEXIT_CRITICAL(&_ledgerMux);
  r.batteryTemp_dC = rtc_vs_batteryTemp_dC;
  rtc_vs_batteryTemp_dC = INT16_MIN;   // "no reading" unless the sketch notes one again next cycle
  rtc_vs_minVoltage_mV = 0;
  return r;
}

void VitalSignsTracker::noteTxFailed() {
  rtc_vs_txFailCount++;
}

void VitalSignsTracker::markSent() {
  rtc_vs_resetReportPending = false;
}

// --- Battery ledger -------------------------------------------------------------------------

void VitalSignsTracker::startLedger(LedgerSampleFn sampleFn, LedgerModeFn modeFn, uint16_t periodMs) {
  if (_ledgerRun || !sampleFn) return;
  _ledgerFn = sampleFn;
  _ledgerModeFn = modeFn;
  _ledgerPeriodMs = periodMs;
  _haveSample = false;
  _ledgerRun = true;
  _ledgerDone = false;
  if (xTaskCreatePinnedToCore(ledgerTask, "vsLedger", 3072, this, 1, NULL, 0) != pdPASS) {
    _ledgerRun = false;
    _ledgerDone = true;
  }
}

void VitalSignsTracker::stopLedger() {
  if (!_ledgerRun) return;
  _ledgerRun = false;
  unsigned long waitStart = millis();
  while (!_ledgerDone && millis() - waitStart < 300) delay(1);
}

void VitalSignsTracker::ledgerTask(void *arg) {
  VitalSignsTracker *t = (VitalSignsTracker *)arg;
  while (t->_ledgerRun) {
    t->ledgerStep();
    // During TX the battery sensor is in single-sample mode (modeFn) - sample every tick for the peak.
    vTaskDelay(t->_txActive ? 1 : pdMS_TO_TICKS(t->_ledgerPeriodMs));
  }
  t->_ledgerDone = true;
  vTaskDelete(NULL);
}

// Trapezoid integration between consecutive samples on the real elapsed time (esp_timer, us), so
// the totals don't depend on the sampling rate or on how often records are sent. The sensor read
// (I2C) happens in the callback before the lock - only arithmetic runs inside it.
void VitalSignsTracker::ledgerStep() {
  LedgerSample s;
  if (!_ledgerFn(s)) return;
  int64_t nowUs = esp_timer_get_time();
  int64_t panel_uW = s.panel_mV > 0 ? (int64_t)s.panel_uA * s.panel_mV / 1000 : 0;
  portENTER_CRITICAL(&_ledgerMux);
  if (_haveSample) {
    int64_t dtMs = (nowUs - _lastSampleUs) / 1000;
    int64_t avg_uA = ((int64_t)_lastBattery_uA + s.battery_uA) / 2;
    if (avg_uA > 0) rtc_vs_dischargeOut_uAms += (uint64_t)(avg_uA * dtMs);
    else            rtc_vs_chargeIn_uAms     += (uint64_t)(-avg_uA * dtMs);
    if (_tag) {
      rtc_vs_tagMs += (uint64_t)dtMs;
      if (avg_uA > 0) rtc_vs_tagDischargeOut_uAms += (uint64_t)(avg_uA * dtMs);
      else            rtc_vs_tagChargeIn_uAms     += (uint64_t)(-avg_uA * dtMs);
    }
    int64_t avg_uW = (_lastPanel_uW + panel_uW) / 2;
    if (avg_uW > 0) rtc_vs_panelIn_nJ += (uint64_t)(avg_uW * dtMs);
    rtc_vs_ledgerMeasuredMs += (uint64_t)dtMs;
    uint16_t gap = dtMs > 65535 ? 65535 : (uint16_t)dtMs;
    if (gap > rtc_vs_ledgerMaxGapMs) rtc_vs_ledgerMaxGapMs = gap;
  }
  _lastSampleUs = nowUs;
  _lastBattery_uA = s.battery_uA;
  _lastPanel_uW = panel_uW;
  _latest = s;
  _haveSample = true;
  if (_txActive) {
    int16_t mA = (int16_t)constrain(s.battery_uA / 1000, -32768, 32767);
    if (mA > _peakMa) _peakMa = mA;
    if (s.battery_mV > 0 && (_minMv == 0 || s.battery_mV < _minMv)) _minMv = s.battery_mV;
  }
  portEXIT_CRITICAL(&_ledgerMux);
}

void VitalSignsTracker::taggedTotals(uint32_t &taggedMs, float &dischargeOut_mAh, float &chargeIn_mAh) {
  portENTER_CRITICAL(&_ledgerMux);
  uint64_t ms = rtc_vs_tagMs, out = rtc_vs_tagDischargeOut_uAms, in = rtc_vs_tagChargeIn_uAms;
  portEXIT_CRITICAL(&_ledgerMux);
  taggedMs = (uint32_t)ms;
  dischargeOut_mAh = out / 3.6e9f;  // uA*ms -> mAh
  chargeIn_mAh = in / 3.6e9f;
}

bool VitalSignsTracker::latestSample(LedgerSample &s) {
  portENTER_CRITICAL(&_ledgerMux);
  bool ok = _haveSample;
  if (ok) s = _latest;
  portEXIT_CRITICAL(&_ledgerMux);
  return ok;
}

void VitalSignsTracker::noteBatteryTemperature(float celsius) {
  float d = celsius * 10.0f;
  rtc_vs_batteryTemp_dC = (int16_t)constrain(d, -32767.0f, 32767.0f);
}
