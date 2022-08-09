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

class WifiManager{


protected:
	HardwareSerial& _HardSerial;
    RTCInfoRecord currentTimerRecord, lastReceptionRTCInfoRecord;
    PCF8563TimeManager& timeManager;
    Esp32SecretManager& secretManager;
    PanchoTankFlowData& tankFlowData;
    PanchoConfigData& panchoConfigData;


public:
	WifiManager(HardwareSerial& serial , PCF8563TimeManager& t, Esp32SecretManager& e, PanchoTankFlowData& tf, PanchoConfigData& p) ;
	void start( );
    void setCurrentSSID(String s);
    void setSensorString(String s);
    String getApAddress();
    String getMacAddress();
    String getSoft_ap_ssid();
    void setSoft_ap_ssid(String s);
    void restartWifi();
    void scanNetworks();
    String getSoft_ap_password();
    void setSoft_ap_password(String s);
    uint8_t getWifiStatus();
    String getTeleonomeData(String url, bool b);
    String getIpAddress();
    double getRSSI();
    //void checkClient();
	String getHostName();
    void setHostName(String s);
    String getSSID();
    void setSSID(String s);
    bool getStationMode( );
    void setCurrentTimerRecord(RTCInfoRecord currentTimerRecord);
    bool configWifiAP(String sas, String sap, String h);
    bool configWifiSTA(String s, String p, String h);
   // void configWifi(String s, String p, String sas, String sap, String h);
    bool getAPStatus();

    virtual ~WifiManager();
    
private:

    String getIndexPage();
    void connect();
    bool connectAP();
    bool connectSTA();
    bool apConnected;
    
};

#endif /* LIBRARIES_DIGITALSTABLES_WIFIMANAGER_H_ */