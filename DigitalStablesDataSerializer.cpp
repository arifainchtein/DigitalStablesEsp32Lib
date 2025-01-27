#include <DigitalStablesDataSerializer.h>

void DigitalStablesDataSerializer::pushToSerial(HardwareSerial &serial, DigitalStablesData digitalStablesData)
{

    serial.print("DigitalStablesDataDeserializer");
    serial.print(F("#"));

    // definition
    for (int i = 0; i < sizeof(digitalStablesData.deviceTypeId); i++)
    {
        if (digitalStablesData.deviceTypeId[i] != NULL)
            serial.print(digitalStablesData.deviceTypeId[i]);
    }
    serial.print(F("#"));

    for (int i = 0; i < sizeof(digitalStablesData.devicename); i++)
    {
        if (digitalStablesData.devicename[i] != NULL && digitalStablesData.devicename[i] != '\n' && digitalStablesData.devicename[i] != '\r')
            serial.print(digitalStablesData.devicename[i]);
    }
    serial.print(F("#"));
    for (int i = 0; i < sizeof(digitalStablesData.deviceshortname); i++)
    {
        if (digitalStablesData.deviceshortname[i] != NULL && digitalStablesData.deviceshortname[i] != '\n' && digitalStablesData.deviceshortname[i] != '\r')
            serial.print(digitalStablesData.deviceshortname[i]);
    }
    serial.print(F("#"));
    for (int i = 0; i < sizeof(digitalStablesData.serialnumberarray); i++)
    {
        if (digitalStablesData.serialnumberarray[i] != NULL)
            serial.print(digitalStablesData.serialnumberarray[i], HEX);
    }
    serial.print(F("#"));

    for (int i = 0; i < sizeof(digitalStablesData.groupidentifier); i++)
    {
        if (digitalStablesData.groupidentifier[i] != NULL)
            serial.print(digitalStablesData.groupidentifier[i]);
    }
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
    for (int i = 0; i < sizeof(digitalStablesData.sensor1name); i++)
    {
        if (digitalStablesData.sensor1name[i] != NULL)
            serial.print(digitalStablesData.sensor1name[i]);
    }
    serial.print(F("#"));
    serial.print(digitalStablesData.qfactor1);
    serial.print(F("#"));

    for (int i = 0; i < sizeof(digitalStablesData.sensor2name); i++)
    {
        if (digitalStablesData.sensor2name[i] != NULL)
            serial.print(digitalStablesData.sensor2name[i]);
    }
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
    serial.print(digitalStablesData.secondsSinceLastPulse);
    serial.print(F("#"));

    serial.print(digitalStablesData.checksum);
    serial.print(F("#"));
    serial.print(digitalStablesData.loraActive);
    serial.print(F("#"));
    serial.print(digitalStablesData.dsLastUpload);
    serial.print(F("#"));
    serial.print(digitalStablesData.solarVoltage);
    serial.print(F("#"));
    serial.print(digitalStablesData.capacitorVoltage);
    serial.print(F("#"));

    serial.print(digitalStablesData.totpcode);
    serial.print(F("#"));
    serial.print(digitalStablesData.outdoortemperature);
    serial.print(F("#"));
    serial.print(digitalStablesData.outdoorhumidity);
    serial.print(F("#"));

    serial.print(digitalStablesData.measuredHeight);
    serial.print(F("#"));
    serial.print(digitalStablesData.scepticAvailablePercentage);
    serial.print(F("#"));
    serial.print(digitalStablesData.lux);
    serial.print(F("#"));
    serial.print(digitalStablesData.sleepTime);
    serial.print(F("#"));
    serial.print(digitalStablesData.ledBrightness);
    serial.println(F("#"));
}

DigitalStablesDataSerializer::~DigitalStablesDataSerializer() {}