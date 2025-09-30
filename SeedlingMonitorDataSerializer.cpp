#include <SeedlingMonitorDataSerializer.h>

void SeedlingMonitorDataSerializer::pushToSerial(HardwareSerial &serial, SeedlingMonitorData seedlingMonitorData)
{

    serial.print("SeedlingMonitorDataDeserializer");
    serial.print(F("#"));

    // definition
    for (int i = 0; i < sizeof(seedlingMonitorData.deviceTypeId); i++)
    {
        if (seedlingMonitorData.deviceTypeId[i] != NULL)
            serial.print(seedlingMonitorData.deviceTypeId[i]);
    }
    serial.print(F("#"));

    for (int i = 0; i < sizeof(seedlingMonitorData.devicename); i++)
    {
        if (seedlingMonitorData.devicename[i] != NULL && seedlingMonitorData.devicename[i] != '\n' && seedlingMonitorData.devicename[i] != '\r')
            serial.print(seedlingMonitorData.devicename[i]);
    }
    serial.print(F("#"));
    for (int i = 0; i < sizeof(seedlingMonitorData.deviceshortname); i++)
    {
        if (seedlingMonitorData.deviceshortname[i] != NULL && seedlingMonitorData.deviceshortname[i] != '\n' && seedlingMonitorData.deviceshortname[i] != '\r')
            serial.print(seedlingMonitorData.deviceshortname[i]);
    }
    serial.print(F("#"));
    for (int i = 0; i < sizeof(seedlingMonitorData.serialnumberarray); i++)
    {
        if (seedlingMonitorData.serialnumberarray[i] != NULL)
            serial.print(seedlingMonitorData.serialnumberarray[i], HEX);
    }
    serial.print(F("#"));

    for (int i = 0; i < sizeof(seedlingMonitorData.groupidentifier); i++)
    {
        if (seedlingMonitorData.groupidentifier[i] != NULL)
            serial.print(seedlingMonitorData.groupidentifier[i]);
    }
    serial.print(F("#"));


    serial.print(seedlingMonitorData.dataSamplingSec);
    serial.print(F("#"));

    serial.print(seedlingMonitorData.latitude);
    serial.print(F("#"));

    serial.print(seedlingMonitorData.longitude);
    serial.print(F("#"));
    
    // Purpose
    serial.print(seedlingMonitorData.greenhouseTemp);
    serial.print(F("#"));

    serial.print(seedlingMonitorData.greenhouseHum);
    serial.print(F("#"));

    serial.print(seedlingMonitorData.temperature);
    serial.print(F("#"));
    serial.print(seedlingMonitorData.outdoorTemperature);
    serial.print(F("#"));
    serial.print(seedlingMonitorData.humidifierstatus);

    

    serial.print(F("#"));
    serial.print(seedlingMonitorData.secondsTime);
    
    serial.print(F("#"));
    serial.print(seedlingMonitorData.rtcBatVolt);
    serial.print(F("#"));
    
    serial.print(seedlingMonitorData.rssi);
    serial.print(F("#"));
    serial.print(seedlingMonitorData.snr);
    serial.print(F("#"));

    serial.print(seedlingMonitorData.operatingStatus);
    serial.print(F("#"));
    serial.print(seedlingMonitorData.digitalStablesUpload);
    serial.print(F("#"));
    serial.print(seedlingMonitorData.secondsSinceLastPulse);
    serial.print(F("#"));

    serial.print(seedlingMonitorData.checksum);
    serial.print(F("#"));
    serial.print(seedlingMonitorData.loraActive);
    serial.print(F("#"));
    serial.print(seedlingMonitorData.dsLastUpload);
    serial.print(F("#"));
    serial.print(seedlingMonitorData.totpcode);
    serial.print(F("#"));
    serial.print("END");
    serial.println(F("#"));
}

SeedlingMonitorDataSerializer::~SeedlingMonitorDataSerializer() {}

SeedlingMonitorDataDeserializer#SeedlingMonitor##SEED##9slwJcM9#3#0.00#0.00#18.00#70.00#26.12#12.38#0#
-1664857306#2.71#-70.00#10.00#
0.00#
0#
0
#
0
#0
#0
#0
#Ok-AsyncData
