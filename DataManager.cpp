#include "GloriaTankFlowPumpData.h"
#include "RosieData.h"
#include "PanchoTankFlowData.h"
#include "DaffodilData.h"
#include "DataManager.h"

JsonObject obj;
//GloriaTankFlowPumpData gloriaTankFlowPumpData;
//RosieData rosieData;
//PanchoTankFlowData panchoaTankFlowData;
//String keys[126];


String  sn="";
DataManager::DataManager(HardwareSerial &serial) : _HardSerial(serial) {}

void DataManager::start(){
}

void DataManager::storePancho(PanchoTankFlowData& panchoTankFlowData){
   sn="";
  uint8_t checksum=0;
for (  uint8_t i = 0; i < 8; i++) {  
  sn += String(panchoTankFlowData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(panchoTankFlowData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
  _HardSerial.print("adding a pancho serialNumber=");
   _HardSerial.print(sn);
if(panchoTankFlowData.checksum==checksum  && sn.length()==15){
//if(serialNumber.length()==15){
  obj = completeObject.createNestedObject(sn);
  generatePanchoTankFlowDataWebData(panchoTankFlowData, obj);
  
  _HardSerial.print("  number of devices=");
   _HardSerial.println(completeObject.size());
}else{
   _HardSerial.print("  pancho rejected pulse serialnumne=");
   _HardSerial.println(sn);
   
}
  
   
}

void DataManager::storeDaffodil(DaffodilData& daffodilData){
 sn="";
  uint8_t checksum=0;
for (  uint8_t i = 0; i < 8; i++) {  
    sn += String(daffodilData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(daffodilData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
   _HardSerial.print("adding a daffodilData  serialNumber=");
   _HardSerial.print(sn);
    _HardSerial.print(" l=");
   _HardSerial.print(sn.length());
    _HardSerial.print(" checksum=");
   _HardSerial.println(checksum);
 _HardSerial.print(" daffodilData.checksum=");
   _HardSerial.println(daffodilData.checksum);
   
if(daffodilData.checksum==checksum && (sn.length()==15 ||sn.length()==14)){
//if(){
 obj = completeObject.createNestedObject(sn);
  generateDaffodilWebData(daffodilData, obj);
  
  serializeJsonPretty(obj, _HardSerial);
 
  _HardSerial.print(" number of devices=");
   _HardSerial.println(completeObject.size());
}else{
   _HardSerial.println(" daffodilData rejected pulse serialnumne=");
   _HardSerial.println(sn);
   
}
}

void DataManager::storeRosie(RosieData& rosieData){
 sn="";
  uint8_t checksum=0;
for (  uint8_t i = 0; i < 8; i++) {  
    sn += String(rosieData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(rosieData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
if(rosieData.checksum==checksum && sn.length()==15 ){
//if(){
 obj = completeObject.createNestedObject(sn);
  generateRosieWebData(rosieData, obj);
   serializeJsonPretty(obj, _HardSerial);
 
  _HardSerial.print("adding a rosie  serialNumber=");
   _HardSerial.print(sn);
  _HardSerial.print(" number of devices=");
   _HardSerial.println(completeObject.size());
}else{
   _HardSerial.println(" rosie rejected pulse serialnumne=");
   _HardSerial.println(sn);
   
}
}

void DataManager::storeGloria(GloriaTankFlowPumpData& gloriaTankFlowPumpData){
 sn="";
   uint8_t checksum=0;
for (  uint8_t i = 0; i < 8; i++) {  
    sn += String(gloriaTankFlowPumpData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(gloriaTankFlowPumpData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
if(gloriaTankFlowPumpData.checksum==checksum  && (sn.length()==15|| sn.length()==14)){
//if(serialNumber.length()==15){

 obj = completeObject.createNestedObject(sn);
  generateGloriaTankFlowPumpWebData(gloriaTankFlowPumpData, obj);

  _HardSerial.print("adding a gloria serialNumber=");
   _HardSerial.print(sn);
  _HardSerial.print("  number of devices=");
   _HardSerial.println(completeObject.size());
}else{
   _HardSerial.print("gloria rejected pulse serialnumne=");
   _HardSerial.println(sn);
    _HardSerial.print("gloriatata checksum=");
   _HardSerial.println(gloriaTankFlowPumpData.checksum);
   _HardSerial.print("checksum=");
   _HardSerial.println(checksum);
   
}
}

void DataManager::generateDaffodilWebData(DaffodilData &daffodilData, JsonObject &json){
    
    json["devicename"] = daffodilData.devicename;
    json["deviceshortname"] = daffodilData.deviceshortname;
    json["groupidentifier"]=daffodilData.groupidentifier;
    json["secondsTime"] = daffodilData.secondsTime;
    json["dataSamplingSec"] = daffodilData.dataSamplingSec;
    json["currentFunctionValue"] = daffodilData.currentFunctionValue;
    json["temperature"] = daffodilData.temperature;
    json["rtcBatVolt"] = daffodilData.rtcBatVolt;
    json["opMode"] = daffodilData.opMode;
    json["rtcBatVolt"] = daffodilData.rtcBatVolt;        
    json["operatingStatus"] = daffodilData.operatingStatus;
    json["sleepPingMinutes"] = daffodilData.sleepPingMinutes;
    json["digitalStablesUpload"] = daffodilData.digitalStablesUpload;
    json["secondsSinceLastPulse"] = daffodilData.secondsSinceLastPulse;
    json["currentFunctionValue"]=daffodilData.currentFunctionValue;
    json["outdoortemperature"] = daffodilData.outdoortemperature;
    json["outdoorhumidity"] = daffodilData.outdoorhumidity;
    json["minimumSepticHeight"] = daffodilData.minimumSepticHeight;
    json["maximumScepticHeight"] = daffodilData.maximumScepticHeight;
    json["scepticAvailablePercentage"] = daffodilData.scepticAvailablePercentage;
    json["capacitorVoltage"]=daffodilData.capacitorVoltage;
  //  json["soft_ap_ssid"] = sn;
    json["serialnumber"] = sn;
    json["sentBy"] = sn;
    
    // json["apAddress"] = apAddress;
    // json["hostname"] = hostname;
    // json["stationmode"] = stationmode;
    // json["ssid"] = ssid;
    // json["ssids"] = ssids;
    // json["lora"] = lora;
    json["internetAvailable"] = daffodilData.internetAvailable;
    //json["internetPingTime"] = internetPingTime;
    json["ipAddress"] = daffodilData.ipAddress;
    //json["totp"] = totpcode;
    json["deviceTypeId"]=daffodilData.deviceTypeId;
    json["dsLastUpload"]=daffodilData.dsLastUpload;
    json["latitude"]=daffodilData.latitude;
     json["longitude"]=daffodilData.longitude;
  }

void DataManager::generateRosieWebData(RosieData &rosieData, JsonObject &json)
{
  
  json["devicename"] = rosieData.devicename;
  json["deviceshortname"] = rosieData.deviceshortname;
  json["flow1name"] = rosieData.flow1name;
  json["flow2name"] = rosieData.flow2name;
  json["tank1name"] = rosieData.tank1name;
  //json["tank2name"] = rosieData.tank2name;
  


  json["currentFunctionValue"] = rosieData.currentFunctionValue;
  json["flow1name"] = rosieData.flow1name;
  json["flow2name"] = rosieData.flow2name;
  json["tank1name"] = rosieData.tank1name;
  json["tank2name"] = rosieData.tank2name;
 // json["groupidentifier"] = rosieData.groupidentifier;
  json["secondsTime"] = rosieData.secondsTime;
  json["dataSamplingSec"] = rosieData.dataSamplingSec;
  json["currentFunctionValue"] = rosieData.currentFunctionValue;
  json["temperature"] = rosieData.temperature;
  json["rtcBatVolt"] = rosieData.rtcBatVolt;
  json["opMode"] = rosieData.opMode;
  json["rssi"] = rosieData.rssi;
  json["snr"] = rosieData.snr;
  
  json["flowrate"] = rosieData.flowRate;
  json["totalmilliLitres"] = rosieData.totalMilliLitres;
  json["flowrate2"] = rosieData.flowRate2;
  json["totalmilliLitres2"] = rosieData.totalMilliLitres2;
  json["tank1pressurePsi"] = rosieData.tank1PressurePsi;
 // json["tank1pressureVolts"] = rosieData.tank1PressureVolts;
//  json["tank1waterLevel"] = rosieData.tank1WaterLevel;
  json["tank1heightMeters"] = rosieData.tank1HeightMeters;
  json["tank2pressurePsi"] = rosieData.tank2PressurePsi;
  //json["tank2pressureVolts"] = rosieData.tank2PressureVolts;
 // json["tank2waterLevel"] = rosieData.tank2WaterLevel;
  json["tank2heightMeters"] = rosieData.tank2HeightMeters;
  json["qfactor1"] = rosieData.qfactor1;
  json["qfactor2"] = rosieData.qfactor2;
  json["rtcBatVolt"] = rosieData.rtcBatVolt;
  json["operatingStatus"] = rosieData.operatingStatus;
  json["sleepPingMinutes"] = rosieData.sleepPingMinutes;
  json["digitalStablesUpload"] = rosieData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = rosieData.secondsSinceLastPulse;
  json["serialnumber"] = sn;
   json["checksum"] = rosieData.checksum;
  json["sentBy"] = sn;
 
  // json["hostname"] = rosieData.hostname;
  // json["stationmode"] = rosieData.stationmode;
  // json["ssid"] = rosieData.ssid;
   json["loraActive"] = rosieData.loraActive;
   json["internetAvailable"] = rosieData.internetAvailable;
  // json["internetPingTime"] = rosieData.internetPingTime;
   json["ipAddress"] = rosieData.ipAddress;
  //json["totp"] = rosieData.totpcode;

  json["deviceTypeId"] = rosieData.deviceTypeId;
  json["dsLastUpload"] = rosieData.dsLastUpload;
  json["latitude"] = rosieData.latitude;
  json["longitude"] = rosieData.longitude;
  json["solarVoltage"] = rosieData.solarVoltage;
  json["capacitorVoltage"] = rosieData.capacitorVoltage;
}

void DataManager::generateGloriaTankFlowPumpWebData(GloriaTankFlowPumpData &gloriaTankFlowPumpData, JsonObject &json)
{
    json["devicename"] = gloriaTankFlowPumpData.devicename;
  json["deviceshortname"] = gloriaTankFlowPumpData.deviceshortname;
  json["flow1name"] = gloriaTankFlowPumpData.flow1name;
  json["flow2name"] = gloriaTankFlowPumpData.flow2name;
  json["tank1name"] = gloriaTankFlowPumpData.tank1name;
  json["tank2name"] = gloriaTankFlowPumpData.tank2name;
  json["currentFunctionValue"] = gloriaTankFlowPumpData.currentFunctionValue;
  json["flow1name"] = gloriaTankFlowPumpData.flow1name;
  json["flow2name"] = gloriaTankFlowPumpData.flow2name;
  json["tank1name"] = gloriaTankFlowPumpData.tank1name;
  json["tank2name"] = gloriaTankFlowPumpData.tank2name;
 // json["groupidentifier"] = gloriaTankFlowPumpData.groupidentifier;
  json["secondsTime"] = gloriaTankFlowPumpData.secondsTime;
  json["dataSamplingSec"] = gloriaTankFlowPumpData.dataSamplingSec;
  json["currentFunctionValue"] = gloriaTankFlowPumpData.currentFunctionValue;
  json["temperature"] = gloriaTankFlowPumpData.temperature;
  json["rtcBatVolt"] = gloriaTankFlowPumpData.rtcBatVolt;
  json["opMode"] = gloriaTankFlowPumpData.opMode;
  json["rssi"] = gloriaTankFlowPumpData.rssi;
  json["snr"] = gloriaTankFlowPumpData.snr;
  json["flowrate"] = gloriaTankFlowPumpData.flowRate;
  json["totalmilliLitres"] = gloriaTankFlowPumpData.totalMilliLitres;
  json["flowrate2"] = gloriaTankFlowPumpData.flowRate2;
  json["totalmilliLitres2"] = gloriaTankFlowPumpData.totalMilliLitres2;
  json["tank1pressurePsi"] = gloriaTankFlowPumpData.tank1PressurePsi;
  //json["tank1pressureVolts"] = gloriaTankFlowPumpData.tank1PressureVolts;
  //json["tank1waterLevel"] = gloriaTankFlowPumpData.tank1WaterLevel;
  json["tank1heightMeters"] = gloriaTankFlowPumpData.tank1HeightMeters;
  json["tank2pressurePsi"] = gloriaTankFlowPumpData.tank2PressurePsi;
  //json["tank2pressureVolts"] = gloriaTankFlowPumpData.tank2PressureVolts;
  //json["tank2waterLevel"] = gloriaTankFlowPumpData.tank2WaterLevel;
  json["tank2heightMeters"] = gloriaTankFlowPumpData.tank2HeightMeters;
  json["qfactor1"] = gloriaTankFlowPumpData.qfactor1;
  json["qfactor2"] = gloriaTankFlowPumpData.qfactor2;
  json["rtcBatVolt"] = gloriaTankFlowPumpData.rtcBatVolt;
  json["operatingStatus"] = gloriaTankFlowPumpData.operatingStatus;
  json["sleepPingMinutes"] = gloriaTankFlowPumpData.sleepPingMinutes;
  json["digitalStablesUpload"] = gloriaTankFlowPumpData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = gloriaTankFlowPumpData.secondsSinceLastPulse;
   json["checksum"] = gloriaTankFlowPumpData.checksum;
  json["serialnumber"] =sn;
  json["sentBy"] =sn;

  // json["soft_ap_ssid"] = gloriaTankFlowPumpData.soft_ap_ssid;
  // json["apAddress"] = gloriaTankFlowPumpData.apAddress;
  // json["hostname"] = gloriaTankFlowPumpData.hostname;
  // json["stationmode"] = gloriaTankFlowPumpData.stationmode;
  // json["ssid"] = gloriaTankFlowPumpData.ssid;
   json["loraActive"] = gloriaTankFlowPumpData.loraActive;
   json["internetAvailable"] = gloriaTankFlowPumpData.internetAvailable;
  // json["internetPingTime"] = gloriaTankFlowPumpData.internetPingTime;
   json["ipAddress"] = gloriaTankFlowPumpData.ipAddress;
  // json["totp"] = gloriaTankFlowPumpData.totpcode;
  json["deviceTypeId"] = gloriaTankFlowPumpData.deviceTypeId;
  json["dsLastUpload"] = gloriaTankFlowPumpData.dsLastUpload;
  json["latitude"] = gloriaTankFlowPumpData.latitude;
  json["longitude"] = gloriaTankFlowPumpData.longitude;
}

void DataManager::generatePanchoTankFlowDataWebData(PanchoTankFlowData &panchoTankFlowData, JsonObject &json)
{
      json["devicename"] = panchoTankFlowData.devicename;
  json["deviceshortname"] = panchoTankFlowData.deviceshortname;
  json["flow1name"] = panchoTankFlowData.flow1name;
  json["flow2name"] = panchoTankFlowData.flow2name;
  json["tank1name"] = panchoTankFlowData.tank1name;
  json["tank2name"] = panchoTankFlowData.tank2name;
  json["currentFunctionValue"] = panchoTankFlowData.currentFunctionValue;
  json["flow1name"] = panchoTankFlowData.flow1name;
  json["flow2name"] = panchoTankFlowData.flow2name;
  json["tank1name"] = panchoTankFlowData.tank1name;
  json["tank2name"] = panchoTankFlowData.tank2name;
  json["groupidentifier"] = panchoTankFlowData.groupidentifier;
  json["secondsTime"] = panchoTankFlowData.secondsTime;
  json["dataSamplingSec"] = panchoTankFlowData.dataSamplingSec;
  json["currentFunctionValue"] = panchoTankFlowData.currentFunctionValue;
  json["temperature"] = panchoTankFlowData.temperature;
  json["rtcBatVolt"] = panchoTankFlowData.rtcBatVolt;
  json["opMode"] = panchoTankFlowData.opMode;
  json["rssi"] = panchoTankFlowData.rssi;
  json["snr"] = panchoTankFlowData.snr;
  json["flowrate"] = panchoTankFlowData.flowRate;
  json["totalmilliLitres"] = panchoTankFlowData.totalMilliLitres;
  json["flowrate2"] = panchoTankFlowData.flowRate2;
  json["totalmilliLitres2"] = panchoTankFlowData.totalMilliLitres2;
  json["tank1pressurePsi"] = panchoTankFlowData.tank1PressurePsi;
  json["tank1pressureVolts"] = panchoTankFlowData.tank1PressureVolts;
  json["tank1waterLevel"] = panchoTankFlowData.tank1WaterLevel;
  json["tank1heightMeters"] = panchoTankFlowData.tank1HeightMeters;
  json["tank2pressurePsi"] = panchoTankFlowData.tank2PressurePsi;
  json["tank2pressureVolts"] = panchoTankFlowData.tank2PressureVolts;
  json["tank2waterLevel"] = panchoTankFlowData.tank2WaterLevel;
  json["tank2heightMeters"] = panchoTankFlowData.tank2HeightMeters;
  json["qfactor1"] = panchoTankFlowData.qfactor1;
  json["qfactor2"] = panchoTankFlowData.qfactor2;
  json["rtcBatVolt"] = panchoTankFlowData.rtcBatVolt;
  json["operatingStatus"] = panchoTankFlowData.operatingStatus;
  json["digitalStablesUpload"] = panchoTankFlowData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = panchoTankFlowData.secondsSinceLastPulse;
  json["checksum"] = panchoTankFlowData.checksum;
  json["serialnumber"] = sn;
 // json["sentBy"] = sn;
  // json["soft_ap_ssid"] = panchoTankFlowData.soft_ap_ssid;
  // json["apAddress"] = panchoTankFlowData.apAddress;
  // json["hostname"] = panchoTankFlowData.hostname;
  // json["stationmode"] = panchoTankFlowData.stationmode;
  // json["ssid"] = panchoTankFlowData.ssid;
   json["loraActive"] = panchoTankFlowData.loraActive;
   json["internetAvailable"] = panchoTankFlowData.internetAvailable;
  // json["internetPingTime"] = panchoTankFlowData.internetPingTime;
   json["ipAddress"] = panchoTankFlowData.ipAddress;
  // json["totp"] = panchoTankFlowData.totpcode;
  json["deviceTypeId"] = panchoTankFlowData.deviceTypeId;
  json["dsLastUpload"] = panchoTankFlowData.dsLastUpload;
  json["latitude"] = panchoTankFlowData.latitude;
  json["longitude"] = panchoTankFlowData.longitude;
}