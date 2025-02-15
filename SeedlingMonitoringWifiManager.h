


#ifndef LIBRARIES_DIGITALSTABLES_SEEDLINGMONITORINGWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_SEEDLINGMONITORINGWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
#include <SeedlingMonitoringData.h>
#include <ArduinoJson.h>

class SeedlingMonitoringWifiManager : public WifiManager{

public:
	SeedlingMonitorData& seedlingMonitorData;
  	
	SeedlingMonitoringWifiManager(HardwareSerial& serial , FS &fs,PCF8563TimeManager& t, Esp32SecretManager& e, SeedlingMonitorData& s) ;
	void start( );
	int uploadDataToDigitalStables();
	void generateWebData(DynamicJsonDocument& json, String s);
    virtual ~SeedlingMonitoringWifiManager();
};
#endif /* LIBRARIES_DIGITALSTABLES_SEEDLINGMONITORINGgOWWIFIMANAGER */