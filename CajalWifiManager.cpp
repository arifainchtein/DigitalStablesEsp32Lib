#include <CajalWifiManager.h>
#include <ArduinoJson.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
//#include <LittleFS.h>
//#define SPIFFS LittleFS


CajalWifiManager::CajalWifiManager(HardwareSerial &serial, DataManager &d, PCF8563TimeManager &t, Esp32SecretManager &e, CajalData &tf) : WifiManager(serial, t, e), dataManager(d), cajalData(tf) {}



void CajalWifiManager::start()
{

    if (!SPIFFS.begin(true))
    {
        // SPIFFS will be configured on reboot
        if(debug) _HardSerial.println("ERROR: Cannot mount SPIFFS, Rebooting");
        delay(1000);
    }else{
      if(debug) _HardSerial.println("SPIFF begin success");
    }

    // File root = SPIFFS.open("/") ;
    // File file=root.openNextFile();
    // while(file){
    //   if(debug) _HardSerial.println(file.name());
    //   file=root.openNextFile();
    // }
    // getCurrentTimeInSeconds
    // get the parameters
    //
    ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();
    ;
    soft_ap_ssid = secretManager.getSoftAPSSID();
    soft_ap_password = secretManager.getSoftAPPASS();
    hostname = secretManager.getHostName();
    stationmode = secretManager.getStationMode();
    if(debug) _HardSerial.println("ssid=");
    if(debug) _HardSerial.println(ssid);
    if(debug) _HardSerial.println("stationmode=");
    if(debug) _HardSerial.println(stationmode);

    if(debug) _HardSerial.print("in  CajalWifiManager ssid=");
    if(debug) _HardSerial.print(ssid);
    if(debug) _HardSerial.print(" stationmode=");
    if(debug) _HardSerial.println(stationmode);
    if(debug) _HardSerial.println("about to do start scan");
    ssids = scanNetworks();
    //
    // set the mode to null so that
    // the hostname is applied
    //  https://github.com/espressif/arduino-esp32/issues/6700
    WiFi.mode(WIFI_MODE_NULL);
    if (stationmode && ssid)
    {
        connectSTA();
    }
    else
    {

        if(debug) _HardSerial.print("ssids=");
        if(debug) _HardSerial.println(ssids);

        if(debug) _HardSerial.println("about to do connectAP");
        connectAP();
    }

    asyncWebServer.on("/assets/bootstrap/css/bootstrap.min.css", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this-> _HardSerial.println("request  bootstrap.min.css=");
    request->send(SPIFFS, "/bootstrap.min.css", String(), false);
    //delay(5);
     });

    asyncWebServer.on("/assets/img/Cajal.svg", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/Cajal.svg", String(), false);
   // delay(5); 
    });


 asyncWebServer.on("/assets/img/Pancho.svg", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/Pancho.svg", String(), false);
 //   delay(5); 
    });

 asyncWebServer.on("/assets/img/Rosie.svg", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/Rosie.svg", String(), false);
   // delay(5); 
    });

asyncWebServer.on("/assets/img/Gloria.svg", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/Gloria.svg", String(), false);
   // delay(5); 
    });

    asyncWebServer.on("/assets/js/TankAndFlowConstants.js", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  this-> _HardSerial.println("request TankFlowConstants.js");
    request->send(SPIFFS, "/TankAndFlowConstants.js", String(), false);
    //delay(5); 
    });

    asyncWebServer.on("/assets/js/jquery.min.js", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  this-> _HardSerial.println(" point 1request  jquery/=");

    request->send(SPIFFS, "/jquery.min.js", String(), false);
  this-> _HardSerial.println(" point 2request  jquery/=");
  
   // delay(5);
     });

    asyncWebServer.on("/assets/css/slideswitch.css", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
this-> _HardSerial.println("request  slideswitch/=");
    request->send(SPIFFS, "/slideswitch.css", String(), false);
  //  delay(5);
     });

    asyncWebServer.on("/assets/css/styles.css", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  this-> _HardSerial.println("request styles.css /=");
    request->send(SPIFFS, "/styles.css", String(), false);
//    delay(5);
     });

    asyncWebServer.on("/assets/fonts/fa-solid-900.woff2", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  this-> _HardSerial.println("request solid woof2 /=");
    request->send(SPIFFS, "/fa-solid-900.woff2", String(), false);
   // delay(5); 
   });

    asyncWebServer.on("/assets/fonts/fa-solid-900.woff", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  this-> _HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/fa-solid-900.woff", String(), false);
   // delay(5);
    });

    asyncWebServer.on("/assets/fonts/fa-solid-900.ttf", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  this-> _HardSerial.println("request  solid woof ttf /=");
    request->send(SPIFFS, "/fa-solid-900.ttf", String(), false);
  // delay(5); 
  });

    asyncWebServer.on("/assets/fonts/fontawesome-all.min.css", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
 this-> _HardSerial.println("request fontawesome /=");
    request->send(SPIFFS, "/fontawesome-all.min.css", String(), false);
    //delay(5); 
    
    });

    asyncWebServer.on("/assets/bootstrap/js/bootstrap.min.js", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
 this-> _HardSerial.println("request  bootstramo min js /=");
    request->send(SPIFFS, "/bootstrap.min.js", String(), false);
   // delay(5); 
   });

    asyncWebServer.on("/js/index.js", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
 // delay(5);
 this-> _HardSerial.println("request  indexjs /=");
  //delay(5);
    request->send(SPIFFS, "/index.js", String(), false);
   // delay(5); 
   });

    asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
	//this-> _HardSerial.println("reqestubg  root =");
    request->send(SPIFFS, "/index.html", String(), false);
  //  delay(5); 
    });

    asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
	//this-> _HardSerial.println("reqestubg index.html=");
    request->send(SPIFFS, "/index.html", String(), false);
 //   delay(5); 
    });

    asyncWebServer.on("/assets/css/Roboto.css", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
	//this-> _HardSerial.println("reqestubg robottocss=");
    request->send(SPIFFS, "/Roboto.css", String(), false);
   // delay(5); 
    });

    asyncWebServer.on("/assets/fonts/Roboto-Regular.woff", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this-> _HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/Roboto-Regular.woff", String(), false);
   // delay(5);
     });

    asyncWebServer.on("/assets/fonts/Roboto-Regular.woff2", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this-> _HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/Roboto-Regular.woff2", String(), false);
   // delay(5); 
    });

    asyncWebServer.on("/GetHostName", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this-> _HardSerial.println("request solid woof900 /=");
     request->send_P(200, "text/plain", hostname.c_str()); 
   // delay(5); 
    });

    asyncWebServer.on("/CajalServlet", HTTP_POST, [this](AsyncWebServerRequest *request)
                      {
    int paramsNr = request->params();
    this-> _HardSerial.println(paramsNr);
   
    AsyncWebParameter* p = request->getParam(0);
    //String formName =p->name;
    String formName =p->value();    
    this-> _HardSerial.print("in post, formName=");
    this-> _HardSerial.println(formName);
    AsyncResponseStream *response = request->beginResponseStream("text/plain");

    if(formName=="ConfigSTA"){

      p = request->getParam(1);
      String ssid =p->value();    
  
      p = request->getParam(2);
      String password =p->value();  

      p = request->getParam(3);
      String hostName =p->value();  

      this-> _HardSerial.print("in servet2, about to set to station=");
     
      bool staok = this->configWifiSTA(ssid, password, hostName);
      if (staok) {
        this-> _HardSerial.print("rebooting ");
        ESP.restart();
      } else {
        this-> _HardSerial.print("failed change to station");
     
      }
    }else  if(formName=="ConfigAP"){

      p = request->getParam(1);
      String apaddress =p->value();    
  
      p = request->getParam(2);
      String password =p->value();  

      p = request->getParam(3);
      String hostName =p->value();  

      this-> _HardSerial.print("in servet2, about to set to access point,h=");
      this-> _HardSerial.print(hostName);
      this-> _HardSerial.print(" apaddress=");
      this-> _HardSerial.println(apaddress);

     
      bool staok = this->configWifiAP(apaddress, password, hostName);
      if (staok) {
        this-> _HardSerial.print("rebooting after setting access point ");
        ESP.restart();
      } else {
        this-> _HardSerial.print("failed change to ap");
     
      }
    }else if(formName=="SetTimeViaInternet"){
      bool r = setTimeFromInternet();
        DynamicJsonDocument json(1800);
       this->generateWebData(json,serialNumber);
        serializeJson(json, *response);
        request->send(response);

    }else if(formName=="ManualSetTime"){
       p = request->getParam(1);
      String timeString =p->value();
      this-> _HardSerial.print("Settg time manually:");
      this-> _HardSerial.println(timeString);
      timeManager.setTime(timeString);
       DynamicJsonDocument json(1800);
       this->generateWebData(json, serialNumber);
       serializeJson(json, *response);
        request->send(response);
    }else  if(formName=="SetTimeZone"){
      p = request->getParam(1);
      String timezone =p->value();    
      secretManager.setTimeZone(timezone);
    }else if(formName=="SetGPS"){
      p = request->getParam(1);
      float lat =p->value().toFloat();  

      p = request->getParam(2);
      float log =p->value().toFloat();  
      cajalData.latitude=lat;
      cajalData.longitude=log;
    
        DynamicJsonDocument json(1800);
       this->generateWebData(json,serialNumber);
        serializeJson(json, *response);
        request->send(response);

    } });

    asyncWebServer.on("/CajalServlet", HTTP_GET, [this](AsyncWebServerRequest *request){
        int paramsNr = request->params();
        this-> _HardSerial.println(paramsNr);

        AsyncWebParameter *p = request->getParam(0);
        // String formName =p->name;
        String formName = p->value();
        this-> _HardSerial.print("in servet2, formName=");
        this-> _HardSerial.println(formName);
        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        if (formName == "GetWebData"){
            DynamicJsonDocument json(1800);
            this->generateWebData(json, serialNumber);
            serializeJson(json, *response);
            request->send(response);
        } else if (formName == "GetDevicesData"){
            serializeJson(dataManager.completeObject, *response);
            request->send(response);
        } 
    });

     asyncWebServer.on("/GetSensorData", HTTP_GET, [this](AsyncWebServerRequest *request){
       this-> _HardSerial.println("curl request returning");
        this-> _HardSerial.println(sensorString);
        request->send_P(200, "text/plain", sensorString.c_str()); 
    });


  asyncWebServer.begin();

}




void CajalWifiManager::generateWebData(DynamicJsonDocument& json, String sentBy){
    json["currentFunctionValue"]= cajalData.currentFunctionValue;
    json["groupidentifier"]=cajalData.groupidentifier;
    json["secondsTime"] = cajalData.secondsTime;
    json["dataSamplingSec"] = cajalData.dataSamplingSec;
    json["temperature"] = cajalData.temperature;
    json["rtcBatVolt"] = cajalData.rtcBatVolt;
    json["opMode"] = cajalData.opMode;
    json["rssi"] = cajalData.rssi;
    json["snr"] = cajalData.snr;
    json["rtcBatVolt"] = cajalData.rtcBatVolt;        
    json["operatingStatus"] = cajalData.operatingStatus;
    json["digitalStablesUpload"] = cajalData.digitalStablesUpload;
    json["secondsSinceLastPulse"] = cajalData.secondsSinceLastPulse;
    json["soft_ap_ssid"] = soft_ap_ssid;
    json["serialnumber"] = serialNumber;
   
   

    json["sentBy"] = serialNumber;
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
    json["deviceTypeId"]=cajalData.deviceTypeId;
    json["dsLastUpload"]=cajalData.dsLastUpload;
    json["latitude"]=cajalData.latitude;
     json["longitude"]=cajalData.longitude;
  }

int CajalWifiManager::uploadDataToDigitalStables()
{

    DynamicJsonDocument json(1800);
    generateWebData(json,"");
    String output;
    serializeJson(json, output);
    
    const char *serverName = "http://devices.digitalstables.com/DeviceUploadServlet";
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    boolean toReturn = false;
    int httpResponseCode = http.POST(output);
   if(debug) _HardSerial.print("upload digitalstables return ");
    if(debug) _HardSerial.println(httpResponseCode);
    cajalData.digitalStablesUpload = false;
    if (httpResponseCode == 200)
    {                                       // Check for the returning code
        String response = http.getString(); // Get the response to the request
        if(debug) _HardSerial.print("reponse= ");
        if(debug) _HardSerial.println(response);
        if (response == "Ok")
        {
            toReturn = true;
            cajalData.digitalStablesUpload = true;
        }
        else
        {
            httpResponseCode = 500;
        }
    }
    http.end();
    if(debug) _HardSerial.print("upload digitalstables return ");
    if(debug) _HardSerial.println(httpResponseCode);

    return httpResponseCode; // toReturn;
}



int CajalWifiManager::uploadDigitalStablesDataToDigitalStables(DigitalStablesData& digitalStablesData){

  DynamicJsonDocument json(1800);
  // JSONObjectTransformer::generateDaffodilWebData(daffodilData,json, serialNumber);
  memcpy(digitalStablesData.sentbyarray, cajalData.serialnumberarray, sizeof(cajalData.serialnumberarray));
 
  dataManager.generateDigitalStablesData(digitalStablesData, json);
  String output;
  serializeJson(json, output);
  const char* serverName = "http://devices.digitalstables.com/DeviceUploadServlet";
  http.begin(serverName);    
  http.addHeader("Content-Type", "application/json");
  boolean toReturn=false;
  int httpResponseCode = http.POST(output);
  if(debug) _HardSerial.print("upload  digitalStablesData ");
 if(debug) _HardSerial.print(digitalStablesData.devicename);
  if(debug) _HardSerial.print(" to digitalstables return ");
if(debug) _HardSerial.println(httpResponseCode);
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


CajalWifiManager::~CajalWifiManager() {}