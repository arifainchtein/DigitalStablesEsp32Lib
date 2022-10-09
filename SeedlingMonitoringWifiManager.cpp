#include <SeedlingMonitoringWifiManager.h>


 
SeedlingMonitoringWifiManager::SeedlingMonitoringWifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e,  PanchoTankFlowData& tf,PanchoConfigData& p) :
WifiManager(serial ,  t, e) {}

 
void SeedlingMonitoringWifiManager::prepareData(DynamicJsonDocument& json){
    json["seedlingtemp"] = panchoTankFlowData.flowRate;
        json["roomtemp"] = panchoTankFlowData.flowRate2;
        
        json["secondsSinceLastPulse"] = panchoTankFlowData.secondsSinceLastPulse;
        json["soft_ap_ssid"] = soft_ap_ssid;
        json["serialNumber"] = serialNumber;
        json["apAddress"] =apAddress;
        json["hostname"] = hostname;
        json["stationmode"] = stationmode;
        json["ssid"] = ssid;
        json["ipAddress"] = ipAddress;
        seedlingMonitorData
}

void SeedlingMonitoringWifiManager::start(){
   //
   // get the parameters
   //
   ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();;
    soft_ap_ssid = secretManager.getSoftAPSSID();
    soft_ap_password = secretManager.getSoftAPPASS();
    hostname=secretManager.getHostName();
    stationmode = secretManager.getStationMode();
    _HardSerial.println("ssid=");
    _HardSerial.println(ssid);
    _HardSerial.println("stationmode=");
    _HardSerial.println(stationmode);
    
    if(stationmode){
       connectSTA();
       asyncWebServer.begin();
    }else{
         //  mark as not connected
    }
    



asyncWebServer.on("/GetWebData", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);
    prepareData(json);
	// json["seedlingtemp"] = this->panchoTankFlowData.flow1name;
	// json["roomtemp"] = this->panchoTankFlowData.flow2name;
	
    // json["secondsSinceLastPulse"] = this->panchoTankFlowData.secondsSinceLastPulse;
    // json["soft_ap_ssid"] = this->soft_ap_ssid;
    // json["serialNumber"] = this->serialNumber;
    // json["apAddress"] = this->apAddress;
    // json["hostname"] = this->hostname;
    // json["stationmode"] = this->stationmode;
    // json["ssid"] =this-> ssid;
    // json["ipAddress"] = this->ipAddress;
    serializeJson(json, *response);
    request->send(response);
  });

 asyncWebServer.on("/GetSensorData", HTTP_GET, [this](AsyncWebServerRequest *request){
       this->_HardSerial.println("curl request returning");
        this->_HardSerial.println(sensorString);
        request->send_P(200, "text/plain", sensorString.c_str()); 
    });

asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument json(1024);
        prepareData(json);
        // json["seedlingtemp"] = this->panchoTankFlowData.flowRate;
        // json["roomtemp"] = this->panchoTankFlowData.flowRate2;
        
        // json["secondsSinceLastPulse"] = this->panchoTankFlowData.secondsSinceLastPulse;
        // json["soft_ap_ssid"] = this->soft_ap_ssid;
        // json["serialNumber"] = this->serialNumber;
        // json["apAddress"] = this->apAddress;
        // json["hostname"] = this->hostname;
        // json["stationmode"] = this->stationmode;
        // json["ssid"] =this-> ssid;
        // json["ipAddress"] = this->ipAddress;
        serializeJson(json, *response);
        request->send(response);
         });
    asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request){
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument json(1024);
        prepareData(json);
        // json["seedlingtemp"] = this->panchoTankFlowData.flow1name;
        // json["roomtemp"] = this->panchoTankFlowData.flow2name;
        
        // json["secondsSinceLastPulse"] = this->panchoTankFlowData.secondsSinceLastPulse;
        // json["soft_ap_ssid"] = this->soft_ap_ssid;
        // json["serialNumber"] = this->serialNumber;
        // json["apAddress"] = this->apAddress;
        // json["hostname"] = this->hostname;
        // json["stationmode"] = this->stationmode;
        // json["ssid"] =this-> ssid;
        // json["ipAddress"] = this->ipAddress;
        serializeJson(json, *response);
        request->send(response);
         });

}

SeedlingMonitoringWifiManager::~SeedlingMonitoringWifiManager() {}