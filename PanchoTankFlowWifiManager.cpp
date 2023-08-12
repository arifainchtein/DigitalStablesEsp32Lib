#include <PanchoTankFlowWifiManager.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
 #define FUN_1_FLOW 1
#define FUN_2_FLOW 2
#define FUN_1_FLOW_1_TANK 3
#define FUN_1_TANK 4
#define FUN_2_TANK 5


PanchoTankFlowWifiManager::PanchoTankFlowWifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e,  PanchoTankFlowData& tf,PanchoConfigData& p) :
WifiManager(serial ,  t, e) , panchoTankFlowData(tf),panchoConfigData(p){}

 

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
   //getCurrentTimeInSeconds
   // get the parameters
   //
   ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();;
    soft_ap_ssid = secretManager.getSoftAPSSID();
    soft_ap_password = secretManager.getSoftAPPASS();
    hostname=secretManager.getHostName();
    uint8_t hostnamelength = hostname.length()+1;
    hostname.toCharArray(panchoTankFlowData.devicename, hostnamelength);
   
    stationmode = secretManager.getStationMode();
    _HardSerial.println("ssid=");
    _HardSerial.println(ssid);
    _HardSerial.println("stationmode=");
    _HardSerial.println(stationmode);
    _HardSerial.println("hosdname=");
    _HardSerial.println(hostname);
    _HardSerial.println("panchoTankFlowData.devicename=");
    _HardSerial.println(panchoTankFlowData.devicename);
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
     this->_HardSerial.print("servlet received, number f params=");
     
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
      // String flow1Name   
      String flow1Name=p->value();
       this->_HardSerial.print("received set sensor 1 name=");
         this->_HardSerial.println(flow1Name);
      int str_len = flow1Name.length() + 1; 
      char flow1NameArray[16];
      flow1Name.toCharArray(flow1NameArray, 16);
      memcpy(panchoTankFlowData.flow1name, flow1NameArray, 16);
    
      p = request->getParam(2);
      float qf1=p->value().toFloat(); 
      panchoTankFlowData.qfactor1=qf1; 
      this->_HardSerial.print("received qf1=");
         this->_HardSerial.println(qf1);

        DynamicJsonDocument json(1800);
       this->generateWebData(json,serialNumber);
        serializeJson(json, *response);
        request->send(response);

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
      panchoTankFlowData.latitude=lat;
      panchoTankFlowData.longitude=log;
    
        DynamicJsonDocument json(1800);
       this->generateWebData(json,serialNumber);
        serializeJson(json, *response);
        request->send(response);

    }else if(formName=="SetFlowSensor2Param"){
      p = request->getParam(1);
      String fn2 =p->value();  
     
      fn2.toCharArray(panchoTankFlowData.flow2name,16);

      p = request->getParam(2);
      panchoTankFlowData.qfactor2=p->value().toFloat();  

      DynamicJsonDocument json(1800);
      this->generateWebData(json, serialNumber);
      serializeJson(json, *response);
      request->send(response);

    }else if(formName=="SetFlowSensor1Param"){
      p = request->getParam(1);
      String fn1=p->value();  
     
      fn1.toCharArray(panchoTankFlowData.flow1name,16);

      p = request->getParam(2);
      panchoTankFlowData.qfactor1=p->value().toFloat();  

      DynamicJsonDocument json(1800);
      this->generateWebData(json, serialNumber);
      serializeJson(json, *response);
      request->send(response);

    }else if(formName=="SetTank1Param"){
      p = request->getParam(1);
      String t1n =p->value();  
     
      t1n.toCharArray(panchoTankFlowData.tank1name,16);

      p = request->getParam(2);
      panchoTankFlowData.tank1heightmeters=p->value().toFloat();  

      p = request->getParam(2);
      panchoTankFlowData.tank1maxvollit=p->value().toFloat();  

        DynamicJsonDocument json(1800);
       this->generateWebData(json, serialNumber);
        serializeJson(json, *response);
        request->send(response);

    }else if(formName=="SetTank2Param"){
      p = request->getParam(1);
      String t2n =p->value();  
      t2n.toCharArray(panchoTankFlowData.tank2name,16);

       p = request->getParam(2);
      panchoTankFlowData.tank2heightmeters=p->value().toFloat();  

      p = request->getParam(2);
      panchoTankFlowData.tank2maxvollit=p->value().toFloat();  

        DynamicJsonDocument json(1800);
       this->generateWebData(json, serialNumber);
        serializeJson(json, *response);
        request->send(response);

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

void PanchoTankFlowWifiManager::generateWebData(DynamicJsonDocument& json, String sentBy){
    json["currentFunctionValue"]= panchoTankFlowData.currentFunctionValue;
    json["flow1name"] = panchoTankFlowData.flow1name;
    json["flow2name"] = panchoTankFlowData.flow2name;
    json["tank1name"] = panchoTankFlowData.tank1name;
    json["tank2name"] = panchoTankFlowData.tank2name;
    json["groupidentifier"]=panchoTankFlowData.groupidentifier;
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
    json["deviceTypeId"]=panchoTankFlowData.deviceTypeId;
    json["dsLastUpload"]=panchoTankFlowData.dsLastUpload;
    json["latitude"]=panchoTankFlowData.latitude;
     json["longitude"]=panchoTankFlowData.longitude;
  }

int PanchoTankFlowWifiManager::uploadDataToDigitalStables(){

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
   panchoTankFlowData.digitalStablesUpload=false;
  if (httpResponseCode == 200) { //Check for the returning code
      String response = http.getString();  //Get the response to the request
    //   _HardSerial.print("reponse= ");
    //  _HardSerial.println(response);
      if(response=="Ok"){
        toReturn =true;
         panchoTankFlowData.digitalStablesUpload=true;
      }else{
        httpResponseCode =500;
      }
  }
  http.end();
   _HardSerial.print("upload digitalstables return ");
  _HardSerial.println(httpResponseCode);
  
  return httpResponseCode;//toReturn;
}


PanchoTankFlowWifiManager::~PanchoTankFlowWifiManager() {}