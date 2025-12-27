


#ifndef LIBRARIES_DIGITALSTABLES_ANNABELLEWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_ANNABELLEWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
#include <AnnabelleData.h>
#include <DataManager.h>

class AnnabelleWifiManager : public WifiManager{



public:
    DataManager& dataManager;
	AnnabelleData& annabelleData;
	boolean debug=false;
	AnnabelleWifiManager(HardwareSerial& serial ,FS &fs,DataManager& d,PCF8563TimeManager& t, Esp32SecretManager& e, AnnabelleData& tf) ;
	void start( );
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();	
	int uploadDigitalStablesDataToDigitalStables(DigitalStablesData& r);

    virtual ~AnnabelleWifiManager();

private:
	String okString="Ok";

};
#endif /* LIBRARIES_DIGITALSTABLES_ROSIEWIFIMANAGER_H_ */