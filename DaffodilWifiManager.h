


#ifndef LIBRARIES_DIGITALSTABLES_DAFFODILWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_DAFFODILWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
#include <DaffodilData.h>


class DaffodilWifiManager : public WifiManager{



public:
	DaffodilData& daffodilData;
    DaffodilConfigData& daffodilConfigData;
	
	DaffodilWifiManager(HardwareSerial& serial , PCF8563TimeManager& t, Esp32SecretManager& e, DaffodilData& tf, DaffodilConfigData& p) ;
	void start( );
	void setWifiActiveSwitchStatus(bool b);
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();
    virtual ~DaffodilWifiManager();

private:
	String okString="Ok";

};
#endif /* LIBRARIES_DIGITALSTABLES_DAFFODILWIFIMANAGER_H_ */