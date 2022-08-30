


#ifndef LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZERWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZERWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>

class PanchoVisualizerWifiManager : public WifiManager{



public:
	PanchoVisualizerWifiManager(HardwareSerial& serial , PCF8563TimeManager& t, Esp32SecretManager& e, PanchoTankFlowData& tf, PanchoConfigData& p) ;
	void start( );
    virtual ~PanchoVisualizerWifiManager();
};
#endif /* LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZEROWWIFIMANAGER */