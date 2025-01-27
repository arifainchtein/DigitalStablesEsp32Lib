

#ifndef LIBRARIES_DIGITALSTABLES_DATAMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_DATAMANAGER_H_
#include "Arduino.h"
#include <GloriaTankFlowPumpSerializer.h>
#include <DigitalStablesDataSerializer.h>
#include <GloriaTankFlowPumpData.h>
#include <PanchoTankFlowData.h>
#include <LangleyData.h>
#include <RosieData.h>
#include <DaffodilData.h>

#include <ArduinoJson.h>
#include <DigitalStablesData.h>
class DataManager
{

public:
  DataManager(HardwareSerial &serial);
  StaticJsonDocument<60000> completeObject;
  void start();
  uint16_t getTotalDataSize();
  void storeGloria(GloriaTankFlowPumpData &p);
  void storeDigitalStablesData(DigitalStablesData &p);
  void generateGloriaTankFlowPumpWebData(GloriaTankFlowPumpData &r, DynamicJsonDocument &json);
  void generateDigitalStablesData(DigitalStablesData &p, DynamicJsonDocument &json);
  void processGloriaQueue();
  void processDigitalStablesDataQueue();

  // uint8_t readKeys();
  // void getDeviceData(JsonArray &array);
  // void storePancho(PanchoTankFlowData &p);
  // void storeRosie(RosieData &p);
  // virtual ~DataManager();
  // void storeDaffodil(DaffodilData &p);

  // void generateDaffodilWebData(DaffodilData &p, DynamicJsonDocument &json);
  // void generatePanchoTankFlowDataWebData(PanchoTankFlowData &p, JsonObject &json);
  // void generateRosieWebData(RosieData &r, JsonObject &json);

protected:
  HardwareSerial &_HardSerial;

private:
  boolean debug = false;
  #define MAX_QUEUE_SIZE 20
  GloriaTankFlowPumpSerializer gloriaTankFlowPumpSerializer;
  DigitalStablesDataSerializer digitalStablesDataSerializer;
  ;
  // Queue for DigitalStablesData
  struct DSQueueElement
  {
    DigitalStablesData data;
  };

  // Queue for GloriaTankAndFlowData
  struct GloriaQueueElement
  {
    GloriaTankFlowPumpData data;
  };

  DSQueueElement dsQueue[MAX_QUEUE_SIZE];
  GloriaQueueElement gloriaQueue[MAX_QUEUE_SIZE];

  struct QueueCounters
  {
    int front = 0;
    int rear = -1;
    int itemCount = 0;
  };

  QueueCounters dsCounters;
  QueueCounters gloriaCounters;

  void enqueueDSData(DigitalStablesData data);
  void enqueueGloriaData(GloriaTankFlowPumpData data);
};
#endif /* LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZEROWWIFIMANAGER */