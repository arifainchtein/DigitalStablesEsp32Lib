


#ifndef LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZERWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZERWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
#include <CajalData.h>
#include <RosieData.h>
class PanchoVisualizerWifiManager : public WifiManager{



public:
	CajalData& cajalData;
    PanchoConfigData& panchoConfigData;
	
	PanchoVisualizerWifiManager(HardwareSerial& serial , FS &fs,PCF8563TimeManager& t, Esp32SecretManager& e, CajalData& tf, PanchoConfigData& p) ;
	void start( );
	void generateWebData(DynamicJsonDocument& json, String s);
	void generateRosieWebData(RosieData& r,DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();
	int uploadRosieDataToDigitalStables(RosieData& r);
    virtual ~PanchoVisualizerWifiManager();
	
};
#endif /* LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZEROWWIFIMANAGER */