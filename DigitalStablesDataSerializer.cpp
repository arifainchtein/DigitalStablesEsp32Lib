#include <DigitalStablesDataSerializer.h>

namespace {
// Prints a fixed-size char buffer as a proper C string: stops at the first '\0'
// instead of scanning the whole buffer and printing every non-null byte. The
// latter prints stale leftover bytes (e.g. "low" instead of "" after a field is
// overwritten with a shorter value) since a short write only nulls the bytes it
// touches, not the rest of the buffer.
void printFixedCString(HardwareSerial &serial, const char *buf, size_t size, bool stripNewlines = false)
{
    for (size_t i = 0; i < size && buf[i] != '\0'; i++)
    {
        if (stripNewlines && (buf[i] == '\n' || buf[i] == '\r')) continue;
        serial.print(buf[i]);
    }
}
}

void DigitalStablesDataSerializer::pushToSerial(HardwareSerial &serial, DigitalStablesData digitalStablesData)
{

    serial.print("DigitalStablesDataDeserializer");
    serial.print(F("#"));

    // definition
    printFixedCString(serial, digitalStablesData.deviceTypeId, sizeof(digitalStablesData.deviceTypeId));
    serial.print(F("#"));

    printFixedCString(serial, digitalStablesData.devicename, sizeof(digitalStablesData.devicename), true);
    serial.print(F("#"));
    printFixedCString(serial, digitalStablesData.deviceshortname, sizeof(digitalStablesData.deviceshortname), true);
    serial.print(F("#"));
    for (int i = 0; i < sizeof(digitalStablesData.serialnumberarray); i++)
    {
        if (digitalStablesData.serialnumberarray[i] != NULL)
            serial.print(digitalStablesData.serialnumberarray[i], HEX);
    }
    serial.print(F("#"));

    printFixedCString(serial, digitalStablesData.groupidentifier, sizeof(digitalStablesData.groupidentifier));
    serial.print(F("#"));

    serial.print(digitalStablesData.currentFunctionValue);
    serial.print(F("#"));

    serial.print(digitalStablesData.dataSamplingSec);
    serial.print(F("#"));

    serial.print(digitalStablesData.latitude);
    serial.print(F("#"));

    serial.print(digitalStablesData.longitude);
    serial.print(F("#"));
    //
    // Sensors
    //
    printFixedCString(serial, digitalStablesData.sensor1name, sizeof(digitalStablesData.sensor1name));
    serial.print(F("#"));
    serial.print(digitalStablesData.qfactor1);
    serial.print(F("#"));

    printFixedCString(serial, digitalStablesData.sensor2name, sizeof(digitalStablesData.sensor2name));
    serial.print(F("#"));

    serial.print(digitalStablesData.qfactor2);
    serial.print(F("#"));

    serial.print(digitalStablesData.tank1HeightMeters);
    serial.print(F("#"));

    serial.print(digitalStablesData.tank1maxvollit);
    serial.print(F("#"));

    serial.print(digitalStablesData.tank2HeightMeters);
    serial.print(F("#"));

    serial.print(digitalStablesData.tank2maxvollit);
    serial.print(F("#"));

    serial.print(digitalStablesData.maximumScepticHeight);
    serial.print(F("#"));

    // Purpose
    serial.print(digitalStablesData.secondsTime);
    serial.print(F("#"));
    serial.print(digitalStablesData.temperature);
    serial.print(F("#"));
    serial.print(digitalStablesData.rtcBatVolt);
    serial.print(F("#"));
    serial.print(digitalStablesData.opMode);
    serial.print(F("#"));
    serial.print(digitalStablesData.rssi);
    serial.print(F("#"));
    serial.print(digitalStablesData.snr);
    serial.print(F("#"));
    serial.print(digitalStablesData.flowRate);
    serial.print(F("#"));
    serial.print(digitalStablesData.totalMilliLitres);
    serial.print(F("#"));
    serial.print(digitalStablesData.flowRate2);
    serial.print(F("#"));
    serial.print(digitalStablesData.totalMilliLitres2);
    serial.print(F("#"));

    serial.print(digitalStablesData.tank1PressurePsi);
    serial.print(F("#"));
    serial.print(digitalStablesData.tank2PressurePsi);
    serial.print(F("#"));

    serial.print(digitalStablesData.operatingStatus);
    serial.print(F("#"));
    serial.print(digitalStablesData.digitalStablesUpload);
    serial.print(F("#"));
  
    serial.print(digitalStablesData.checksum);
    serial.print(F("#"));
    serial.print(digitalStablesData.loraActive);
    serial.print(F("#"));
    serial.print(digitalStablesData.dsLastUpload);
    serial.print(F("#"));
    serial.print(digitalStablesData.batteryVoltage);
    serial.print(F("#"));
    serial.print(digitalStablesData.v50Voltage);
    serial.print(F("#"));
    serial.print(digitalStablesData.batteryCurrent);
    serial.print(F("#"));
    serial.print(digitalStablesData.estimatedRuntime);
    serial.print(F("#"));
    
    serial.print(digitalStablesData.totpcode);
    serial.print(F("#"));
    serial.print(digitalStablesData.outdoortemperature);
    serial.print(F("#"));
    serial.print(digitalStablesData.outdoorhumidity);
    serial.print(F("#"));

    serial.print(digitalStablesData.measuredHeight);
    serial.print(F("#"));
    serial.print(digitalStablesData.panelVoltage);
    serial.print(F("#"));
    serial.print(digitalStablesData.lux);
    serial.print(F("#"));
    serial.print(digitalStablesData.sleepTime);
    serial.print(F("#"));
    serial.print(digitalStablesData.ledBrightness);
    serial.print(F("#"));
    serial.print(digitalStablesData.altitude);
    serial.print(F("#"));
    serial.print(digitalStablesData.minimumEfficiencyForLed);
    serial.print(F("#"));
    serial.print(digitalStablesData.minimumEfficiencyForWifi);
    serial.print(F("#"));

// AsyncData Values
// value     storedata     sendlora                                           in teleonome
// 1 =          n             n    debug statement in setup                         n
// 2 =          y             n    end of set up efficiemcy test                    yes - goes to sleep mode
// 3 =          y             n    end of setup test for battery voltage            yes  -goes to sleep mode
// 6 =          y             n    checking remotemonitortimer                       n
// 7 =          y             n    in loop after before led display code            yes  was in loop mode and went to sleep
// 9 =          n             y    in normal cycle                                  yes  in loop mode  
// 10 =         y             n    Serial Command                                    n

    serial.print(digitalStablesData.asyncdata);
    serial.print(F("#"));
    serial.print(digitalStablesData.wakeTimeSec);
    serial.print(F("#"));
    serial.print(digitalStablesData.wifiStatus);
    serial.print(F("#"));
    serial.print(digitalStablesData.panelCurrent);
    serial.println(F("#"));
}

DigitalStablesDataSerializer::~DigitalStablesDataSerializer() {}


