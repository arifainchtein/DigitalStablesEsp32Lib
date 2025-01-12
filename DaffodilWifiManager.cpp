#include <DaffodilWifiManager.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
 #define FUN_1_FLOW 1
#define FUN_2_FLOW 2
#define FUN_1_FLOW_1_TANK 3
#define FUN_1_TANK 4
#define FUN_2_TANK 5
#define WATCHDOG_WDI 18


DaffodilWifiManager::DaffodilWifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e,  DigitalStablesData& tf,DigitalStablesConfigData& p) : WifiManager(serial ,  t,e) , digitalStablesData(tf),digitalStablesConfigData(p){}

 void DaffodilWifiManager::setWifiActiveSwitchStatus(bool b){
    wifiActiveSwitchStatus=b;
 }

void DaffodilWifiManager::start(){

	if (!SPIFFS.begin(true)) {
		// SPIFFS will be configured on reboot
		_HardSerial.println("ERROR: Cannot mount SPIFFS, Rebooting");
	//	delay(1000);

	}

// File root = SPIFFS.open("/") ;
// File file=root.openNextFile();
// while(file){
//   _HardSerial.println(file.name());
//   file=root.openNextFile();
// }
   //getCurrentTimeInSeconds
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
    
    _HardSerial.print("in  DaffodilWifiManager ssid=");
    _HardSerial.print(ssid);
    _HardSerial.print(" stationmode=");
    _HardSerial.println(stationmode);
    //_HardSerial.println("about to do start scan");
     digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
     ssids = scanNetworks();
      digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    //   //
    //   //set the mode to null so that 
    //   // the hostname is applied
    //   //  https://github.com/espressif/arduino-esp32/issues/6700
       WiFi.mode(WIFI_MODE_NULL);
    if(   stationmode && ssid){
       connectSTA();
       
    }else{
  	  
      _HardSerial.print("ssids=");
      _HardSerial.println(ssids);

		  _HardSerial.println("about to do connectAP");
         connectAP();
    }



  asyncWebServer.on("/assets/bootstrap/css/bootstrap.min.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  bootstrap.min.css=");
    request->send(SPIFFS, "/bootstrap.min.css", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/img/Daffodil.svg", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  Daffodil.svg");
    request->send(SPIFFS, "/Daffodil.svg", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/js/TankAndFlowConstants.js", HTTP_GET, [this](AsyncWebServerRequest *request){
 this->_HardSerial.println("request TankFlowConstants.js");
    request->send(SPIFFS, "/TankAndFlowConstants.js", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/js/jquery.min.js", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  jquery/=");
    request->send(SPIFFS, "/jquery.min.js", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/css/slideswitch.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  slideswitch/=");
    request->send(SPIFFS, "/slideswitch.css", String(), false);
    delay(5);
  });


asyncWebServer.on("/assets/css/styles.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request styles.css /=");
    request->send(SPIFFS, "/styles.css", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/fonts/fa-solid-900.woff2", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request solid woof2 /=");
    request->send(SPIFFS, "/fa-solid-900.woff2", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/fonts/fa-solid-900.woff", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/fa-solid-900.woff", String(), false);
    delay(5);
  });
  
  asyncWebServer.on("/assets/fonts/fa-solid-900.ttf", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request  solid woof ttf /=");
    request->send(SPIFFS, "/fa-solid-900.ttf", String(), false);
    delay(5);
  });


asyncWebServer.on("/assets/fonts/fontawesome-all.min.css", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request fontawesome /=");
    request->send(SPIFFS, "/fontawesome-all.min.css", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/bootstrap/js/bootstrap.min.js", HTTP_GET, [this](AsyncWebServerRequest *request){
 this->_HardSerial.println("request  bootstramo min js /=");
    request->send(SPIFFS, "/bootstrap.min.js", String(), false);
    delay(5);
  });

asyncWebServer.on("/js/index.js", HTTP_GET, [this](AsyncWebServerRequest *request){
  delay(5);
 // this->_HardSerial.println("request  indexjs /=");
  delay(5);
    request->send(SPIFFS, "/index.js", String(), false);
    delay(5);
  });
  
asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
	this->_HardSerial.println("reqestubg  root =");
    request->send(SPIFFS, "/index.html", String(), false);
    delay(5);
  });

  asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request){
	this->_HardSerial.println("reqestubg index.html=");
    request->send(SPIFFS, "/index.html", String(), false);
    delay(5);
  });

   asyncWebServer.on("/assets/css/Roboto.css", HTTP_GET, [this](AsyncWebServerRequest *request){
	this->_HardSerial.println("reqestubg robottocss=");
    request->send(SPIFFS, "/Roboto.css", String(), false);
    delay(5);
  });

asyncWebServer.on("/assets/fonts/Roboto-Regular.woff", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/Roboto-Regular.woff", String(), false);
    delay(5);
  });

  asyncWebServer.on("/assets/fonts/Roboto-Regular.woff2", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/Roboto-Regular.woff2", String(), false);
    delay(5);
  });
  

   asyncWebServer.on("/GetHostName", HTTP_GET, [this](AsyncWebServerRequest *request){
  this->_HardSerial.println("request solid woof900 /=");
     request->send_P(200, "text/plain", hostname.c_str()); 
    delay(5);
  });
  
  

asyncWebServer.on("/DaffodilServlet", HTTP_POST, [this](AsyncWebServerRequest *request) {
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

    }else if(formName=="SetTimeViaInternet"){
      bool r = setTimeFromInternet();
        DynamicJsonDocument json(1800);
       this->generateWebData(json,serialNumber);
        serializeJson(json, *response);
        request->send(response);

    }else if(formName=="ManualSetTime"){
       p = request->getParam(1);
      String timeString =p->value();
      this->_HardSerial.print("Settg time manually:");
      this->_HardSerial.println(timeString);
      timeManager.setTime(timeString);
       DynamicJsonDocument json(1800);
       this->generateWebData(json, serialNumber);
       serializeJson(json, *response);
        request->send(response);
    }else if(formName=="SetGPS"){
      p = request->getParam(1);
      float lat =p->value().toFloat();  

      p = request->getParam(2);
      float log =p->value().toFloat();  
      digitalStablesData.latitude=lat;
      digitalStablesData.longitude=log;
    
        DynamicJsonDocument json(1800);
       this->generateWebData(json,serialNumber);
        serializeJson(json, *response);
        request->send(response);

    }
});


asyncWebServer.on("/DaffodilServlet", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int paramsNr = request->params();
    this->_HardSerial.println(paramsNr);
   
    AsyncWebParameter* p = request->getParam(0);
    //String formName =p->name;
    String formName =p->value();    
    this->_HardSerial.print("in servet2, formName=");
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

void DaffodilWifiManager::generateWebData(DynamicJsonDocument& json, String sentBy){
    
    json["devicename"] = digitalStablesData.devicename;
    json["deviceshortname"] = digitalStablesData.deviceshortname;
    json["groupidentifier"]=digitalStablesData.groupidentifier;
    json["secondsTime"] = digitalStablesData.secondsTime;
    json["dataSamplingSec"] = digitalStablesData.dataSamplingSec;
    json["currentFunctionValue"] = digitalStablesData.currentFunctionValue;
    json["temperature"] = digitalStablesData.temperature;
    json["rtcBatVolt"] = digitalStablesData.rtcBatVolt;
    json["opMode"] = digitalStablesData.opMode;
    json["rtcBatVolt"] = digitalStablesData.rtcBatVolt;        
    json["operatingStatus"] = digitalStablesData.operatingStatus;
   // json["sleepPingMinutes"] = digitalStablesData.sleepPingMinutes;
    json["digitalStablesUpload"] = digitalStablesData.digitalStablesUpload;
    json["secondsSinceLastPulse"] = digitalStablesData.secondsSinceLastPulse;
    json["currentFunctionValue"]=digitalStablesData.currentFunctionValue;
    json["outdoortemperature"] = digitalStablesData.outdoortemperature;
    json["outdoorhumidity"] = digitalStablesData.outdoorhumidity;
 //  json["minimumSepticHeight"] = digitalStablesData.minimumSepticHeight;
    json["maximumScepticHeight"] = digitalStablesData.maximumScepticHeight;
    json["scepticAvailablePercentage"] = digitalStablesData.scepticAvailablePercentage;
    json["capacitorVoltage"]=digitalStablesData.capacitorVoltage;
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
    json["deviceTypeId"]=digitalStablesData.deviceTypeId;
    json["dsLastUpload"]=digitalStablesData.dsLastUpload;
    json["latitude"]=digitalStablesData.latitude;
     json["longitude"]=digitalStablesData.longitude;
  }

int DaffodilWifiManager::uploadDataToDigitalStables(){

  DynamicJsonDocument json(1800);
  generateWebData(json, serialNumber);
  
  String output;
  serializeJson(json, output);
  const char* serverName = "http://devices.digitalstables.com/DeviceUploadServlet";
  http.setTimeout(1150);
   digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
  http.begin(serverName);    
   digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
  http.addHeader("Content-Type", "application/json");
  boolean toReturn=false;
  int httpResponseCode = http.POST(output);
  _HardSerial.print("upload digitalstables return ");
  _HardSerial.println(httpResponseCode);
   digitalStablesData.digitalStablesUpload=false;
   digitalStablesData.internetAvailable=false;
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
  if (httpResponseCode == 200) { //Check for the returning code
      String response = http.getString();  //Get the response to the request
       _HardSerial.print("reponse= ");
      _HardSerial.println(response);
      if(response=="Ok"){
        toReturn =true;
         digitalStablesData.digitalStablesUpload=true;
          digitalStablesData.internetAvailable=true;
      }else{
        httpResponseCode =500;
         digitalStablesData.digitalStablesUpload=false;
          digitalStablesData.internetAvailable=false;
        _HardSerial.print("line 405 upload digitalstables failed=");
         _HardSerial.print(response);
      }
  }else{
      _HardSerial.printf("Error Upload Digital Stables: %s\n", http.errorToString(httpResponseCode).c_str());
  }
   digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
  http.end();
   digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
   _HardSerial.print("upload digitalstables return ");
  _HardSerial.println(httpResponseCode);
  
  return httpResponseCode;//toReturn;
}


DaffodilWifiManager::~DaffodilWifiManager() {}