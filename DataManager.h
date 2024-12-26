


#ifndef LIBRARIES_DIGITALSTABLES_DATAMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_DATAMANAGER_H_
#include "Arduino.h"

#include <GloriaTankFlowPumpData.h>
#include <PanchoTankFlowData.h>
#include <LangleyData.h>
#include <RosieData.h>
#include <DaffodilData.h>

#include <ArduinoJson.h>
class DataManager{

public:
    
   
	DataManager(HardwareSerial& serial) ;
	
     StaticJsonDocument<60000> completeObject;
    void start();
     uint16_t getTotalDataSize();
    uint8_t readKeys();
   void getDeviceData(JsonArray& array);
    void storePancho(PanchoTankFlowData& p);
    void storeRosie(RosieData& p);
    void storeGloria(GloriaTankFlowPumpData& p);
     void storeDaffodil(DaffodilData& p);
    //virtual ~DataManager();
      void generateDaffodilWebData(DaffodilData& p,DynamicJsonDocument& json);
    void generatePanchoTankFlowDataWebData(PanchoTankFlowData& p,JsonObject& json);
	void generateRosieWebData(RosieData& r,JsonObject& json);
    void generateGloriaTankFlowPumpWebData(GloriaTankFlowPumpData& r,JsonObject& json);

protected:
	HardwareSerial& _HardSerial;

private:
   
};
#endif /* LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZEROWWIFIMANAGER */