#include <LangleyDataSerializer.h>
#include "SentBy.h"
#include "DeviceSerial.h"

void LangleyDataSerializer::pushToSerial(HardwareSerial& serial, LangleyData data) {
    serial.print("LangleyDataDeserializer");
    serial.print(F("#"));
    for (int i = 0; i < (int)sizeof(data.devicename); i++) {
        if (data.devicename[i] != '\0' && data.devicename[i] != '\n' && data.devicename[i] != '\r')
            serial.print(data.devicename[i]);
    }
    serial.print(F("#"));
    for (int i = 0; i < (int)sizeof(data.deviceshortname); i++) {
        if (data.deviceshortname[i] != '\0' && data.deviceshortname[i] != '\n' && data.deviceshortname[i] != '\r')
            serial.print(data.deviceshortname[i]);
    }
    serial.print(F("#"));
    serial.print(DeviceSerial::toString(data.serialnumberarray));  // the efuse MAC, as the factory records it
    serial.print(F("#"));
    serial.print(data.secondsTime);
    serial.print(F("#"));
    serial.print(data.externalBatteryVoltage, 2);
    serial.print(F("#"));
    serial.print(data.energizerBatteryVoltage, 2);
    serial.print(F("#"));
    serial.print(data.energizerBatteryCurrent, 2);
    serial.print(F("#"));
    serial.print(data.temperature, 2);
    serial.print(F("#"));
    serial.print(data.fenceVoltage, 2);
    serial.print(F("#"));
    serial.print(data.fenceVoltageMin, 2);
    serial.print(F("#"));
    serial.print(data.fenceVoltageMax, 2);
    serial.print(F("#"));
    serial.print(data.fenceVoltageAvg, 2);
    serial.print(F("#"));
    serial.print(data.pulseCount);
    serial.print(F("#"));
    serial.print(data.rssi);
    serial.print(F("#"));
    serial.print(data.snr);
    serial.print(F("#"));
    serial.print(data.operatingStatus);
    serial.print(F("#"));
    serial.print(data.sleepTime);
    serial.print(F("#"));
    serial.print(data.wifiStatus);
    serial.print(F("#"));
    serial.print(data.opMode);
    serial.print(F("#"));
    serial.print(data.secondsSinceLastPulse);
    serial.print(F("#"));
    serial.print(data.solarVoltage, 2);
    serial.print(F("#"));
    serial.print(data.solarCurrentMa, 2);
    serial.print(F("#"));
    serial.print(data.batteryVoltage, 2);
    serial.print(F("#"));
    serial.print(data.batteryCurrentMa, 2);
    serial.print(F("#"));
    serial.print(data.estimatedRuntime, 2);
    serial.print(F("#"));
    serial.print(data.batteryChemistry);
    serial.print(F("#"));
    serial.print(data.rtcBatVolt, 2);
    serial.print(F("#"));
    serial.print(data.overnightMah, 4);
    serial.print(F("#"));
    for (int i = 0; i < (int)sizeof(data.parentShortname); i++) {
        if (data.parentShortname[i] != '\0' && data.parentShortname[i] != '\n' && data.parentShortname[i] != '\r')
            serial.print(data.parentShortname[i]);
    }
    serial.print(F("#"));
    for (int i = 0; i < (int)sizeof(data.branchLabel); i++) {
        if (data.branchLabel[i] != '\0' && data.branchLabel[i] != '\n' && data.branchLabel[i] != '\r')
            serial.print(data.branchLabel[i]);
    }
    serial.print(F("#"));
    serial.print(data.latitude, 8);
    serial.print(F("#"));
    serial.print(data.longitude, 8);
    serial.print(F("#"));
    serial.print(data.altitude, 2);
    serial.print(F("#"));
    serial.print(data.checksum);
    serial.print(F("#"));
    // keyed, so a framework that reads the fixed tokens 0-34 ignores it (2026-11-12); empty: no thermistor
    serial.print(F("battemp:"));
    if (data.batteryTemperature > -100.0f) serial.print(data.batteryTemperature, 1);
    serial.print(F("#"));
    printSentByToken(serial, data.sentbyarray);  // 2026-11-11 - see SentBy.h
    serial.print(F("#"));
    printRoutesToken(serial, data.serialnumberarray, data.secondsTime);  // last token
    serial.println();
}

LangleyDataSerializer::~LangleyDataSerializer() {}
