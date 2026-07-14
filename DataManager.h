

#ifndef LIBRARIES_DIGITALSTABLES_DATAMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_DATAMANAGER_H_
#include "Arduino.h"
#include <GloriaTankFlowPumpSerializer.h>
#include <DigitalStablesDataSerializer.h>
#include <SeedlingMonitorDataSerializer.h>
#include "ChinampaData.h"
#include <GloriaTankFlowPumpData.h>
#include <LangleyData.h>
#include <DaffodilData.h>
#include <TimeUtils.h>
#include <ChinampaDataSerializer.h>
#include <CommaRecord.h>
#include <CommaRecordSerializer.h>
#include <LangleyDataSerializer.h>
#include <ArduinoJson.h>
#include <DigitalStablesData.h>
#include <SeedlingMonitoringData.h>
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
  void storeSeedlingMonitorData(SeedlingMonitorData &p);
  void storeChinampaData(ChinampaData &p);

  void generateGloriaTankFlowPumpWebData(GloriaTankFlowPumpData &r, DynamicJsonDocument &json);
  void generateDigitalStablesData(DigitalStablesData &p, DynamicJsonDocument &json);
  void generateSeedlingMonitorData(SeedlingMonitorData &p, DynamicJsonDocument &json);
  void generateChinampaData(ChinampaData &p, DynamicJsonDocument &json);
  void processGloriaQueue();
  void processDigitalStablesDataQueue();
  void processSeedlingMonitorDataQueue();
  void processChinampaDataQueue();
  void storeCommaRecord(CommaRecord &r);
  void processCommaRecordQueue();
  void clearAllCommaRecords();
  void storeLangleyData(LangleyData &p);
  void processLangleyQueue();

  int getDSDStoredCount();
  int getSeedlingStoredCount();
  bool readAllDSDData(DigitalStablesData* dataArray, int maxSize, int& actualSize);
  bool readAllSeedlingMonitorData(SeedlingMonitorData* dataArray, int maxSize, int& actualSize);
  void updateDSDStoredCount(int count) ;
  int storeDSDData(DigitalStablesData& data);
  void updateSeedlingStoredCount(int count) ;
  void updateChinampaStoredCount(int count) ;
  
  
  void printAllDSDData();
  void clearAllDSDData() ;
  void clearAllChinampaData() ;
  
  void printDigitalStablesData(const DigitalStablesData& data);
  void printSeedlingStoreData(const SeedlingMonitorData& data);
  void printChinampaData(const ChinampaData& data);

  void exportDSDCSV() ;
  
protected:


private:
  HardwareSerial& _HardSerial; 
  FS& _fs; 
  bool _initialized;
  const char* DSD_DATA_FILE = "/digitalstables.dat";
  const char* DSD_COUNT_FILE = "/dscount.txt";
  const char* CHINAMPA_DATA_FILE = "/chinampa.dat";
  const char* CHINAMPA_COUNT_FILE = "/chinampacount.txt";
  
  const char* SEEDLING_DATA_FILE = "/seedling.dat";
  const char* SEEDLING_COUNT_FILE = "/seedlingcount.txt";
 
  struct DSDIndex {
    int head; // Index of the oldest record
    int tail; // Index of the next available slot
    int count; // Current number of records
};

struct SeedlingIndex {
    int head; // Index of the oldest record
    int tail; // Index of the next available slot
    int count; // Current number of records
};

  #define MAXIMUM_STORED_RECORDS 2000
  #define DSD_INDEX_FILE "/dsd_index.dat"
  #define SEEDLING_INDEX_FILE "/seedling_index.dat"

  boolean debug = false;
  #define MAX_QUEUE_SIZE 20        // Chinampa, Gloria, Seedling queues
  #define MAX_DSD_QUEUE_SIZE 50   // DigitalStablesData — larger to avoid losing records
  #define MAX_COMMA_QUEUE_SIZE 50  // CommaRecord — solar devices may send long batches
  GloriaTankFlowPumpSerializer gloriaTankFlowPumpSerializer;
  DigitalStablesDataSerializer digitalStablesDataSerializer;
  SeedlingMonitorDataSerializer seedlingMonitorDataSerializer;
  ChinampaDataSerializer chinampaDataSerializer;
  CommaRecordSerializer commaRecordSerializer;
  LangleyDataSerializer langleyDataSerializer;
  
  ;
  // Queue for DigitalStablesData
  struct DSQueueElement
  {
    DigitalStablesData data;
  };

  struct SeedQueueElement
  {
    SeedlingMonitorData data;
  };
  // Queue for GloriaTankAndFlowData
  struct GloriaQueueElement
  {
    GloriaTankFlowPumpData data;
  };

    struct ChinampaQueueElement
  {
    ChinampaData data;
  };

  struct CommaQueueElement
  {
    CommaRecord data;
  };

  struct LangleyQueueElement
  {
    LangleyData data;
  };

  DSQueueElement dsQueue[MAX_DSD_QUEUE_SIZE];
  GloriaQueueElement gloriaQueue[MAX_QUEUE_SIZE];
  SeedQueueElement seedQueue[MAX_QUEUE_SIZE];
  ChinampaQueueElement chinampaQueue[MAX_QUEUE_SIZE];
  CommaQueueElement commaQueue[MAX_COMMA_QUEUE_SIZE];
  LangleyQueueElement langleyQueue[MAX_QUEUE_SIZE];

  struct QueueCounters
  {
    int front = 0;
    int rear = -1;
    int itemCount = 0;
  };

  QueueCounters dsCounters;
  QueueCounters gloriaCounters;
  QueueCounters seedCounters;
  QueueCounters chinampaCounters;
  QueueCounters commaCounters;
  QueueCounters langleyCounters;


  void enqueueSeedlingData(SeedlingMonitorData data);
  void enqueueChinampaData(ChinampaData data);
  void enqueueCommaRecord(CommaRecord data);
  void enqueueLangleyData(LangleyData data);

  void enqueueDSData(DigitalStablesData data);
  void enqueueGloriaData(GloriaTankFlowPumpData data);
  void initializeDSDFile();
  void initializeChinampaFile();
  void initializeSeedlingMonitorFile();

  bool saveDSDIndex(const DSDIndex& index) ;
  bool loadDSDIndex(DSDIndex& index);
  bool loadSeedlingIndex(SeedlingIndex& index);
  
};
#endif /* LIBRARIES_DIGITALSTABLES_PANCHOVISUALIZEROWWIFIMANAGER */