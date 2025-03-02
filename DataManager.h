

#ifndef LIBRARIES_DIGITALSTABLES_DATAMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_DATAMANAGER_H_
#include "Arduino.h"
#include <GloriaTankFlowPumpSerializer.h>
#include <DigitalStablesDataSerializer.h>
#include <GloriaTankFlowPumpData.h>
#include <LangleyData.h>
#include <DaffodilData.h>
#include <TimeUtils.h>

#include <ArduinoJson.h>
#include <DigitalStablesData.h>

#include <FS.h>

class DataManager
{

public:
  DataManager(HardwareSerial& serial, FS& fs);
  StaticJsonDocument<60000> completeObject;
  void start();
  //uint16_t getTotalDataSize();
  void storeGloria(GloriaTankFlowPumpData &p);
  void storeDigitalStablesData(DigitalStablesData &p);
  void generateGloriaTankFlowPumpWebData(GloriaTankFlowPumpData &r, DynamicJsonDocument &json);
  void generateDigitalStablesData(DigitalStablesData &p, DynamicJsonDocument &json);
  void processGloriaQueue();
  void processDigitalStablesDataQueue();

  int getDSDStoredCount();
  bool readAllDSDData(DigitalStablesData* dataArray, int maxSize, int& actualSize);
  void updateDSDStoredCount(int count) ;
  int storeDSDData(DigitalStablesData& data);
  void printAllDSDData();
  void clearAllDSDData() ;
  void printDigitalStablesData(const DigitalStablesData& data);
  void exportDSDCSV() ;
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


private:
  HardwareSerial& _HardSerial; 
  FS& _fs; 
  bool _initialized;
  const char* DSD_DATA_FILE = "/digitalstables.dat";
  const char* DSD_COUNT_FILE = "/dscount.txt";

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