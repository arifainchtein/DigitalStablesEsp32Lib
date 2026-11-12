#include <DaffodilWifiManager.h>
#include <ArduinoJson.h>

 #define FUN_1_FLOW 1
#define FUN_2_FLOW 2
#define FUN_1_FLOW_1_TANK 3
#define FUN_1_TANK 4
#define FUN_2_TANK 5
#define WATCHDOG_WDI 18


DaffodilWifiManager::DaffodilWifiManager(HardwareSerial &serial,  FS &fs,PCF8563TimeManager &t, Esp32SecretManager &e,  DigitalStablesData& tf,DigitalStablesConfigData& p) : 
WifiManager(serial ,fs,  t,e) , digitalStablesData(tf),digitalStablesConfigData(p){}

 void DaffodilWifiManager::setWifiActiveSwitchStatus(bool b){
    wifiActiveSwitchStatus=b;
 }

void DaffodilWifiManager::start(){
 _isValid = true;
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
   
     ssids = scanNetworks();
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



// The whole page - Bootstrap, scripts, fonts, logo - is one file, data/index.html.gz built from
// web/ by build_web.py (2026-11-06, as on Annabelle). The 16 separate files it replaced were fetched
// over several connections at once, and over the soft AP the page never finished loading
// (HorseTrough, 2026-11-06). The page's ETag is the file's size and time: a reload the browser
// already has gets 304.
{
  File page = _fs.open("/index.html.gz", "r");
  if (page) {
    char etag[24];
    snprintf(etag, sizeof(etag), "\"%x-%lx\"", (unsigned)page.size(), (unsigned long)page.getLastWrite());
    pageEtag = etag;
    page.close();
  } else {
    this->_HardSerial.println("www: /index.html.gz missing - run build_web.py and ./uploadData.sh");
  }
}

asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
    sendPage(request);
});

asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request){
    sendPage(request);
});
  

asyncWebServer.on("/DaffodilServlet", HTTP_POST, [this](AsyncWebServerRequest *request) {
    // int paramsNr = request->params();
    // this->_HardSerial.println(paramsNr);
 String formName="";
     if (request->hasParam("formName", true)) {
         formName = request->getParam("formName", true)->value();
        this->_HardSerial.println("POST formName: " + formName);
    } else {
        request->send(400, "text/plain", "Missing formName in POST data");
    }
    
  
    
      const AsyncWebParameter* p ;
    //String formName =p->name;
        
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
      String flow1Name = request->getParam("flow1Name",true)->value();
      float qfactor1 = request->getParam("qfactor1",true)->value().toFloat();

      flow1Name.toCharArray(digitalStablesData.sensor1name, sizeof(digitalStablesData.sensor1name));
      digitalStablesData.qfactor1 = qfactor1;
      secretManager.saveFlow1Name(flow1Name);
      secretManager.saveQFactor1(qfactor1);

      DynamicJsonDocument json(1800);
      this->generateWebData(json,serialNumber);
      serializeJson(json, *response);
      request->send(response);

    }else if(formName=="SetFlowSensor2Param"){
      String flow2Name = request->getParam("flow2Name",true)->value();
      float qfactor2 = request->getParam("qfactor2",true)->value().toFloat();

      flow2Name.toCharArray(digitalStablesData.sensor2name, sizeof(digitalStablesData.sensor2name));
      digitalStablesData.qfactor2 = qfactor2;
      secretManager.saveFlow2Name(flow2Name);
      secretManager.saveQFactor2(qfactor2);

      DynamicJsonDocument json(1800);
      this->generateWebData(json,serialNumber);
      serializeJson(json, *response);
      request->send(response);

    }else if(formName=="SetTank1Param"){
      String tank1Name = request->getParam("tank1Name",true)->value();
      float tank1heightmeters = request->getParam("tank1heightmeters",true)->value().toFloat();
      float tank1maxvollit = request->getParam("tank1maxvollit",true)->value().toFloat();

      tank1Name.toCharArray(digitalStablesData.sensor1name, sizeof(digitalStablesData.sensor1name));
      digitalStablesData.tank1HeightMeters = tank1heightmeters;
      digitalStablesData.tank1maxvollit = tank1maxvollit;
      secretManager.saveTank1Name(tank1Name);
      secretManager.saveTank1Height(tank1heightmeters);
      secretManager.saveTank1MaxVol(tank1maxvollit);

      DynamicJsonDocument json(1800);
      this->generateWebData(json,serialNumber);
      serializeJson(json, *response);
      request->send(response);

    }else if(formName=="SetTank2Param"){
      String tank2name = request->getParam("tank2name",true)->value();
      float tank2heightmeters = request->getParam("tank2heightmeters",true)->value().toFloat();
      float tank2maxvollit = request->getParam("tank2maxvollit",true)->value().toFloat();

      tank2name.toCharArray(digitalStablesData.sensor2name, sizeof(digitalStablesData.sensor2name));
      digitalStablesData.tank2HeightMeters = tank2heightmeters;
      digitalStablesData.tank2maxvollit = tank2maxvollit;
      secretManager.saveTank2Name(tank2name);
      secretManager.saveTank2Height(tank2heightmeters);
      secretManager.saveTank2MaxVol(tank2maxvollit);

      DynamicJsonDocument json(1800);
      this->generateWebData(json,serialNumber);
      serializeJson(json, *response);
      request->send(response);

    }else if(formName=="CalibrateCSW"){
      // Web equivalent of the serial CalibrateCSWReference command: arms the flag, then reboots
      // so setup() captures the reference at the correct point in boot (see the serial command's
      // handler in Daffodil.ino for why it can't just read the ladder here). Caller is
      // responsible for having the config switches at 00000 before triggering this - same
      // requirement as the serial command.
      secretManager.armCSWCalibration();
      this->_HardSerial.println("CSW calibration armed via web - rebooting now to capture the reference (switches must be at 00000).");
      ESP.restart();

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
       String formName = request->getParam("formName")->value();

      p = request->getParam(1);
      float lat =p->value().toFloat();  

      const AsyncWebParameter* p2 = request->getParam(2);
      float log =p2->value().toFloat();  
      digitalStablesData.latitude=lat;
      digitalStablesData.longitude=log;
    
        DynamicJsonDocument json(1800);
       this->generateWebData(json,serialNumber);
        serializeJson(json, *response);
        request->send(response);

    }else if(formName=="SetViewerConfig"){
      String err = "Viewer not available";
      if (viewerConfigSetter && request->hasParam("config", true)) {
        err = viewerConfigSetter(request->getParam("config", true)->value());
      } else if (viewerConfigSetter) {
        err = "missing config";
      }
      if (err == "") {
        JsonDocument json;
        if (viewerDataProvider) viewerDataProvider(json);
        serializeJson(json, *response);
        request->send(response);
      } else {
        delete response;  // built above for the success path - not sent
        request->send(400, "text/plain", err);
      }

    }else if(formName=="SetScepticRange"){
         
      float height =request->getParam("height",true)->value().toFloat();  
      float max =request->getParam("max",true)->value().toFloat();  
      float min =request->getParam("min",true)->value().toFloat();  
            
      digitalStablesData.troughlevelminimumcm=min;
      digitalStablesData.troughlevelmaximumcm=max;
      digitalStablesData.maximumScepticHeight=height;
       secretManager.saveTroughParameters(digitalStablesData.maximumScepticHeight, digitalStablesData.troughlevelminimumcm, digitalStablesData.troughlevelmaximumcm);

        DynamicJsonDocument json(1800);
       this->generateWebData(json,serialNumber);
        serializeJson(json, *response);
        request->send(response);
    }
});


asyncWebServer.on("/DaffodilServlet", HTTP_GET, [this](AsyncWebServerRequest *request) {
 if (!request->hasParam("formName")) {
        request->send(400, "text/plain", "Missing formName parameter");
        return;
    }
    
    // Get the formName parameter by name (safer than by index)
    String formName = request->getParam("formName")->value();
    this->_HardSerial.println("formName: " + formName);
    if(formName=="GetWebData"){
      AsyncResponseStream *response = request->beginResponseStream("text/plain");
      DynamicJsonDocument json(1800);
      this->generateWebData(json, serialNumber);
      serializeJson(json, *response);
      request->send(response);
    }else if(formName=="GetPowerData"){
      AsyncResponseStream *response = request->beginResponseStream("text/plain");
      JsonDocument json;
      if (powerDataProvider) powerDataProvider(json);
      serializeJson(json, *response);
      request->send(response);
    }else if(formName=="GetViewerConfig"){
      AsyncResponseStream *response = request->beginResponseStream("text/plain");
      JsonDocument json;
      if (viewerDataProvider) viewerDataProvider(json);
      serializeJson(json, *response);
      request->send(response);
    }else{
      // Before 2026-10-14 an unknown form got no answer at all and the page just hung.
      request->send(400, "text/plain", "Unknown form " + formName);
    }
});
  
// asyncWebServer.on("/DaffodilServlet", HTTP_GET, [this](AsyncWebServerRequest *request) {

//      if (!_isValid) {
//         request->send(500, "text/plain", "Server instance no longer valid");
//         return;
//     }

//   this->_HardSerial.println("lin 261 numberof param=");
//     int paramsNr = request->params();
//     this->_HardSerial.println("lin 263 numberof param=");
//    this->  _HardSerial.println( paramsNr);
//     const AsyncWebParameter* p = request->getParam(1);
  
//     //String formName =p->name;
//     String formName =p->value();    
//    this-> _HardSerial.print("in servet2, formName=");
//    this-> _HardSerial.println(formName);
//     AsyncResponseStream *response = request->beginResponseStream("text/plain");
//     if(formName=="GetWebData"){
//        DynamicJsonDocument json(1800);
//       this->generateWebData(json, serialNumber);
//         serializeJson(json, *response);
//         request->send(response);
// 	  }
      
//   });



 asyncWebServer.on("/GetSensorData", HTTP_GET, [this](AsyncWebServerRequest *request){
       this->_HardSerial.println("curl request returning");
        this->_HardSerial.println(sensorString);
        request->send_P(200, "text/plain", sensorString.c_str()); 
    });
//asyncWebServer.serveStatic("/", fs, "/");
  asyncWebServer.onNotFound([](AsyncWebServerRequest *request){
    Serial.println("File Not Found: " + request->url());
    request->send(404, "text/plain", "Not found");
  });
asyncWebServer.begin();
  Serial.println("HTTP server started");
}

void DaffodilWifiManager::generateWebData(DynamicJsonDocument& json, String sentBy){
    
    json["devicename"] = digitalStablesData.devicename;
    json["deviceshortname"] = digitalStablesData.deviceshortname;
    json["groupidentifier"]=digitalStablesData.groupidentifier;
    json["secondsTime"] = digitalStablesData.secondsTime;
    json["dataSamplingSec"] = digitalStablesData.dataSamplingSec;
    json["batteryCurrent"] = digitalStablesData.batteryCurrent;
    json["temperature"] = digitalStablesData.temperature;
    json["rtcBatVolt"] = digitalStablesData.rtcBatVolt;
    json["opMode"] = digitalStablesData.opMode;
    json["rtcBatVolt"] = digitalStablesData.rtcBatVolt;        
    json["operatingStatus"] = digitalStablesData.operatingStatus;
   // json["sleepPingMinutes"] = digitalStablesData.sleepPingMinutes;
    json["digitalStablesUpload"] = digitalStablesData.digitalStablesUpload;
  //  json["secondsSinceLastPulse"] = digitalStablesData.secondsSinceLastPulse;
    json["currentFunctionValue"]=digitalStablesData.currentFunctionValue;
    json["outdoortemperature"] = digitalStablesData.outdoortemperature;
    json["outdoorhumidity"] = digitalStablesData.outdoorhumidity;
 //  json["minimumSepticHeight"] = digitalStablesData.minimumSepticHeight;
    json["maximumScepticHeight"] = digitalStablesData.maximumScepticHeight;
    json["troughlevelminimumcm"] = digitalStablesData.troughlevelminimumcm;
    json["troughlevelmaximumcm"] = digitalStablesData.troughlevelmaximumcm;
    json["panelVoltage"] = digitalStablesData.panelVoltage;
    json["panelCurrent"] = digitalStablesData.panelCurrent;
    json["maximumScepticHeight"] = digitalStablesData.maximumScepticHeight;
    json["measuredHeight"] = digitalStablesData.measuredHeight;
    json["maximumScepticHeight2"] = digitalStablesData.maximumScepticHeight2;
    json["measuredHeight2"] = digitalStablesData.measuredHeight2;

    json["batteryVoltage"]=digitalStablesData.batteryVoltage;
    json["v50Voltage"]=digitalStablesData.v50Voltage;
    json["batteryCurrent"]=digitalStablesData.batteryCurrent;
    json["estimatedRuntime"]=digitalStablesData.estimatedRuntime;
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

    // Names are fixed char[6] fields that may lack a terminator (2026-11-06: an unterminated name ran on
    // into binary bytes, the control characters went out raw and the browser rejected the whole JSON).
    // Read at most the field's size and keep printable characters only.
    auto boundedName = [](const char *s, size_t n) {
      String out;
      for (size_t i = 0; i < n && s[i]; i++) if (s[i] >= 32 && s[i] < 127) out += s[i];
      return out;
    };
    json["flow1name"] = boundedName(digitalStablesData.sensor1name, sizeof(digitalStablesData.sensor1name));
    json["flow2name"] = boundedName(digitalStablesData.sensor2name, sizeof(digitalStablesData.sensor2name));
    json["tank1name"] = boundedName(digitalStablesData.sensor1name, sizeof(digitalStablesData.sensor1name));
    json["tank2name"] = boundedName(digitalStablesData.sensor2name, sizeof(digitalStablesData.sensor2name));
    json["qfactor1"] = digitalStablesData.qfactor1;
    json["qfactor2"] = digitalStablesData.qfactor2;
    json["flowrate"] = digitalStablesData.flowRate;
    json["flowrate2"] = digitalStablesData.flowRate2;
    json["totalmilliLitres"] = digitalStablesData.totalMilliLitres;
    json["totalmilliLitres2"] = digitalStablesData.totalMilliLitres2;
    json["tank1heightmeters"] = digitalStablesData.tank1HeightMeters;
    json["tank2heightmeters"] = digitalStablesData.tank2HeightMeters;
    json["tank1maxvollit"] = digitalStablesData.tank1maxvollit;
    json["tank2maxvollit"] = digitalStablesData.tank2maxvollit;

    // 0 = this device has never run CalibrateCSW/CalibrateCSWReference - see the serial command's
    // comment in Daffodil.ino for why this matters after a battery swap (the CSW ladder's raw
    // reading scales with whatever the attached battery/boost-converter actually outputs).
    json["cswReferenceRaw"] = secretManager.getCSWReference();
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
   digitalStablesData.wifiStatus=2;
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
          digitalStablesData.wifiStatus=3;
      }else{
        httpResponseCode =500;
         digitalStablesData.digitalStablesUpload=false;
          digitalStablesData.wifiStatus=2;
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


DaffodilWifiManager::~DaffodilWifiManager() {
    _isValid = false;
    asyncWebServer.end();
}

// data/index.html.gz - the web library finds the .gz for /index.html and sends it with
// Content-Encoding: gzip. At most WEB_PAGE_MAX_ACTIVE page replies at once (each holds ~10 KB until
// its connection closes); more get 503 and the browser simply reloads.
void DaffodilWifiManager::sendPage(AsyncWebServerRequest *request) {
  if (pageEtag.length() && request->hasHeader("If-None-Match")
      && request->getHeader("If-None-Match")->value() == pageEtag) {
    request->send(304);
    return;
  }
  if (pageActive >= WEB_PAGE_MAX_ACTIVE) {
    request->send(503, "text/plain", "busy");
    return;
  }
  AsyncWebServerResponse *response = request->beginResponse(_fs, "/index.html", "text/html");
  if (!response) {  // NULL when the file can't be opened
    request->send(503, "text/plain", "page missing - run build_web.py and ./uploadData.sh");
    return;
  }
  pageActive++;
  request->onDisconnect([this]() { if (pageActive) pageActive--; });
  if (pageEtag.length()) response->addHeader("ETag", pageEtag);
  response->addHeader("Cache-Control", "no-cache");  // keep it, but ask (304) before using it
  request->send(response);
}
