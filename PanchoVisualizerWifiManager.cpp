#include <PanchoVisualizerWifiManager.h>
#include <ArduinoJson.h>

#include "SPIFFS.h"
 #define FUN_1_FLOW 1
#define FUN_2_FLOW 2
#define FUN_1_FLOW_1_TANK 3
#define FUN_1_TANK 4
#define FUN_2_TANK 5


PanchoVisualizerWifiManager::PanchoVisualizerWifiManager(HardwareSerial &serial,FS &fs, PCF8563TimeManager &t, Esp32SecretManager &e,  CajalData& tf,PanchoConfigData& p) :
WifiManager(serial , fs, t, e) , cajalData(tf),panchoConfigData(p){
}

 

void PanchoVisualizerWifiManager::start(){

	if (!SPIFFS.begin(true)) {
		// SPIFFS will be configured on reboot
		//_HardSerial.println("ERROR: Cannot mount SPIFFS, Rebooting");
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
    //_HardSerial.print("in  PanchoVisualizerWifiManager ssid=");
    //_HardSerial.print(ssid);
    //_HardSerial.print(" stationmode=");
    //_HardSerial.println(stationmode);
    //_HardSerial.println("about to do start scan");
    ssids = scanNetworks();
      //
      //set the mode to null so that 
      // the hostname is applied
      //  https://github.com/espressif/arduino-esp32/issues/6700
      WiFi.mode(WIFI_MODE_NULL);
    if(stationmode && ssid){
       bool gotConnection = connectSTA();
       if(!gotConnection){
          //_HardSerial.print("Coult not connect to ");
          //_HardSerial.println(ssid);
		      bool apModeActive = connectAP();
       }
    }else{
  	  
      //_HardSerial.print("ssids=");
      //_HardSerial.println(ssids);

		  //_HardSerial.println("about to do connectAP");
        bool apModeActive = connectAP();
    }
    
    

asyncWebServer.on("/assets/bootstrap/css/bootstrap.min.css", HTTP_GET, [this](AsyncWebServerRequest *request){
 // this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/bootstrap.min.css", String(), false);
  });

asyncWebServer.on("/assets/img/Pancho.svg", HTTP_GET, [this](AsyncWebServerRequest *request){
 // this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/Pancho.svg", String(), false);
  });

asyncWebServer.on("/assets/js/TankAndFlowConstants.js", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/TankAndFlowConstants.js", String(), false);
  });

asyncWebServer.on("/assets/js/jquery.min.js", HTTP_GET, [this](AsyncWebServerRequest *request){
 // this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/jquery.min.js", String(), false);
  });

asyncWebServer.on("/assets/css/slideswitch.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  slideswitch/=");
    request->send(SPIFFS, "/slideswitch.css", String(), false);
    delay(5);
  });



asyncWebServer.on("/assets/css/styles.css", HTTP_GET, [this](AsyncWebServerRequest *request){
 // this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/styles.css", String(), false);
  });

asyncWebServer.on("/assets/fonts/fa-solid-900.woff2", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/fa-solid-900.woff2", String(), false);
  });

asyncWebServer.on("/assets/fonts/fa-solid-900.woff", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/fa-solid-900.woff", String(), false);
  });
  
  asyncWebServer.on("/assets/fonts/fa-solid-900.ttf", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/fa-solid-900.ttf", String(), false);
  });


asyncWebServer.on("/assets/fonts/fontawesome-all.min.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/fontawesome-all.min.css", String(), false);
  });

asyncWebServer.on("/assets/bootstrap/js/bootstrap.min.js", HTTP_GET, [this](AsyncWebServerRequest *request){
 // this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/bootstrap.min.js", String(), false);
  });

asyncWebServer.on("/js/index.js", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request  /=");
    request->send(SPIFFS, "/index.js", String(), false);
  });
  
asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
	//this->_HardSerial.println("reqestubg /=");
    request->send(SPIFFS, "/index.html", String(), false);
  });

  asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request){
	//this->_HardSerial.println("reqestubg index.html=");
    request->send(SPIFFS, "/index.html", String(), false);
  });
asyncWebServer.on("/assets/css/Roboto.css", HTTP_GET, [this](AsyncWebServerRequest *request){
	//this->_HardSerial.println("reqestubg robottocss=");
    request->send(SPIFFS, "/Roboto.css", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/fonts/Roboto-Regular.woff", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/Roboto-Regular.woff", String(), false);
    delay(5);
  });

  asyncWebServer.on("/assets/fonts/Roboto-Regular.woff2", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/Roboto-Regular.woff2", String(), false);
    delay(5);
  });


   asyncWebServer.on("/GetHostName", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request solid woof900 /=");
     request->send_P(200, "text/plain", hostname.c_str()); 
    delay(5);
  });
  

asyncWebServer.on("/PanchoTankAndFlowServlet", HTTP_POST, [this](AsyncWebServerRequest *request) {
    int paramsNr = request->params();
    this->_HardSerial.println(paramsNr);
   
    AsyncWebParameter* p = request->getParam(0);
    //String formName =p->name;
    String formName =p->value();    
    this->_HardSerial.print("in post, formName=");
    this->_HardSerial.println(formName);
    AsyncResponseStream *response = request->beginResponseStream("text/plain");

    if(formName=="ConfigSTA"){

      p = request->getParam(1);
      String ssid =p->value();    
  
      p = request->getParam(2);
      String password =p->value();  

      p = request->getParam(3);
      String hostName =p->value();  

      this->_HardSerial.print("in servet2, about to set to station=");
     
      bool staok = this->configWifiSTA(ssid, password, hostName);
      if (staok) {
        this->_HardSerial.print("rebooting ");
        ESP.restart();
      } else {
        this->_HardSerial.print("failed change to station");
     
      }
    }else  if(formName=="ConfigAP"){

      p = request->getParam(1);
      String apaddress =p->value();    
  
      p = request->getParam(2);
      String password =p->value();  

      p = request->getParam(3);
      String hostName =p->value();  

      this->_HardSerial.print("in servet2, about to set to access point,h=");
      this->_HardSerial.print(hostName);
      this->_HardSerial.print(" apaddress=");
      this->_HardSerial.println(apaddress);

     
      bool staok = this->configWifiAP(apaddress, password, hostName);
      if (staok) {
        this->_HardSerial.print("rebooting after setting access point ");
        ESP.restart();
      } else {
        this->_HardSerial.print("failed change to ap");
     
      }
    }else  if(formName=="SetSensor1Param"){
      p = request->getParam(1);
      String flow1Name =p->value();    
  
      p = request->getParam(2);
      String qfactor1 =p->value();  

    }else  if(formName=="SetTime"){
      p = request->getParam(1);
      String time =p->value();    
      bool r = timeManager.setTime(time);
      if(r){
        String ok = "Ok";
         request->send_P(200, "text/plain", ok.c_str()); 
      }else{
        String fail="Fail";
        request->send_P(200, "text/plain", fail.c_str()); 
      }

    }
});

asyncWebServer.on("/PanchoTankAndFlowServlet", HTTP_GET, [this](AsyncWebServerRequest *request) {
 int paramsNr = request->params();
    this->_HardSerial.println(paramsNr);
    AsyncWebParameter* p = request->getParam(0);
    //String formName =p->name;
    String formName =p->value();    
    this->_HardSerial.print("in servet, formName=");
    this->_HardSerial.println(formName);
    AsyncResponseStream *response = request->beginResponseStream("text/plain");
    if(formName=="GetWebData"){
		  DynamicJsonDocument json(1800);
      this->generateWebData(json, serialNumber);
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
 
 int PanchoVisualizerWifiManager::uploadRosieDataToDigitalStables(RosieData& rosieData){

  DynamicJsonDocument json(1800);
  generateRosieWebData(rosieData,json, serialNumber);
  
  String output;
  serializeJson(json, output);
  const char* serverName = "http://devices.digitalstables.com/DeviceUploadServlet";
  http.begin(serverName);    
  http.addHeader("Content-Type", "application/json");
  boolean toReturn=false;
  int httpResponseCode = http.POST(output);
  //_HardSerial.print("upload digitalstables return ");
  //_HardSerial.println(httpResponseCode);
   cajalData.digitalStablesUpload=false;
  if (httpResponseCode == 200) { //Check for the returning code
      String response = http.getString();  //Get the response to the request
      if(response=="Ok"){
        toReturn =true;
         cajalData.digitalStablesUpload=true;
      }else{
        httpResponseCode=500;
      }
  }
  http.end();
  return httpResponseCode;//toReturn;
}

 void PanchoVisualizerWifiManager::generateRosieWebData(RosieData& rosieData,DynamicJsonDocument& json, String sentBy){
    json["currentFunctionValue"]= rosieData.currentFunctionValue;
    json["flow1name"] = rosieData.flow1name;
    json["flow2name"] = rosieData.flow2name;
    json["tank1name"] = rosieData.tank1name;
    json["tank2name"] = rosieData.tank2name;
  //  json["groupidentifier"]=rosieData.groupidentifier;
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
  //  json["tank1pressureVolts"] = rosieData.tank1PressureVolts;
  //  json["tank1waterLevel"] = rosieData.tank1WaterLevel;
    json["tank1heightMeters"] = rosieData.tank1HeightMeters;
    json["tank2pressurePsi"] = rosieData.tank2PressurePsi;
 //   json["tank2pressureVolts"] = rosieData.tank2PressureVolts;
  //  json["tank2waterLevel"] = rosieData.tank2WaterLevel;
    json["tank2heightMeters"] = rosieData.tank2HeightMeters;
    json["qfactor1"] = rosieData.qfactor1;
    json["qfactor2"] = rosieData.qfactor2;
    json["rtcBatVolt"] = rosieData.rtcBatVolt;        
    json["operatingStatus"] = rosieData.operatingStatus;
    json["sleepPingMinutes"] = rosieData.sleepPingMinutes;
    json["digitalStablesUpload"] = rosieData.digitalStablesUpload;
    json["secondsSinceLastPulse"] = rosieData.secondsSinceLastPulse;
    json["soft_ap_ssid"] = soft_ap_ssid;
    json["serialnumber"] = serialNumber;
    json["sentBy"] = sentBy;
    
    json["apAddress"] = apAddress;
    json["hostname"] = hostname;
    json["stationmode"] = stationmode;
    json["ssid"] = ssid;
    json["ssids"] = ssids;
    json["lora"] = lora;
    json["internetAvailable"] = internetAvailable;
    json["internetPingTime"] = internetPingTime;
    json["ipAddress"] = ipAddress;
    json["totp"] = totpcode;
    json["deviceTypeId"]=rosieData.deviceTypeId;
    json["dsLastUpload"]=rosieData.dsLastUpload;
    json["latitude"]=rosieData.latitude;
     json["longitude"]=rosieData.longitude;
  }

void PanchoVisualizerWifiManager::generateWebData(DynamicJsonDocument& json, String sentBy){

		json["soft_ap_ssid"] = this->soft_ap_ssid;
		json["serialnumber"] = this->serialNumber;
    json["apAddress"] = apAddress;
    json["hostname"] = hostname;
    json["stationmode"] = stationmode;
    json["ssid"] = ssid;
    json["groupidentifier"]=cajalData.groupidentifier;
    json["secondsTime"] = cajalData.secondsTime;
    json["ssids"] = ssids;
    json["lora"] = lora;
    json["internetAvailable"] = internetAvailable;
    json["internetPingTime"] = internetPingTime;
    json["ipAddress"] = ipAddress;
    json["totp"] = totpcode;
    json["deviceTypeId"]=cajalData.deviceTypeId;
    json["dsLastUpload"]=cajalData.dsLastUpload;
    json["latitude"]=cajalData.latitude;
    json["longitude"]=cajalData.longitude;
}   

int PanchoVisualizerWifiManager::uploadDataToDigitalStables(){

  DynamicJsonDocument json(1800);
  generateWebData(json, serialNumber);
  
  String output;
  serializeJson(json, output);
  const char* serverName = "http://devices.digitalstables.com/DeviceUploadServlet";
  http.begin(serverName);    
  http.addHeader("Content-Type", "application/json");
  boolean toReturn=false;
  int httpResponseCode = http.POST(output);
  //_HardSerial.print("upload digitalstables return ");
  //_HardSerial.println(httpResponseCode);
   cajalData.digitalStablesUpload=false;
  if (httpResponseCode == 200) { //Check for the returning code
      String response = http.getString();  //Get the response to the request
      if(response=="Ok"){
        toReturn =true;
         cajalData.digitalStablesUpload=true;
      }else{
        httpResponseCode=500;
      }
  }
  http.end();
  return httpResponseCode;//toReturn;
}


PanchoVisualizerWifiManager::~PanchoVisualizerWifiManager() {}