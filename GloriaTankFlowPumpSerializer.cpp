#include <GloriaTankFlowPumpSerializer.h>


void GloriaTankFlowPumpSerializer::pushToSerial(HardwareSerial& serial , GloriaTankFlowPumpData& gloriaTankFlowPumpData){
  //
  //definition
  //
   for(int i = 0; i < sizeof(gloriaTankFlowPumpData.deviceTypeId); i++){
        if(gloriaTankFlowPumpData.deviceTypeId[i]!=NULL)Serial.print(gloriaTankFlowPumpData.deviceTypeId[i]);
    }
     serial.print(F("#"));

     for(int i = 0; i < sizeof(gloriaTankFlowPumpData.devicename); i++){
        if(gloriaTankFlowPumpData.devicename[i]!=NULL &&  gloriaTankFlowPumpData.devicename[i]!='\n' &&  gloriaTankFlowPumpData.devicename[i]!='\r')Serial.print(gloriaTankFlowPumpData.devicename[i]);
     }
    serial.print(F("#"));
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.deviceshortname); i++){
        if(gloriaTankFlowPumpData.deviceshortname[i]!=NULL &&  gloriaTankFlowPumpData.deviceshortname[i]!='\n' &&  gloriaTankFlowPumpData.deviceshortname[i]!='\r')Serial.print(gloriaTankFlowPumpData.deviceshortname[i]);
    }
    serial.print(F("#"));

     for(int i = 0; i < sizeof(gloriaTankFlowPumpData.serialnumberarray); i++){
       if(gloriaTankFlowPumpData.serialnumberarray[i]!=NULL) Serial.print(gloriaTankFlowPumpData.serialnumberarray[i], HEX);
    }
    serial.print(F("#"));

    //  for(int i = 0; i < sizeof(gloriaTankFlowPumpData.groupidentifier); i++){
    //     if(gloriaTankFlowPumpData.groupidentifier[i]!=NULL &&  gloriaTankFlowPumpData.groupidentifier[i]!='\n' &&  gloriaTankFlowPumpData.groupidentifier[i]!='\r'){
    //         Serial.print(gloriaTankFlowPumpData.groupidentifier[i]);
    //     }
    // }
     serial.print(F("#"));

    serial.print(gloriaTankFlowPumpData.currentFunctionValue);
    serial.print(F("#"));
      serial.print(gloriaTankFlowPumpData.dataSamplingSec);
     serial.print(F("#"));
     serial.print(gloriaTankFlowPumpData.latitude);
     serial.print(F("#"));
     serial.print(gloriaTankFlowPumpData.longitude);
      serial.print(F("#"));

     //
     // sensors
     //
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.flow1name); i++){
        if(gloriaTankFlowPumpData.flow1name[i]!=NULL)Serial.print(gloriaTankFlowPumpData.flow1name[i]);
    }
   
      serial.print(F("#"));
     serial.print(gloriaTankFlowPumpData.qfactor1);
      serial.print(F("#"));
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.flow2name); i++){
        if(gloriaTankFlowPumpData.flow2name[i]!=NULL)Serial.print(gloriaTankFlowPumpData.flow2name[i]);
    }
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.qfactor2);
     serial.print(F("#"));
     
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.tank1name); i++){
        if(gloriaTankFlowPumpData.tank1name[i]!=NULL)Serial.print(gloriaTankFlowPumpData.tank1name[i]);
    }
     serial.print(F("#"));
      serial.print(gloriaTankFlowPumpData.tank1HeightMeters);
     serial.print(F("#"));
    for(int i = 0; i < sizeof(gloriaTankFlowPumpData.tank2name); i++){
        if(gloriaTankFlowPumpData.tank2name[i]!=NULL)Serial.print(gloriaTankFlowPumpData.tank2name[i]);
    }
       serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.tank2HeightMeters);
     serial.print(F("#"));
  
     //
     // purpose
     // 
   serial.print(gloriaTankFlowPumpData.secondsTime);
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
    // serial.print(gloriaTankFlowPumpData.tank1PressureVolts);
    //  serial.print(F("#"));
    // serial.print(gloriaTankFlowPumpData.tank1WaterLevel);
    //  serial.print(F("#"));
   
    serial.print(gloriaTankFlowPumpData.tank2PressurePsi);
     serial.print(F("#"));
    // serial.print(gloriaTankFlowPumpData.tank2PressureVolts);
    //  serial.print(F("#"));
    // serial.print( gloriaTankFlowPumpData.tank2WaterLevel);
    //  serial.print(F("#"));
     
    serial.print(gloriaTankFlowPumpData.operatingStatus);
     serial.print(F("#"));
    
    serial.print(gloriaTankFlowPumpData.digitalStablesUpload);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.secondsSinceLastPulse);
     serial.print(F("#"));
     //serial.print(gloriaTankFlowPumpData.sleepPingMinutes);
     serial.print(66);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.solarvoltage);
     serial.print(F("#"));
    serial.print(gloriaTankFlowPumpData.capacitorvoltage);
     serial.println(F("#"));
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
    
    }

GloriaTankFlowPumpSerializer::~GloriaTankFlowPumpSerializer() {}