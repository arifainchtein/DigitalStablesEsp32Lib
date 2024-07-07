


#ifndef LIBRARIES_DIGITALSTABLES_CAJALWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_CAJALWFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
#include <CajalData.h>
#include <DataManager.h>

class CajalWifiManager : public WifiManager{



public:
    DataManager& dataManager;
	CajalData& cajalData;
   
	CajalWifiManager(HardwareSerial& serial ,DataManager& d,PCF8563TimeManager& t, Esp32SecretManager& e, CajalData& tf) ;
	void start( );
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();
    virtual ~CajalWifiManager();

private:
	String okString="Ok";

};
#endif /* LIBRARIES_DIGITALSTABLES_ROSIEWIFIMANAGER_H_ */