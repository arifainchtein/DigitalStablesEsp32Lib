


#ifndef LIBRARIES_DIGITALSTABLES_ROSIEWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_ROSIEWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
#include <RosieData.h>


class RosieWifiManager : public WifiManager{



public:
	RosieData& rosieData;
    RosieConfigData& rosieConfigData;
	
	RosieWifiManager(HardwareSerial& serial , FS &fs,PCF8563TimeManager& t, Esp32SecretManager& e, RosieData& tf, RosieConfigData& p) ;
	void start( );
	void setWifiActiveSwitchStatus(bool b);
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();
    virtual ~RosieWifiManager();

private:
	String okString="Ok";

};
#endif /* LIBRARIES_DIGITALSTABLES_ROSIEWIFIMANAGER_H_ */