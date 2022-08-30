


#ifndef LIBRARIES_DIGITALSTABLES_SEEDLINGMONITORINGWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_SEEDLINGMONITORINGWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
#include <ArduinoJson.h>

class SeedlingMonitoringWifiManager : public WifiManager{

private:
void prepareData(DynamicJsonDocument& json);

public:
	SeedlingMonitoringWifiManager(HardwareSerial& serial , PCF8563TimeManager& t, Esp32SecretManager& e, PanchoTankFlowData& tf, PanchoConfigData& p) ;
	void start( );
    virtual ~SeedlingMonitoringWifiManager();
};
#endif /* LIBRARIES_DIGITALSTABLES_SEEDLINGMONITORINGgOWWIFIMANAGER */