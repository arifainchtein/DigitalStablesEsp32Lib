#include <PanchoTankFlowSerializer.h>


void PanchoTankFlowSerializer::pushToSerial(HardwareSerial& serial , PanchoTankFlowData& panchoTankFlowData){
    
    //definition
    for(int i = 0; i < sizeof(panchoTankFlowData.deviceTypeId); i++){
        if(panchoTankFlowData.deviceTypeId[i]!=NULL)Serial.print(panchoTankFlowData.deviceTypeId[i]);
    }
    serial.print(F("#"));

    for(int i = 0; i < sizeof(panchoTankFlowData.devicename); i++){
        if(panchoTankFlowData.deviceshortname[i]!=NULL &&  panchoTankFlowData.devicename[i]!='\n' &&  panchoTankFlowData.devicename[i]!='\r')Serial.print(panchoTankFlowData.devicename[i]);
     }
    serial.print(F("#"));
    for(int i = 0; i < sizeof(panchoTankFlowData.deviceshortname); i++){
        if(panchoTankFlowData.deviceshortname[i]!=NULL &&  panchoTankFlowData.deviceshortname[i]!='\n' &&  panchoTankFlowData.deviceshortname[i]!='\r')Serial.print(panchoTankFlowData.deviceshortname[i]);
    }
    serial.print(F("#"));
     for(int i = 0; i < sizeof(panchoTankFlowData.serialnumberarray); i++){
       if(panchoTankFlowData.serialnumberarray[i]!=NULL) Serial.print(panchoTankFlowData.serialnumberarray[i],HEX);
    }
     serial.print(F("#"));
      for(int i = 0; i < sizeof(panchoTankFlowData.groupidentifier); i++){
        if(panchoTankFlowData.groupidentifier[i]!=NULL)Serial.print(panchoTankFlowData.groupidentifier[i]);
    }
    serial.print(F("#"));
    serial.print(panchoTankFlowData.currentFunctionValue);
    serial.print(F("#"));

    serial.print(panchoTankFlowData.dataSamplingSec);
    serial.print(F("#"));

   
     serial.print(panchoTankFlowData.latitude);
     serial.print(F("#"));
     serial.print(panchoTankFlowData.longitude);
      serial.print(F("#"));
    //
    // Sensors
    //
    for(int i = 0; i < sizeof(panchoTankFlowData.flow1name); i++){
        if(panchoTankFlowData.flow1name[i]!=NULL)Serial.print(panchoTankFlowData.flow1name[i]);
    }
    serial.print(F("#"));
     serial.print(panchoTankFlowData.qfactor1);
     serial.print(F("#"));

    for(int i = 0; i < sizeof(panchoTankFlowData.flow2name); i++){
        if(panchoTankFlowData.flow2name[i]!=NULL)Serial.print(panchoTankFlowData.flow2name[i]);
    }
     serial.print(F("#"));
     serial.print(panchoTankFlowData.qfactor2);
     serial.print(F("#"));

    for(int i = 0; i < sizeof(panchoTankFlowData.tank1name); i++){
        if(panchoTankFlowData.tank1name[i]!=NULL)Serial.print(panchoTankFlowData.tank1name[i]);
    }
     serial.print(F("#"));
    serial.print(panchoTankFlowData.tank1HeightMeters);
     serial.print(F("#"));
    
    for(int i = 0; i < sizeof(panchoTankFlowData.tank2name); i++){
        if(panchoTankFlowData.tank2name[i]!=NULL)Serial.print(panchoTankFlowData.tank2name[i]);
    }
    serial.print(F("#"));
    serial.print(panchoTankFlowData.tank2HeightMeters);
     serial.print(F("#"));


   

    // Purpose
   serial.print(panchoTankFlowData.secondsTime);
     serial.print(F("#"));
   
    serial.print(panchoTankFlowData.temperature);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.rtcBatVolt);
     serial.print(F("#"));
   serial.print(panchoTankFlowData.opMode);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.rssi);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.snr);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.flowRate);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.totalMilliLitres);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.flowRate2);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.totalMilliLitres2);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.tank1PressurePsi);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.tank1PressureVolts);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.tank1WaterLevel);
     serial.print(F("#"));
    
    serial.print(panchoTankFlowData.tank2PressurePsi);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.tank2PressureVolts);
     serial.print(F("#"));
    serial.print( panchoTankFlowData.tank2WaterLevel);
     serial.print(F("#"));
  
       
    serial.print(panchoTankFlowData.operatingStatus);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.digitalStablesUpload);
     serial.print(F("#"));
    serial.print(panchoTankFlowData.secondsSinceLastPulse);
     serial.println(F("#"));
    
    // serial.print(panchoTankFlowData.hostname);
    //  serial.print(F("#"));
    // serial.print(panchoTankFlowData.ssid);
    //  serial.print(F("#"));
    // serial.print(panchoTankFlowData.lora);
    //  serial.print(F("#"));
    // serial.print(panchoTankFlowData.internetAvailable);
    //  serial.print(F("#"));
    // serial.print(panchoTankFlowData.ipAddress);
    //  serial.print(F("#"));
   
    }

PanchoTankFlowSerializer::~PanchoTankFlowSerializer() {}