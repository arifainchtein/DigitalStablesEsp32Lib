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
    void setCurrentStatusData(RTCInfoRecord c, RTCInfoRecord l, bool opmode);
    void setCurrentSSID(String s);
    void setSensorString(String s);
    String getApAddress();
    String getMacAddress();
    String getSoft_ap_ssid();
    void setSoft_ap_ssid(String s);
    void restartWifi();

    String getSoft_ap_password();
    void setSoft_ap_password(String s);
    uint8_t getWifiStatus();
   
    String getIpAddress();
    double getRSSI();
    //void checkClient();
	String getHostName();
    void setHostName(String s);
    String getSSID();
    void setSSID(String s);
    void configWifi(String s, String p, String sas, String sap, String h);
    virtual ~WifiManager();
    
private:
    String getIndexPage();
    void connect();
    
};

#endif /* LIBRARIES_DIGITALSTABLES_WIFIMANAGER_H_ */