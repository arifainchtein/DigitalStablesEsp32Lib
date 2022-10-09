


#ifndef LIBRARIES_DIGITALSTABLES_PANCHOTANKANDFLOWWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_PANCHOTANKANDFLOWWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>

class PanchoTankFlowWifiManager : public WifiManager{



public:
	PanchoTankFlowData& panchoTankFlowData;
    PanchoConfigData& panchoConfigData;
	
	PanchoTankFlowWifiManager(HardwareSerial& serial , PCF8563TimeManager& t, Esp32SecretManager& e, PanchoTankFlowData& tf, PanchoConfigData& p) ;
	void start( );
    virtual ~PanchoTankFlowWifiManager();
};
#endif /* LIBRARIES_DIGITALSTABLES_PANCHOTANKANDFLOWWIFIMANAGER_H_ */