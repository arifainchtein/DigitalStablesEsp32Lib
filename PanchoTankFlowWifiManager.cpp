#include <PanchoTankFlowWifiManager.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
 #define FUN_1_FLOW 1
#define FUN_2_FLOW 2
#define FUN_1_FLOW_1_TANK 3
#define FUN_1_TANK 4
#define FUN_2_TANK 5

 String okString="Ok";

PanchoTankFlowWifiManager::PanchoTankFlowWifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e,  PanchoTankFlowData& tf,PanchoConfigData& p) :
WifiManager(serial ,  t, e) {
     panchoTankFlowData(tf);
    panchoConfigData(p);

}

 

void PanchoTankFlowWifiManager::start(){

	if (!SPIFFS.begin(true)) {
		// SPIFFS will be configured on reboot
		_HardSerial.println("ERROR: Cannot mount SPIFFS, Rebooting");
		delay(1000);

	}

// File root = SPIFFS.open("/") ;
// File file=root.openNextFile();
// while(file){
//   _HardSerial.println(file.name());
//   file=root.openNextFile();
// }
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
    
    _HardSerial.print("in  PanchoVisualizerWifiManager ssid=");
    _HardSerial.print(ssid);
    _HardSerial.print(" stationmode=");
    _HardSerial.println(stationmode);
    _HardSerial.println("about to do start scan");
    ssids = scanNetworks();
      //
      //set the mode to null so that 
      // the hostname is applied
      //  https://github.com/espressif/arduino-esp32/issues/6700
      WiFi.mode(WIFI_MODE_NULL);
    if(stationmode && ssid){
       connectSTA();
       
    }else{
  	  
      _HardSerial.print("ssids=");
      _HardSerial.println(ssids);

		  _HardSerial.println("about to do connectAP");
         connectAP();
    }


    asyncWebServer.on("/SetQFactor1", HTTP_GET, [this](AsyncWebServerRequest *request){
        int numberOfParameters = request->params();
        AsyncWebParameter* p = request->getParam(0);
        panchoTankFlowData.qfactor1 = p->value().toFloat();
       
        request->send_P(200, "text/plain", okString.c_str()); 
    });



asyncWebServer.on("/assets/bootstrap/css/bootstrap.min.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request  bootstrap.min.css=");
    request->send(SPIFFS, "/bootstrap.min.css", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/img/Pancho.svg", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request  Pancho.svg");
    request->send(SPIFFS, "/Pancho.svg", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/js/TankAndFlowConstants.js", HTTP_GET, [this](AsyncWebServerRequest *request){
 // this->_HardSerial.println("request TankFlowConstants.js");
    request->send(SPIFFS, "/TankAndFlowConstants.js", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/js/jquery.min.js", HTTP_GET, [this](AsyncWebServerRequest *request){
 // this->_HardSerial.println("request  jquery/=");
    request->send(SPIFFS, "/jquery.min.js", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/css/slideswitch.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request  slideswitch/=");
    request->send(SPIFFS, "/slideswitch.css", String(), false);
    delay(5);
  });


asyncWebServer.on("/assets/css/styles.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request styles.css /=");
    request->send(SPIFFS, "/styles.css", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/fonts/fa-solid-900.woff2", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request solid woof2 /=");
    request->send(SPIFFS, "/fa-solid-900.woff2", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/fonts/fa-solid-900.woff", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/fa-solid-900.woff", String(), false);
    delay(5);
  });
  
  asyncWebServer.on("/assets/fonts/fa-solid-900.ttf", HTTP_GET, [this](AsyncWebServerRequest *request){
  //this->_HardSerial.println("request  solid woof ttf /=");
    request->send(SPIFFS, "/fa-solid-900.ttf", String(), false);
    delay(5);
  });


asyncWebServer.on("/assets/fonts/fontawesome-all.min.css", HTTP_GET, [this](AsyncWebServerRequest *request){
 // this->_HardSerial.println("request fontawesome /=");
    request->send(SPIFFS, "/fontawesome-all.min.css", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/bootstrap/js/bootstrap.min.js", HTTP_GET, [this](AsyncWebServerRequest *request){
//  this->_HardSerial.println("request  bootstramo min js /=");
    request->send(SPIFFS, "/bootstrap.min.js", String(), false);
    delay(5);
  });

asyncWebServer.on("/js/index.js", HTTP_GET, [this](AsyncWebServerRequest *request){
 // delay(5);
 // this->_HardSerial.println("request  indexjs /=");
  delay(5);
    request->send(SPIFFS, "/index.js", String(), false);
    delay(5);
  });
  
asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
	//this->_HardSerial.println("reqestubg  root =");
    request->send(SPIFFS, "/index.html", String(), false);
    delay(5);
  });

  asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request){
	//this->_HardSerial.println("reqestubg index.html=");
    request->send(SPIFFS, "/index.html", String(), false);
    delay(5);
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

    }
});


asyncWebServer.on("/PanchoTankAndFlowServlet", HTTP_GET, [this](AsyncWebServerRequest *request) {
 int paramsNr = request->params();
    this->_HardSerial.println(paramsNr);
   
    AsyncWebParameter* p = request->getParam(0);
    //String formName =p->name;
    String formName =p->value();    
  this->_HardSerial.print("in servet2, formName=");
  this->_HardSerial.println(formName);
   AsyncResponseStream *response = request->beginResponseStream("text/plain");
    if(formName=="GetWebData"){
        //AsyncResponseStream *response = request->beginResponseStream("application/json");
       
        
          
        DynamicJsonDocument json(1800);
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
        json["flowrate"] = this->panchoTankFlowData.flowRate;
        json["totalmilliLitres"] = this->panchoTankFlowData.totalMilliLitres;
        json["flowrate2"] = this->panchoTankFlowData.flowRate2;
        json["totalmilliLitres2"] = this->panchoTankFlowData.totalMilliLitres2;
        json["tankpressurePsi"] = this->panchoTankFlowData.tankPressurePsi;
        json["tankpressureVolts"] = this->panchoTankFlowData.tankPressureVolts;
        json["tankwaterLevel"] = this->panchoTankFlowData.tankWaterLevel;
        json["tankheightMeters"] = this->panchoTankFlowData.tankHeightMeters;
        json["tank2pressurePsi"] = this->panchoTankFlowData.tank2PressurePsi;
        json["tank2pressureVolts"] = this->panchoTankFlowData.tank2PressureVolts;
        json["tank2waterLevel"] = this->panchoTankFlowData.tank2WaterLevel;
        json["tank2heightMeters"] = this->panchoTankFlowData.tank2HeightMeters;
        json["qfactor1"] = this->panchoTankFlowData.qfactor1;
        json["qfactor2"] = this->panchoTankFlowData.qfactor2;
        json["rtcBatVolt"] = 2.9;//this->panchoTankFlowData.qfactor2;
        
        json["operatingStatus"] = this->panchoTankFlowData.operatingStatus;
        
        
        json["sleepPingMinutes"] = this->panchoTankFlowData.sleepPingMinutes;
        json["secondsSinceLastPulse"] = this->panchoTankFlowData.secondsSinceLastPulse;
        json["soft_ap_ssid"] = this->soft_ap_ssid;
        json["serialnumber"] = this->serialNumber;
        json["apAddress"] = this->apAddress;
        json["hostname"] = this->hostname;
        json["stationmode"] = this->stationmode;
        json["ssid"] =this-> ssid;
        json["ssids"] =this-> ssids;
        json["lora"] =this-> lora;
        
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

PanchoTankFlowWifiManager::~PanchoTankFlowWifiManager() {}