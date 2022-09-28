#include <PanchoVisualizerWifiManager.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
 #define FUN_1_FLOW 1
#define FUN_2_FLOW 2
#define FUN_1_FLOW_1_TANK 3
#define FUN_1_TANK 4
#define FUN_2_TANK 5


PanchoVisualizerWifiManager::PanchoVisualizerWifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e,  PanchoTankFlowData& tf,PanchoConfigData& p) :
WifiManager(serial ,  t, e,  tf,  p) {}

 

void PanchoVisualizerWifiManager::start(){

	if (!SPIFFS.begin(true)) {
		// SPIFFS will be configured on reboot
		_HardSerial.println("ERROR: Cannot mount SPIFFS, Rebooting");
		delay(1000);

	}
   //
   // get the parameters
   //
   ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();;
    soft_ap_ssid = secretManager.getSoftAPSSID();
    soft_ap_password = secretManager.getSoftAPPASS();
    hostname=secretManager.getHostName();
    stationmode = secretManager.getStationMode();
    _HardSerial.print("in  PanchoVisualizerWifiManager ssid=");
    _HardSerial.print(ssid);
    _HardSerial.print(" stationmode=");
    _HardSerial.println(stationmode);
    
    if(stationmode){
       connectSTA();
       
    }else{
		_HardSerial.println("about to do connectAP");
         connectAP();
    }
    

asyncWebServer.on("/assets/bootstrap/css/bootstrap.min.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/bootstrap.min.css", String(), false);
  });

asyncWebServer.on("/assets/img/Pancho.svg", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/Pancho.svg", String(), false);
  });

asyncWebServer.on("/assets/js/TankAndFlowConstants.js", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/TankAndFlowConstants.js", String(), false);
  });

asyncWebServer.on("/assets/js/jquery.min.js", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/jquery.min.js", String(), false);
  });

asyncWebServer.on("/assets/css/slideswitch.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/fontawesome-all.min.css", String(), false);
  });


asyncWebServer.on("/assets/css/styles.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/styles.css", String(), false);
  });

asyncWebServer.on("/assets/fonts/fa-solid-900.woff2", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/fa-solid-900.woff2", String(), false);
  });

asyncWebServer.on("/assets/fonts/fa-solid-900.woff", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/fa-solid-900.woff", String(), false);
  });
  
  asyncWebServer.on("/assets/fonts/fa-solid-900.ttf", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/fa-solid-900.ttf", String(), false);
  });


asyncWebServer.on("/assets/fonts/fontawesome-all.min.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/fontawesome-all.min.css", String(), false);
  });

asyncWebServer.on("/assets/bootstrap/js/bootstrap.min.js", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/bootstrap.min.js", String(), false);
  });

asyncWebServer.on("/js/index.js", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/index.js", String(), false);
  });
  
asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
	this->_HardSerial.println("reqestubg /=");
    request->send(SPIFFS, "/index.html", String(), false);
  });

  asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request){
	this->_HardSerial.println("reqestubg index.html=");
    request->send(SPIFFS, "/index.html", String(), false);
  });





asyncWebServer.on("/PanchoTankAndFlowServlet", HTTP_GET, [this](AsyncWebServerRequest *request) {
 int paramsNr = request->params();
    this->_HardSerial.println(paramsNr);
   
    AsyncWebParameter* p = request->getParam(0);
    //String formName =p->name;
    String formName =p->value();    
  this->_HardSerial.print("in servet, formName=");
  this->_HardSerial.println(formName);
  
    if(formName=="GetWebData"){
		//AsyncResponseStream *response = request->beginResponseStream("application/json");
		AsyncResponseStream *response = request->beginResponseStream("text/plain");
		int paramsNr = request->params();
		this->_HardSerial.println(paramsNr);
	
		AsyncWebParameter* p = request->getParam(0);
	    //String formName =p->name;
    	String formName =p->value();    
		this->_HardSerial.print("in servet, formName=");
		this->_HardSerial.println(formName);
			
		DynamicJsonDocument json(1024);
		json["currentFunctionValue"]= FUN_1_FLOW_1_TANK;
		json["flow1name"] = this->panchoTankFlowData.flow1name;
		json["flow2name"] = this->panchoTankFlowData.flow2name;
		json["tank1name"] = this->panchoTankFlowData.tank1name;
		json["tank2name"] = this->panchoTankFlowData.tank2name;
		json["secondsTime"] = this->panchoTankFlowData.secondsTime;
		json["dataSamplingSec"] = this->panchoTankFlowData.dataSamplingSec;
		json["currentFunctionValue"] = this->panchoTankFlowData.currentFunctionValue;
		json["temperature"] = this->panchoTankFlowData.temperature;
		json["reg33Voltage"] = this->panchoTankFlowData.reg33Voltage;
		json["rtcBatVolt"] = this->panchoTankFlowData.rtcBatVolt;
		json["opMode"] = this->panchoTankFlowData.opMode;
		json["rssi"] = this->panchoTankFlowData.rssi;
		json["snr"] = this->panchoTankFlowData.snr;
		json["flowRate"] = this->panchoTankFlowData.flowRate;
		json["totalMilliLitres"] = this->panchoTankFlowData.totalMilliLitres;
		json["flowRate2"] = this->panchoTankFlowData.flowRate2;
		json["totalMilliLitres2"] = this->panchoTankFlowData.totalMilliLitres2;
		json["tankPressurePsi"] = this->panchoTankFlowData.tankPressurePsi;
		json["tankPressureVolts"] = this->panchoTankFlowData.tankPressureVolts;
		json["tankWaterLevel"] = this->panchoTankFlowData.tankWaterLevel;
		json["tankHeightMeters"] = this->panchoTankFlowData.tankHeightMeters;
		json["tank2PressurePsi"] = this->panchoTankFlowData.tank2PressurePsi;
		json["tank2PressureVolts"] = this->panchoTankFlowData.tank2PressureVolts;
		json["tank2WaterLevel"] = this->panchoTankFlowData.tank2WaterLevel;
		json["tank2HeightMeters"] = this->panchoTankFlowData.tank2HeightMeters;
		json["qfactor1"] = this->panchoTankFlowData.qfactor1;
		json["qfactor2"] = this->panchoTankFlowData.qfactor2;
		json["operatingStatus"] = this->panchoTankFlowData.operatingStatus;
		json["sleepPingMinutes"] = this->panchoTankFlowData.sleepPingMinutes;
		json["secondsSinceLastPulse"] = this->panchoTankFlowData.secondsSinceLastPulse;
		json["soft_ap_ssid"] = this->soft_ap_ssid;
		json["serialNumber"] = this->serialNumber;
		json["apAddress"] = this->apAddress;
		json["hostname"] = this->hostname;
		json["stationmode"] = this->stationmode;
		json["ssid"] =this-> ssid;
		json["ipAddress"] = this->ipAddress;
		serializeJson(json, *response);
		request->send(response);
	}
  });

 asyncWebServer.on("/GetSensorData", HTTP_GET, [this](AsyncWebServerRequest *request){
       this->_HardSerial.println("curl request returning");
        this->_HardSerial.println(sensorString);
        request->send_P(200, "text/plain", sensorString.c_str()); 
    });

asyncWebServer.begin();
    
    

}

PanchoVisualizerWifiManager::~PanchoVisualizerWifiManager() {}