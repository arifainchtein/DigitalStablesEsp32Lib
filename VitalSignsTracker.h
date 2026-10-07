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
  void beginTx(VitalSignsSampleFn sampleFn, int16_t panel_mA = -1, uint16_t v50i_mV = 0);
  void endTx(bool ok);

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
};

#endif
