


#ifndef LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZERWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZERWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>

class PanchoVisualizerWifiManager : public WifiManager{



public:
	PanchoTankFlowData& panchoTankFlowData;
    PanchoConfigData& panchoConfigData;
	
	PanchoVisualizerWifiManager(HardwareSerial& serial , PCF8563TimeManager& t, Esp32SecretManager& e, PanchoTankFlowData& tf, PanchoConfigData& p) ;
	void start( );
	void generateWebData(DynamicJsonDocument& json, String s);
	bool uploadDataToDigitalStables();
    virtual ~PanchoVisualizerWifiManager();
	
};
#endif /* LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZEROWWIFIMANAGER */