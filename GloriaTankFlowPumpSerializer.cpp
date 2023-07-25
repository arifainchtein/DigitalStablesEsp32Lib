#include <GloriaTankFlowPumpSerializer.h>

GloriaTankFlowPumpSerializer::GloriaTankFlowPumpSerializer(){}


void GloriaTankFlowPumpSerializer::pushToSerial(HardwareSerial& serial , GloriaTankFlowPumpData& gloriaTankFlowPumpData){
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.devicename); i++){
        Serial.print(gloriaTankFlowPumpData.devicename[i]);
    }
    serial.print(F("#"));
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.deviceshortname); i++){
        Serial.print(gloriaTankFlowPumpData.deviceshortname[i]);
    }
    serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.currentFunctionValue);
    serial.print(F("#"));
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.flow1name); i++){
        Serial.print(gloriaTankFlowPumpData.flow1name[i]);
    }
    serial.print(F("#"));
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.flow2name); i++){
        Serial.print(gloriaTankFlowPumpData.flow2name[i]);
    }
     serial.print(F("#"));
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.tank1name); i++){
        Serial.print(gloriaTankFlowPumpData.tank1name[i]);
    }
     serial.print(F("#"));
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.tank2name); i++){
        Serial.print(gloriaTankFlowPumpData.tank2name[i]);
    }
     serial.print(F("#"));
      for(int i = 0; i < sizeof(gloriaTankFlowPumpData.groupidentifier); i++){
        Serial.print(gloriaTankFlowPumpData.groupidentifier[i]);
    }
     serial.print(F("#"));
      serial.print(gloriaTankFlowPumpData.deviceTypeId);
      for(int i = 0; i < sizeof(gloriaTankFlowPumpData.deviceTypeId); i++){
        Serial.print(gloriaTankFlowPumpData.deviceTypeId[i]);
    }
     serial.print(F("#"));
   serial.print(gloriaTankFlowPumpData.secondsTime);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.dataSamplingSec);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.temperature);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.rtcBatVolt);
     serial.print(F("#"));
   serial.print(gloriaTankFlowPumpData.opMode);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.rssi);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.snr);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.flowRate);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.totalMilliLitres);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.flowRate2);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.totalMilliLitres2);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.tank1PressurePsi);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.tank1PressureVolts);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.tank1WaterLevel);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.tank1HeightMeters);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.tank2PressurePsi);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.tank2PressureVolts);
     serial.print(F("#"));
    serial.print( gloriaTankFlowPumpData.tank2WaterLevel);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.tank2HeightMeters);
     serial.print(F("#"));
   serial.print(gloriaTankFlowPumpData.qfactor1);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.qfactor2);
     serial.print(F("#"));
   serial.print(gloriaTankFlowPumpData.rtcBatVolt); 
     serial.print(F("#"));       
    serial.print(gloriaTankFlowPumpData.operatingStatus);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.sleepPingMinutes);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.digitalStablesUpload);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.secondsSinceLastPulse);
     serial.print(F("#"));
     for(int i = 0; i < sizeof(gloriaTankFlowPumpData.serialnumberarray); i++){
        Serial.print(gloriaTankFlowPumpData.serialnumberarray[i]);
    }
     serial.print(F("#"));
    // serial.print(gloriaTankFlowPumpData.hostname);
    //  serial.print(F("#"));
    // serial.print(gloriaTankFlowPumpData.ssid);
    //  serial.print(F("#"));
    // serial.print(gloriaTankFlowPumpData.lora);
    //  serial.print(F("#"));
    // serial.print(gloriaTankFlowPumpData.internetAvailable);
    //  serial.print(F("#"));
    // serial.print(gloriaTankFlowPumpData.ipAddress);
    //  serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.latitude);
     serial.print(F("#"));
     serial.print(gloriaTankFlowPumpData.longitude);
      serial.print(F("#"));
    }

