


#ifndef LIBRARIES_DIGITALSTABLES_DAFFODILWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_DAFFODILWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
// #include <DaffodilData.h>
//#include <RosieData.h>
#include <DigitalStablesData.h>
#include <atomic>
#include <functional>

class DaffodilWifiManager : public WifiManager{



public:
	DigitalStablesData& digitalStablesData;
    DigitalStablesConfigData& digitalStablesConfigData;
	
	DaffodilWifiManager(HardwareSerial& serial , FS &fs, PCF8563TimeManager& t, Esp32SecretManager& e, DigitalStablesData& tf, DigitalStablesConfigData& p) ;
	void start( );
	void setWifiActiveSwitchStatus(bool b);
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();
	// Power panel of the web page (2026-10-14): /DaffodilServlet?formName=GetPowerData. Filled by
	// the sketch, which holds the battery ledger and vital signs. Runs in the async_tcp task.
	void setPowerDataProvider(std::function<void(JsonDocument&)> provider) { powerDataProvider = provider; }

    virtual ~DaffodilWifiManager();

private:
	String okString="Ok";
	std::function<void(JsonDocument&)> powerDataProvider;
    std::atomic<bool> _isValid{true};

};
#endif /* LIBRARIES_DIGITALSTABLES_DAFFODILWIFIMANAGER_H_ */