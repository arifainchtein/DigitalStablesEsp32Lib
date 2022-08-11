/*
 * WifiManager.cpp
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#include <WifiManager.h>


WifiManager::WifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e,  PanchoTankFlowData& tf,PanchoConfigData& p) :
 _HardSerial(serial),timeManager(t),secretManager(e), tankFlowData(tf) ,panchoConfigData(p)  {}


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

void WifiManager::scanNetworks() {
  _HardSerial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  _HardSerial.println("scan done");
  if (n == 0) {
      _HardSerial.println("no networks found");
  } else {
    _HardSerial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      _HardSerial.print(i + 1);
      _HardSerial.print(": ");
      _HardSerial.print(WiFi.SSID(i));
      _HardSerial.print(" (");
      _HardSerial.print(WiFi.RSSI(i));
      _HardSerial.print(")");
      _HardSerial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);
}

uint8_t WifiManager::getWifiStatus(){
    return WiFi.status();
}

bool WifiManager::getAPStatus(){
    return apConnected;
}
/*
void WifiManager::configWifi(String s, String p, String sas, String sap, String h ){
    ssid = s;
    password = p;
    soft_ap_ssid = sas;
    soft_ap_password = sap;
    hostname=h;
    secretManager.saveWifiParameters( ssid,  password, soft_ap_ssid, soft_ap_password,hostname);
   // secretManager.saveConfigData(float fieldId, stationName );
    WiFi.disconnect();
    connect();
}
*/
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
    if(soft_ap_ssid=="")soft_ap_ssid="192.168.4.1";
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
       connectSTA();
    }else{
         connectAP();
    }
}


bool WifiManager::connectSTA(){
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
     WiFi.setHostname(hostname.c_str()); //define hostname
  
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
    
    return gotConnection;

    
    
}

bool WifiManager::connectAP(){
    WiFi.mode(WIFI_MODE_AP);
    
    //  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
 //   WiFi.onEvent(OnWiFiEvent);
    apConnected=false;
    WiFi.setHostname(hostname.c_str());
    WiFi.softAP(soft_ap_ssid.c_str(), soft_ap_password.c_str());
    apAddress = WiFi.softAPIP().toString();
    if(apAddress=="192.168.4.1")apConnected=true;
    /*
    bool gotConnection=true;
    uint8_t counter=0;
    bool keepGoing=true;
    while (keepGoing){
        keepGoing=WiFi.status() != WL_CONNECTED;
        delay(500);
        _HardSerial.print(".");
        counter++;
        if(counter>10){
            keepGoing=false;
            gotConnection=false;
        }
    }


    if(gotConnection){
       
        apAddress = WiFi.softAPIP().toString();
        _HardSerial.print(" after got connectin apConnected=");
        _HardSerial.println(this->apConnected);
        if(apAddress=="192.168.4.1")apConnected=true;
        _HardSerial.println("Access Point Enabled connected.");
        _HardSerial.println("Access Point IP address: ");
        _HardSerial.println(apAddress); 
    }else{
        _HardSerial.print("Error connecting to wifi router ssid=");
         _HardSerial.println(ssid);
    }
    */
    return apConnected;
}



WifiManager::~WifiManager() {}
