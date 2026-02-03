#include <ChinampaDataSerializer.h>

void ChinampaDataSerializer::pushToSerial(HardwareSerial &serial, ChinampaData chinampaData)
{

    serial.print("ChinampaDataDeserializer");
    serial.print(F("#"));

    // definition
    for (int i = 0; i < sizeof(chinampaData.deviceTypeId); i++)
    {
        if (chinampaData.deviceTypeId[i] != NULL)
            serial.print(chinampaData.deviceTypeId[i]);
    }
    serial.print(F("#"));

    for (int i = 0; i < sizeof(chinampaData.devicename); i++)
    {
        if (chinampaData.devicename[i] != NULL && chinampaData.devicename[i] != '\n' && chinampaData.devicename[i] != '\r')
            serial.print(chinampaData.devicename[i]);
    }
    serial.print(F("#"));
    for (int i = 0; i < sizeof(chinampaData.deviceshortname); i++)
    {
        if (chinampaData.deviceshortname[i] != NULL && chinampaData.deviceshortname[i] != '\n' && chinampaData.deviceshortname[i] != '\r')
            serial.print(chinampaData.deviceshortname[i]);
    }
    serial.print(F("#"));
    for (int i = 0; i < sizeof(chinampaData.serialnumberarray); i++)
    {
        if (chinampaData.serialnumberarray[i] != NULL)
            serial.print(chinampaData.serialnumberarray[i], HEX);
    }
    serial.print(F("#"));

    for (int i = 0; i < sizeof(chinampaData.groupidentifier); i++)
    {
        if (chinampaData.groupidentifier[i] != NULL)
            serial.print(chinampaData.groupidentifier[i]);
    }
    serial.print(F("#"));
    serial.print(chinampaData.dataSamplingSec);
    serial.print(F("#"));

    serial.print(chinampaData.latitude);
    serial.print(F("#"));

    serial.print(chinampaData.longitude);
    serial.print(F("#"));
    
    // Purpose
    
    serial.print(chinampaData.pumprelaystatus);
    serial.print(F("#"));

    serial.print(chinampaData.fishtankoutflowsolenoidrelaystatus);
    serial.print(F("#"));

    
    serial.print(chinampaData.fishtankoutflowflowRate);
    serial.print(F("#"));

     serial.print(chinampaData.fishtankoutQFactor);
    serial.print(F("#"));


    serial.print(chinampaData.minimumFishTankLevel);
    serial.print(F("#"));
    serial.print(chinampaData.maximumFishTankLevel);
    serial.print(F("#"));
    serial.print(chinampaData.fishTankMeasuredHeight);
    serial.print(F("#"));
    serial.print(chinampaData.fishTankHeight);
     serial.print(F("#"));
    serial.print(chinampaData.minimumSumpTroughLevel);
     serial.print(F("#"));
      serial.print(chinampaData.maximumSumpTroughLevel);
     serial.print(F("#"));
    serial.print(chinampaData.sumpTroughMeasuredHeight);
     serial.print(F("#"));
    serial.print(chinampaData.sumpTroughHeight);
     serial.print(F("#"));
    serial.print(chinampaData.sumpTroughStaleDataSeconds);
     serial.print(F("#"));
    serial.print(chinampaData.fishTankStaleDataSeconds);
    serial.print(F("#"));
    serial.print(chinampaData.alertstatus);
     serial.print(F("#"));
      serial.print(chinampaData.alertcode);



    serial.print(F("#"));
    serial.print(chinampaData.pumpflowRate);
    serial.print(F("#"));
    serial.print(chinampaData.microtemperature);
    
    serial.print(F("#"));
    serial.print(chinampaData.secondsTime);
    
    serial.print(F("#"));
    serial.print(chinampaData.rtcBatVolt);
    serial.print(F("#"));
    
    serial.print(chinampaData.rssi);
    serial.print(F("#"));
    serial.print(chinampaData.snr);
    serial.print(F("#"));

    serial.print(chinampaData.digitalStablesUpload);
    serial.print(F("#"));
    serial.print(chinampaData.secondsSinceLastPulse);
    serial.print(F("#"));

    serial.print(chinampaData.checksum);
    serial.print(F("#"));
    serial.print(chinampaData.loraActive);
    serial.print(F("#"));
    serial.print(chinampaData.dsLastUpload);
    serial.print(F("#"));
    serial.print(chinampaData.totpcode);
    serial.print(F("#"));
    serial.print(chinampaData.secondsSinceLastFishTankData);
    serial.print(F("#"));
    
    serial.print(chinampaData.previousFishTankMeasuredHeight);
    ;
    serial.print(F("#"));
    serial.print(chinampaData.secondsSinceLastSumpTroughData);
    serial.print(F("#"));
    serial.print(chinampaData.previousSumpTroughMeasuredHeight);
     serial.print(F("#"));
    for (int i = 0; i < sizeof(chinampaData.sensorstatus); i++)
    {
        if (chinampaData.sensorstatus[i] != NULL){
            serial.print(chinampaData.sensorstatus[i]);
        }   
         serial.print(F("#"));
    }

    serial.print(F("#"));
    serial.print(chinampaData.microtemperatureMaximum);
    serial.print(F("#"));
    serial.print(chinampaData.outdoortemperature);
    serial.print(F("#"));
    serial.print(chinampaData.outdoorhumidity);

}

ChinampaDataSerializer::~ChinampaDataSerializer() {}