#include <CajalWifiManager.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"

bool wifiActiveSwitchStatus = false;

CajalWifiManager::CajalWifiManager(HardwareSerial &serial, DataManager &d, PCF8563TimeManager &t, Esp32SecretManager &e, CajalData &tf, CajalConfigData &p) : WifiManager(serial, t, e), DataManager() CajalData(tf), CajalConfigData(p) {}

void CajalWifiManager::setWifiActiveSwitchStatus(bool b)
{
    wifiActiveSwitchStatus = b;
}

void CajalWifiManager::start()
{

    if (!SPIFFS.begin(true))
    {
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
    _HardSerial.println("ssid=");
    _HardSerial.println(ssid);
    _HardSerial.println("stationmode=");
    _HardSerial.println(stationmode);

    _HardSerial.print("in  CajalWifiManager ssid=");
    _HardSerial.print(ssid);
    _HardSerial.print(" stationmode=");
    _HardSerial.println(stationmode);
    _HardSerial.println("about to do start scan");
    ssids = scanNetworks();
    //
    // set the mode to null so that
    // the hostname is applied
    //  https://github.com/espressif/arduino-esp32/issues/6700
    WiFi.mode(WIFI_MODE_NULL);
    if (wifiActiveSwitchStatus && stationmode && ssid)
    {
        connectSTA();
    }
    else
    {

        _HardSerial.print("ssids=");
        _HardSerial.println(ssids);

        _HardSerial.println("about to do connectAP");
        connectAP();
    }

    asyncWebServer.on("/assets/bootstrap/css/bootstrap.min.css", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this->_HardSerial.println("request  bootstrap.min.css=");
    request->send(SPIFFS, "/bootstrap.min.css", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/img/Cajal.svg", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this->_HardSerial.println("request  Cajal.svg");
    request->send(SPIFFS, "/Cajal.svg", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/js/TankAndFlowConstants.js", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
 // this->_HardSerial.println("request TankFlowConstants.js");
    request->send(SPIFFS, "/TankAndFlowConstants.js", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/js/jquery.min.js", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
 // this->_HardSerial.println("request  jquery/=");
    request->send(SPIFFS, "/jquery.min.js", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/css/slideswitch.css", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this->_HardSerial.println("request  slideswitch/=");
    request->send(SPIFFS, "/slideswitch.css", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/css/styles.css", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this->_HardSerial.println("request styles.css /=");
    request->send(SPIFFS, "/styles.css", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/fonts/fa-solid-900.woff2", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this->_HardSerial.println("request solid woof2 /=");
    request->send(SPIFFS, "/fa-solid-900.woff2", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/fonts/fa-solid-900.woff", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this->_HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/fa-solid-900.woff", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/fonts/fa-solid-900.ttf", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this->_HardSerial.println("request  solid woof ttf /=");
    request->send(SPIFFS, "/fa-solid-900.ttf", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/fonts/fontawesome-all.min.css", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
 // this->_HardSerial.println("request fontawesome /=");
    request->send(SPIFFS, "/fontawesome-all.min.css", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/bootstrap/js/bootstrap.min.js", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
//  this->_HardSerial.println("request  bootstramo min js /=");
    request->send(SPIFFS, "/bootstrap.min.js", String(), false);
    delay(5); });

    asyncWebServer.on("/js/index.js", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
 // delay(5);
 // this->_HardSerial.println("request  indexjs /=");
  delay(5);
    request->send(SPIFFS, "/index.js", String(), false);
    delay(5); });

    asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
	//this->_HardSerial.println("reqestubg  root =");
    request->send(SPIFFS, "/index.html", String(), false);
    delay(5); });

    asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
	//this->_HardSerial.println("reqestubg index.html=");
    request->send(SPIFFS, "/index.html", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/css/Roboto.css", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
	//this->_HardSerial.println("reqestubg robottocss=");
    request->send(SPIFFS, "/Roboto.css", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/fonts/Roboto-Regular.woff", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this->_HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/Roboto-Regular.woff", String(), false);
    delay(5); });

    asyncWebServer.on("/assets/fonts/Roboto-Regular.woff2", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this->_HardSerial.println("request solid woof900 /=");
    request->send(SPIFFS, "/Roboto-Regular.woff2", String(), false);
    delay(5); });

    asyncWebServer.on("/GetHostName", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
  //this->_HardSerial.println("request solid woof900 /=");
     request->send_P(200, "text/plain", hostname.c_str()); 
    delay(5); });

    asyncWebServer.on("/CajalServlet", HTTP_POST, [this](AsyncWebServerRequest *request)
                      {
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
      CajalData.latitude=lat;
      CajalData.longitude=log;
    
        DynamicJsonDocument json(1800);
       this->generateWebData(json,serialNumber);
        serializeJson(json, *response);
        request->send(response);

    } });

    asyncWebServer.on("/CajalServlet", HTTP_GET, [this](AsyncWebServerRequest *request){
        int paramsNr = request->params();
        this->_HardSerial.println(paramsNr);

        AsyncWebParameter *p = request->getParam(0);
        // String formName =p->name;
        String formName = p->value();
        this->_HardSerial.print("in servet2, formName=");
        this->_HardSerial.println(formName);
        AsyncResponseStream *response = request->beginResponseStream("text/plain");
        if (formName == "GetWebData"){
            uint16_t totalSize = dataManager.getTotalSize();
            int docSize = 1;
            while (power < 1.2 * totalSize)
                docSize *= 2;
            StaticJsonDocument<docSize> doc;
            JsonArray array = doc.to<JsonArray>();

            this->generateWebData(array, serialNumber);

            serializeJson(doc, *response);
            request->send(response);
        } 
    });

    asyncWebServer.on("/GetSensorData", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->_HardSerial.println("curl request returning");
        this->_HardSerial.println(sensorString);
        request->send_P(200, "text/plain", sensorString.c_str()); });
        asyncWebServer.begin();
    }

void CajalWifiManager::generateWebData( JsonArray& array, String sentBy){
    JsonObject obj2 = doc.createNestedObject();
}

int CajalWifiManager::uploadDataToDigitalStables()
{

    DynamicJsonDocument json(1800);
    generateWebData(json, serialNumber);

    String output;
    serializeJson(json, output);
    const char *serverName = "http://devices.digitalstables.com/DeviceUploadServlet";
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    boolean toReturn = false;
    int httpResponseCode = http.POST(output);
    _HardSerial.print("upload digitalstables return ");
    _HardSerial.println(httpResponseCode);
    cajalData.digitalStablesUpload = false;
    if (httpResponseCode == 200)
    {                                       // Check for the returning code
        String response = http.getString(); // Get the response to the request
        _HardSerial.print("reponse= ");
        _HardSerial.println(response);
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
    _HardSerial.print("upload digitalstables return ");
    _HardSerial.println(httpResponseCode);

    return httpResponseCode; // toReturn;
}

CajalWifiManager::~CajalWifiManager() {}