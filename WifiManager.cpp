/*
 * WifiManager.cpp
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#include <WifiManager.h>


WifiManager::WifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e,  PanchoTankFlowData& tf,PanchoConfigData& p) :
 _HardSerial(serial),timeManager(t),secretManager(e), panchoTankFlowData(tf) ,panchoConfigData(p),asyncWebServer(80)  {}

void WifiManager::setLora(boolean b){
    lora=b;
    _HardSerial.print("setting lora to ");
    _HardSerial.println(lora);
    
}

void WifiManager::setSerialNumber(String s){
    serialNumber=s;
}
void WifiManager::setCurrentTimerRecord(const RTCInfoRecord c){
    currentTimerRecord=c;   
}
    
bool WifiManager::getStationMode( )
{
    return stationmode;
}

String WifiManager::getMacAddress( )
{
    return WiFi.macAddress();
}

void WifiManager::setCurrentSSID(String s)
{
    ssid = s;
}
void WifiManager::setSensorString(String s)
{
    sensorString=s;
}


String WifiManager::getApAddress()
{
    apAddress = WiFi.softAPIP().toString();

    return apAddress;
}


double WifiManager::getRSSI()
{
    return WiFi.RSSI();
}


String WifiManager::getSoft_ap_password()
{
    return soft_ap_password;
}

void WifiManager::setSoft_ap_password(String s)
{
     soft_ap_password=s;
}

String WifiManager::getSoft_ap_ssid()
{
    return soft_ap_ssid;
}

void WifiManager::setSoft_ap_ssid(String s)
{
     soft_ap_ssid=s;
}

String WifiManager::getIpAddress()
{
    return ipAddress;
}

String WifiManager::getHostName()
{
    return hostname;
}

void  WifiManager::setHostName(String c)
{
     hostname=c;
}

String WifiManager::getSSID()
{
    return ssid;
}

void  WifiManager::setSSID(String c)
{
     ssid=c;
}

void WifiManager::createDeneWord(JsonObject& deneWord, String name, String value, String valueType){
    deneWord["Name"] = name;
	deneWord["Value"] =value;
	deneWord["Dene Type"] = valueType;
}

void WifiManager::createDeneWord(JsonObject& deneWord, String name, float value, String valueType){
    deneWord["Name"] = name;
	deneWord["Value"] =value;
	deneWord["Value Type"] = valueType;
}

void WifiManager::createDeneWord(JsonObject& deneWord, String name, int value, String valueType){
    deneWord["Name"] = name;
	deneWord["Value"] =value;
	deneWord["Value Type"] = valueType;
}

void WifiManager::createDeneWord(JsonObject& deneWord, String name, long value, String valueType){
    deneWord["Name"] = name;
	deneWord["Value"] =value;
	deneWord["Value Type"] = valueType;
}


String WifiManager::getTeleonomeData(String url, bool debug){
    
    
    int endPos = url.indexOf(".local");
    if(endPos>0){
        // http://Sento.local
        // http://Ra.local
        int startPos = 7;
        String hostname = url.substring(startPos, endPos);// + ".local";
         if(debug){
            Serial.print("hostname: ");
            Serial.print(hostname);
        }
        IPAddress ipAddress;
            
        int err = WiFi.hostByName(hostname.c_str(), ipAddress) ;
        String ipA = String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3])  ; 
        if(debug){
            Serial.print(" Ip address: ");
            Serial.print(ipA);
            Serial.print(" err: ");
            Serial.print(err);
        }
        if(err == 1){
            String h = (String)hostname;
            h.concat(".local");
                url.replace(h,ipA);
                if(debug){
                    Serial.print(" new url: ");
                    Serial.println(url);
                }
                
        } 
    }
    
    String toReturn="Error";
   
   // _HardSerial.println(WiFi.localIP());

 // _HardSerial.print("Fetching " + url );

   // if(WiFi.status()== WL_CONNECTED){
        http.begin(url);    
        int httpCode = http.GET();
        if (httpCode == 200) { //Check for the returning code
            toReturn = http.getString();
        }else{
           toReturn="Error:" + httpCode; 
        }
        http.end();
   // }


    if(debug){
        _HardSerial.print("teleonome data httpCode: ");
        _HardSerial.println(httpCode);
    }
 
    return toReturn;
}

String WifiManager::scanNetworks() {
    delay(5);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(5);
    int n= WiFi.scanNetworks();
    delay(5);
     String json = "[";
     n = WiFi.scanComplete();
      this->_HardSerial.println("scan 2 complete");
    if(n){
      for (int i = 0; i < n; ++i){
        if(i) json += ",";
        json += "{";
        json += "\"rssi\":"+String(WiFi.RSSI(i));
        json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
        json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
        json += ",\"channel\":"+String(WiFi.channel(i));
        json += ",\"secure\":"+String(WiFi.encryptionType(i));
        //json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
        this->_HardSerial.print(",");
        this->_HardSerial.print(WiFi.SSID(i));
        json += "}";
      }
    }
    json += "]";
      WiFi.scanDelete();
      return json;
}

uint8_t WifiManager::getWifiStatus(){
    return WiFi.status();
}

bool WifiManager::getAPStatus(){
    return apConnected;
}

bool WifiManager::configWifiAP( String sas, String sap, String h ){
    ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();;
    soft_ap_ssid = sas;
    soft_ap_password = sap;
    hostname=h;
    stationmode=false;
    secretManager.saveWifiParameters( ssid,  password, soft_ap_ssid, soft_ap_password,hostname, stationmode);
   // secretManager.saveConfigData(float fieldId, stationName );
    WiFi.disconnect();
    return connectAP();
}

bool WifiManager::configWifiSTA(String s, String p, String h ){
    ssid = s;
    password = p;
    hostname=h;
    soft_ap_ssid = secretManager.getSoftAPSSID();
    if(soft_ap_ssid==""){
        soft_ap_ssid="192.168.4.1";
    }else{
        
    }
    soft_ap_password = secretManager.getSoftAPPASS();
    //hostname=secretManager.getHostName();
    stationmode=true;
    secretManager.saveWifiParameters( ssid,  password, soft_ap_ssid, soft_ap_password,hostname, stationmode);
    WiFi.disconnect();
    return connectSTA();
}

void WifiManager::restartWifi(){
    WiFi.disconnect();
     if(stationmode){
       bool gotConnection = connectSTA();
       if(!gotConnection){
          _HardSerial.print("restartwifi could not connect ");
          
		      connectAP();
       }
    }else{
         connectAP();
    }
}


bool WifiManager::connectSTA(){
    ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();;
    ssid = secretManager.getSSID();
    hostname = secretManager.getHostName();
    _HardSerial.print("Setting hostname=");
    _HardSerial.println(hostname); 
    //
    // the host name needs to be set before setting 
    // the mode
    //
    const char* hnchar = hostname.c_str();
     int hnl = hostname.length();
     int hnlt = hnl+1;
     char hname[hnlt];
    // hname[hnlt] = '\0';

    snprintf(hname, hnlt, hnchar, 32);
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hname);

    
  //  WiFi.setHostname(hostname.c_str()); //define hostname

//    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
     
   WiFi.begin(const_cast<char*>(ssid.c_str()), const_cast<char*>(password.c_str()));
    bool gotConnection=true;
    uint8_t counter=0;
    bool keepGoing=true;
    while (keepGoing){
        keepGoing=WiFi.status() != WL_CONNECTED;
        delay(1000);
        _HardSerial.print(".");
        counter++;
        if(counter>30){
            keepGoing=false;
            gotConnection=false;
        }
    }

_HardSerial.print("in connectSTA after settmg wifi, ip=");
  _HardSerial.println(WiFi.localIP());

    if(gotConnection){
       
        ipAddress = WiFi.localIP().toString();
        _HardSerial.print(" Connected with ip=");
        _HardSerial.println(ipAddress); 
    }else{
        _HardSerial.print("Error connecting to wifi router ssid=");
         _HardSerial.println(ssid);
    }
    if (!MDNS.begin(hostname.c_str())) {
      _HardSerial.println("Error setting up MDNS responder!");
    }else{
        _HardSerial.println("MDNS setup ok");
    }

    return gotConnection;

    
    
}

bool WifiManager::connectAP(){
    //WiFi.mode(WIFI_MODE_AP);
    WiFi.mode(WIFI_MODE_AP);

     soft_ap_ssid = secretManager.getSoftAPSSID();
    if(soft_ap_ssid=="" || soft_ap_ssid=="192.168.4.1"){
        soft_ap_ssid="Pancho";
    }
    soft_ap_password = secretManager.getSoftAPPASS();
    hostname = secretManager.getHostName();;


    _HardSerial.print("Starting connectAP 2 soft_ap_ssid=");
    _HardSerial.println(soft_ap_ssid);
    
    //  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
 //   WiFi.onEvent(OnWiFiEvent);
    apConnected=false;
   // WiFi.setHostname(hostname.c_str());
    //soft_ap_ssid="PanchoPool";
    WiFi.softAP(soft_ap_ssid.c_str(), soft_ap_password.c_str());
    delay(500);
    apAddress = WiFi.softAPIP().toString();
    _HardSerial.print("Ap Mode using Ip Address of ");
    
    _HardSerial.println(apAddress);
    if(apAddress=="192.168.4.1")apConnected=true;
   
   
    return apConnected;
}



WifiManager::~WifiManager() {}
