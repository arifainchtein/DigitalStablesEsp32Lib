


#ifndef LIBRARIES_DIGITALSTABLES_DATAMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_DATAMANAGER_H_
#include "Arduino.h"
class DataManager{

public:
    uint8_t keycounter=0;

	DataManager(HardwareSerial& serial) ;
	
    void store(String name, const TStore& store);
    void start();
     uint16_t getTotalDataSize();
    uint8_t readKeys();
   void DataManager::getDeviceData(JsonArray& array);
}
    virtual ~DataManager();

private:
   
    void generatePanchoTankFlowDataWebData(PanchoTankFlowData p,DynamicJsonDocument& json);
	void generateRosieWebData(RosieData& r,DynamicJsonDocument& json);
    void generateGloriaTankFlowPumpWebData(GloriaTankFlowPumpData& r,DynamicJsonDocument& json);
    
	int readHeader(String name);
};
#endif /* LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZEROWWIFIMANAGER */