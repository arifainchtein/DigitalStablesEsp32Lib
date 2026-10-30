#include <VitalSignsSerializer.h>
#include "DeviceSerial.h"

void VitalSignsSerializer::pushToSerial(HardwareSerial& serial, const VitalSignsRecord& r, float rssi, float snr, uint32_t receivedAgeSeconds) {
    serial.print("VitalSignsDeserializer");
    serial.print(F("#"));
    serial.print(DeviceSerial::toString(r.serialnumberarray));  // the efuse MAC, as the factory records it
    serial.print(F("#")); serial.print(r.version);
    serial.print(F("#")); serial.print(r.firmwareBuild);
    serial.print(F("#")); serial.print(r.resetCount);
    serial.print(F("#")); serial.print(r.lastResetReason);
    serial.print(F("#")); serial.print(r.lastResetTime);
    serial.print(F("#")); serial.print(r.wakeCount);
    serial.print(F("#")); serial.print(r.earlyWakeCount);
    serial.print(F("#")); serial.print(r.commaWakeCount);
    serial.print(F("#")); serial.print(r.awakeSecondsTotal);
    serial.print(F("#")); serial.print(r.sleptSecondsTotal);
    serial.print(F("#")); serial.print(r.lastWakeCause);
    serial.print(F("#")); serial.print(r.lastWakeDriftSec);
    serial.print(F("#")); serial.print(r.lastAwakeSec);
    serial.print(F("#")); serial.print(r.wakeVoltage_mV);
    serial.print(F("#")); serial.print(r.minVoltageSinceReport_mV);
    serial.print(F("#")); serial.print(r.txDurationMs);
    serial.print(F("#")); serial.print(r.txBatteryPre_mA);
    serial.print(F("#")); serial.print(r.txBatteryPeak_mA);
    serial.print(F("#")); serial.print(r.txBatteryPost_mA);
    serial.print(F("#")); serial.print(r.txPanel_mA);
    serial.print(F("#")); serial.print(r.txV50i_mV);
    serial.print(F("#")); serial.print(r.txMinVoltage_mV);
    serial.print(F("#")); serial.print(r.loraTxFailCount);
    serial.print(F("#")); serial.print(r.seq);
    serial.print(F("#")); serial.print(r.i2cDeviceMask);
    serial.print(F("#")); serial.print(rssi, 1);
    serial.print(F("#")); serial.print(snr, 1);
    serial.print(F("#")); serial.print(receivedAgeSeconds);
    // v3 battery ledger, appended AFTER receivedAgeSeconds so a framework that only knows the
    // first 30 tokens keeps working. Always sent (zeros / -32768 for a version-2 record).
    serial.print(F("#")); serial.print(r.chargeIn_cmAh);
    serial.print(F("#")); serial.print(r.dischargeOut_cmAh);
    serial.print(F("#")); serial.print(r.panelIn_dmWh);
    serial.print(F("#")); serial.print(r.ledgerMeasuredSec);
    serial.print(F("#")); serial.print(r.ledgerMaxGapMs);
    serial.print(F("#")); serial.print(r.batteryTemp_dC);
    // v4 WiFi totals (tokens 36-37), appended the same way. Zeros for a version 2/3 record.
    serial.print(F("#")); serial.print(r.wifiOnSec);
    serial.print(F("#")); serial.println(r.wifiOut_cmAh);
}

VitalSignsSerializer::~VitalSignsSerializer() {}
