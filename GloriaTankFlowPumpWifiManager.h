


#ifndef LIBRARIES_DIGITALSTABLES_GLORIATANKFLOWPUMPWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_GLORIATANKFLOWPUMPWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
#include <GloriaTankFlowPumpData.h>

class GloriaTankFlowPumpWifiManager : public WifiManager{



public:
	GloriaTankFlowPumpData& gloriaTankFlowPumpData;
    GloriaConfigData& gloriaConfigData;
	
	GloriaTankFlowPumpWifiManager(HardwareSerial& serial , PCF8563TimeManager& t, Esp32SecretManager& e, GloriaTankFlowPumpData& tf, GloriaConfigData& p) ;
	void start( );
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();
    virtual ~GloriaTankFlowPumpWifiManager();
};
#endif /* LIBRARIES_DIGITALSTABLES_GLORIATANKFLOWPUMPWIFIMANAGER_H_ */