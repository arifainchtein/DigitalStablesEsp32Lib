/*
 * WifiManager.h
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_WIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_WIFIMANAGER_H_
#include "Arduino.h"
#include <RTCInfoRecord.h>
#include <PCF8563TimeManager.h>
#include <Esp32SecretManager.h>
#include <PanchoTankFlowData.h>
#include <WiFi.h>
#include <time.h>

#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Ping.h>
class WifiManager{



protected:
	HardwareSerial& _HardSerial;
    RTCInfoRecord currentTimerRecord, lastReceptionRTCInfoRecord;
    PCF8563TimeManager& timeManager;
    Esp32SecretManager& secretManager;
    bool wifiActiveSwitchStatus=false;

    JsonArray availablessids;
    String ssid;
    String ssids;
    long  totpcode;    
    String password;
    String soft_ap_ssid ;
    String timezone;    
    String soft_ap_password;
    AsyncWebServer asyncWebServer;
    HTTPClient http;
    String apAddress;
    String ipAddress;
    String sensorString;
    String serialNumber;
    String tiumezone;
    uint8_t delayT=10;
    float fieldId;
    String hostname;
    bool stationmode;
    bool lora;
    bool internetAvailable;
    int internetPingTime;
    
    void connect();
    bool connectAP();
    bool connectSTA();
    bool apConnected;
    void createDeneWord(JsonObject& deneWord, String name, String value, String valueType);
    void createDeneWord(JsonObject& deneWord, String name, float value, String valueType);
    void createDeneWord(JsonObject& deneWord, String name, int value, String valueType);
    void createDeneWord(JsonObject& deneWord, String name, long value, String valueType);
    void internetConnectionAvailable();
    bool setTimeFromInternet();
    
    virtual void generateWebData(DynamicJsonDocument& json, String s)=0;
   

public:
	WifiManager(HardwareSerial& serial , PCF8563TimeManager& t, Esp32SecretManager& e) ;
	virtual void start( )=0;
    void stop( );
     bool getWifiStatus();
    virtual int uploadDataToDigitalStables()=0;
    void setCurrentToTpCode(long t);
    void setCurrentSSID(String s);
    void setSensorString(String s);
    String getApAddress();
    String getMacAddress();
    String getSoft_ap_ssid();
    void setSoft_ap_ssid(String s);
    void restartWifi();
    String scanNetworks();
    String getSoft_ap_password();
    void setSoft_ap_password(String s);
   // uint8_t getWifiStatus();
    String getTeleonomeData(String url, bool b);
    String getIpAddress();
    double getRSSI();
    //void checkClient();
	String getHostName();
    void setHostName(String s);
    bool getInternetAvailable();
    String getSSID();
    void setSSID(String s);
    void setSerialNumber(String s);
    void setLora(bool b);
    void setTimezone(String timezone);
    bool getStationMode( );
    void setCurrentTimerRecord(RTCInfoRecord currentTimerRecord);
    bool configWifiAP(String sas, String sap, String h);
    bool configWifiSTA(String s, String p, String h);
   // void configWifi(String s, String p, String sas, String sap, String h);
    bool getAPStatus();
   

    virtual ~WifiManager();
    
   
    
};

#endif /* LIBRARIES_DIGITALSTABLES_WIFIMANAGER_H_ */