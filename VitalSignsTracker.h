#ifndef VITALSIGNSTRACKER_H
#define VITALSIGNSTRACKER_H

#include "Arduino.h"
#include <VitalSignsRecord.h>

// Device-side bookkeeping for VitalSignsRecord, shared by Daffodil, Langley and Chinampa.
//
// Persistence:
//   - reset counter / reason / time : NVS (Preferences namespace "vitals") - survive power loss;
//                                     written once per REAL reset, never per deep-sleep wake.
//   - sleep / TX running totals     : RTC_DATA_ATTR (in the .cpp) - survive deep sleep; the
//                                     bootloader re-initialises them on every real reset, which
//                                     is exactly when resetCount goes up.
//
// Typical sleeping device (Daffodil / Langley):
//   setup():  vitalSigns.captureBoot();                      // very first line, no I/O
//             ... RTC up ...
//             vitalSigns.recordBoot(nowEpoch);               // BEFORE the early-exit checks
//             early-wake path:  vitalSigns.recordEarlyWake();  then sleep
//             COMMA path:       vitalSigns.recordCommaWake(v); then sleep
//             first voltage of a full boot: vitalSigns.recordWakeVoltage(v);
//   sendMessage(): beginTx() / endTx() around the blocking LoRa.endPacket(false)
//   goToSleep():   vitalSigns.recordSleep(nowEpoch, seconds); then send the record
//
// A device that never sleeps (Chinampa) only calls captureBoot(), recordBoot(), the TX hooks and
// buildRecord().

// Reads one sample for the TX summary. Return false if no reading is available.
// batteryDraw_mA: + = drawn from the battery. battery_mV: battery bus voltage (0 if unknown).
typedef bool (*VitalSignsSampleFn)(int16_t &batteryDraw_mA, uint16_t &battery_mV);

// One battery-ledger reading, filled by the product sketch's ledger callback. While the ledger
// runs, the callback is called from core 0 every ~70 ms; anything on core 1 that reads the same
// current sensors must share a lock with it (an INA219 read is a register-pointer write followed
// by a read, so two cores reading the same chip can get each other's register).
struct LedgerSample {
  int32_t  battery_uA = 0;     // + = drawn from the battery, - = charging (project convention)
  uint16_t battery_mV = 0;     // 0 = not read
  int32_t  panel_uA = 0;       // into the board from the panel/USB input
  uint16_t panel_mV = 0;       // 0 = no panel sensor (panel energy then not integrated)
};
typedef bool (*LedgerSampleFn)(LedgerSample &s);
// fastMode: true while a LoRa TX is in progress - the sketch switches the battery sensor to
// single-sample conversions so the TX peak is a real peak, and back to averaging after.
typedef void (*LedgerModeFn)(bool fastMode);

class VitalSignsTracker {
public:
  // YYMMDDhh from the sketch's __DATE__ ("Oct  5 2026") and __TIME__ ("14:03:07").
  static uint32_t buildStamp(const char *date, const char *time);

  // Very first line of setup(): reads esp_reset_reason() and esp_sleep_get_wakeup_cause().
  void captureBoot();
  // Once the RTC is readable, before any return-to-sleep path. On a real reset: NVS resetCount++,
  // reason and time saved, RTC totals zeroed. On a deep-sleep wake: wakeCount++ and wake drift
  // (skipped when nowEpoch is 0, i.e. the RTC hasn't been read yet). Only the first call per
  // boot counts.
  void recordBoot(uint32_t nowEpoch);
  void recordEarlyWake();
  // COMMA recheck still low, about to sleep sleepSeconds. nowEpoch 0 = RTC not read yet.
  void recordCommaWake(float batteryVoltage, uint32_t nowEpoch, uint32_t sleepSeconds);
  void recordWakeVoltage(float batteryVoltage);
  void noteBatteryVoltage(float batteryVoltage);  // folds into minVoltageSinceReport

  // TX summary. sampleFn may be nullptr (device has no current sensor) - then only duration and
  // the fail count are recorded. With a sampleFn, a short task on core 0 samples it while the
  // caller blocks in LoRa.endPacket(false) (polling LoRa.isTransmitting() never sees the radio
  // busy on this hardware, so the samples can't be taken from the sending loop itself).
  // While the ledger runs, beginTx()/endTx() take the TX summary from it instead (sampleFn unused).
  void beginTx(VitalSignsSampleFn sampleFn, int16_t panel_mA = -1, uint16_t v50i_mV = 0);
  void endTx(bool ok);
  // A send that never went out, or failed outside beginTx()/endTx() - the channel stayed busy
  // through every CAD back-off, or a packet type the TX summary doesn't follow (VitalSigns) failed
  // to key up. Counted in loraTxFailCount with endTx() failures (2026-10-15: TopTank's seq gaps
  // could not tell a packet Annabelle missed from one that was never sent).
  void noteTxFailed();

  // Battery ledger (VitalSignsRecord v3). Start once the current sensors are configured; runs on
  // core 0 until recordSleep() (sleeping products) or forever (Chinampa). periodMs >= the sensor's
  // conversion time (68.1 ms at 128-sample averaging) so every read is a fresh average.
  // modeFn may be nullptr.
  void startLedger(LedgerSampleFn sampleFn, LedgerModeFn modeFn = nullptr, uint16_t periodMs = 70);
  void stopLedger();
  bool ledgerRunning() const { return _ledgerRun; }
  bool latestSample(LedgerSample &s);     // false until the first sample after startLedger()
  // Tagged sub-ledger (2026-10-14): while the tag is on, each step is ALSO added to separate
  // totals - the sketch tags the time its WiFi is on, to measure what WiFi costs. Sent as
  // VitalSignsRecord.wifiOnSec / wifiOut_cmAh (v4). RTC memory, since the last real reset.
  void setLedgerTag(bool on) { _tag = on; }
  void taggedTotals(uint32_t &taggedMs, float &dischargeOut_mAh, float &chargeIn_mAh);
  // Battery temperature for VitalSignsRecord.batteryTemp_dC (PowerProfileRecord.batteryTempSource
  // says where it comes from). Call each cycle the sensor reads OK; not calling it = no reading.
  void noteBatteryTemperature(float celsius);

  // goToSleep(): closes this awake cycle and stores the intended wake time for the drift check.
  void recordSleep(uint32_t nowEpoch, uint32_t sleepSeconds);

  // True until the first record after a real reset has been sent.
  bool resetReportPending() const;
  bool wasRealReset() const { return _realReset; }
  uint8_t resetReason() const { return _resetReason; }

  // Fills everything except totpcode and checksum (the sketch's sendMessage() stamps those).
  // Increments seq and clears the "since last report" minimum.
  VitalSignsRecord buildRecord(const uint8_t serialnumber[8], uint32_t firmwareBuild, uint8_t i2cDeviceMask);
  void markSent();

private:
  static void txSamplerTask(void *arg);
  static void ledgerTask(void *arg);
  void ledgerStep();

  volatile bool _tag = false;
  uint8_t _resetReason = 0;
  uint8_t _wakeCause = 0;
  bool _realReset = false;
  bool _bootRecorded = false;
  bool _sleepRecorded = false;

  VitalSignsSampleFn _sampleFn = nullptr;
  volatile bool _samplerRun = false;
  volatile bool _samplerDone = true;
  volatile int16_t _peakMa = 0;
  volatile uint16_t _minMv = 0;
  unsigned long _txStartMs = 0;

  LedgerSampleFn _ledgerFn = nullptr;
  LedgerModeFn _ledgerModeFn = nullptr;
  uint16_t _ledgerPeriodMs = 70;
  volatile bool _ledgerRun = false;
  volatile bool _ledgerDone = true;
  volatile bool _txActive = false;
  bool _haveSample = false;
  LedgerSample _latest;
  int64_t _lastSampleUs = 0;
  int32_t _lastBattery_uA = 0;
  int64_t _lastPanel_uW = 0;
  portMUX_TYPE _ledgerMux = portMUX_INITIALIZER_UNLOCKED;  // 64-bit totals aren't atomic on ESP32
};

#endif
