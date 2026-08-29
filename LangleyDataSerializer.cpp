#include <LangleyDataSerializer.h>

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
    for (int i = 0; i < (int)sizeof(data.serialnumberarray); i++) {
        if (data.serialnumberarray[i] != 0)
            serial.print(data.serialnumberarray[i], HEX);
    }
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
    serial.print(data.wpsFrequencySeconds);
    serial.print(F("#"));
    serial.print(data.maxWPSVoltage);
    serial.print(F("#"));
    serial.print(data.minWPSVoltage);
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
    serial.println(data.checksum);
}

LangleyDataSerializer::~LangleyDataSerializer() {}
