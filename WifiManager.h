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

class WifiManager{


protected:
	HardwareSerial& _HardSerial;
    RTCInfoRecord currentTimerRecord, lastReceptionRTCInfoRecord;
    PCF8563TimeManager& timeManager;
    Esp32SecretManager& secretManager;
    TankFlowData* tankFlowData;
    PanchoConfigData* panchoConfigData;


public:
	WifiManager(HardwareSerial& serial , PCF8563TimeManager& t, Esp32SecretManager& e, TankFlowData& t, PanchoConfigData& p) ;
	void start( );
    void setCurrentStatusData(RTCInfoRecord c, RTCInfoRecord l);
    void setCurrentSSID(char* c);
    void setSensorString(String s);
    String getApAddress();
    String getSoft_ap_ssid();
    String getIpAddress();
    double getRSSI();
    void checkClient();
	virtual ~WifiManager();
private:
    String getIndexPage();
};

#endif /* LIBRARIES_DIGITALSTABLES_WIFIMANAGER_H_ */