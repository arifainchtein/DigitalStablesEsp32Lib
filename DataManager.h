

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

  // Non-destructive peek at how many records the next AsyncData command would
  // emit across all six device queues, without draining them. Lets the Pi ask
  // "how many lines are you about to send" before it asks for the data, so it
  // can read exactly that many lines instead of guessing when the response ends.
  int getPendingQueueItemCount();

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

  //
  // Overflow tier (2026-08-04) -- only engaged when a type's small in-RAM
  // QueueCounters queue above is already full, so normal operation never
  // touches flash: routine traffic is drained out of RAM every pulse well
  // before it fills. This is what survives a real backlog (radio outage,
  // Pi down for a while) without silently losing data the moment RAM fills.
  // Drop-oldest circular buffer on flash, one per device type, sized from
  // the 1MB total LittleFS budget agreed 2026-08-04 (see conversation).
  // "dropped" only increments once the overflow store ITSELF is also full --
  // i.e. RAM full AND the much larger flash reserve also exhausted, real and
  // permanent loss, not just "some data is waiting." That's the number worth
  // watching to know whether a type's buffer needs to grow.
  //
  // DSD was built first (the highest-volume "everywhere" type); the other
  // five below reuse the same OverflowIndex shape and shed the same shape of
  // method per type.
  //
  void overflowDSDData(DigitalStablesData &data);
  void processDSDOverflow();
  int getDSDOverflowCount();
  unsigned long getDSDDroppedCount();
  long getDSDFlashHealthDaysRemaining();

  void overflowLangleyData(LangleyData &data);
  void processLangleyOverflow();
  int getLangleyOverflowCount();
  unsigned long getLangleyDroppedCount();
  long getLangleyFlashHealthDaysRemaining();

  void overflowGloriaData(GloriaTankFlowPumpData &data);
  void processGloriaOverflow();
  int getGloriaOverflowCount();
  unsigned long getGloriaDroppedCount();
  long getGloriaFlashHealthDaysRemaining();

  void overflowSeedlingData(SeedlingMonitorData &data);
  void processSeedlingOverflow();
  int getSeedlingOverflowCount();
  unsigned long getSeedlingDroppedCount();
  long getSeedlingFlashHealthDaysRemaining();

  void overflowChinampaData(ChinampaData &data);
  void processChinampaOverflow();
  int getChinampaOverflowCount();
  unsigned long getChinampaDroppedCount();
  long getChinampaFlashHealthDaysRemaining();

  void overflowCommaRecord(CommaRecord &data);
  void processCommaOverflow();
  int getCommaOverflowCount();
  unsigned long getCommaDroppedCount();
  long getCommaFlashHealthDaysRemaining();

  // Called ~once/sec from Annabelle.ino's existing RTC tick handler (cheap --
  // just a member write) so flash-wear rate estimates above have a real
  // wall-clock "now" to work from, without threading a time parameter through
  // every enqueue/overflow call. Deliberately wall-clock (via the PCF8563
  // RTC), not millis() -- millis() resets to 0 on every reboot, which would
  // silently corrupt the rate calculation after any restart.
  void setCurrentEpoch(unsigned long epoch);

  // RAM-tier-only peeks, used alongside each type's *OverflowCount() above to
  // report a combined "available" total in the wire format.
  int getDSDQueueCount();
  int getGloriaQueueCount();
  int getSeedlingQueueCount();
  int getChinampaQueueCount();
  int getCommaQueueCount();
  int getLangleyQueueCount();

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

  //
  // Overflow tier (2026-08-04). One shared index shape for all six device
  // types. "dropped" is cumulative since boot, persisted alongside the index
  // so it survives reboots; deliberately never auto-reset -- the Pi reads it
  // as a running total, same pattern as this codebase's other rolling
  // counters (e.g. the Hypothalamus/Heart/webapp RSS counters).
  //
  struct OverflowIndex {
    int head = 0;            // index of the oldest record
    int tail = -1;            // index of the most recently written record
    int count = 0;           // records currently held (<= capacity)
    unsigned long dropped = 0; // cumulative records evicted because this store was also full
    unsigned long writeCount = 0;     // cumulative overflow writes ever made (flash-wear tracking)
    unsigned long firstWriteEpoch = 0; // wall-clock epoch of this store's very first write, 0 = unset
  };

  // Per-type capacities, starting points sized against the ~1MB total
  // LittleFS budget agreed 2026-08-04 -- weighted toward DSD (the
  // "everywhere" Daffodil type) and Comma (tiny ~32-byte records, so deep
  // history is cheap), lighter for the rest. Byte estimates are hand-counted
  // from each struct's fields, not sizeof() -- confirm against the real
  // values (each struct's size is printed at boot with debug=true) before
  // treating these as precise; they're explicitly meant to be retuned once
  // real per-type "dropped" data comes in from the field.
  //
  //   DSD      1000 x ~250B  =~ 250KB
  //   Langley   600 x ~190B  =~ 114KB
  //   Gloria    250 x ~377B  =~  94KB  (command[64] makes this one pricier/record)
  //   Seedling  300 x ~190B  =~  57KB
  //   Chinampa  300 x ~230B  =~  69KB
  //   Comma    2000 x ~32B   =~  64KB
  //   -----------------------------
  //   total                 =~ 648KB, leaving ~370KB headroom in the 1MB
  //   partition for the existing weather/wifi/secret files, filesystem
  //   overhead, and safety margin.
  //
  #define MAX_OVERFLOW_DSD_RECORDS 1000
  const char* DSD_OVERFLOW_DATA_FILE = "/ov_dsd.dat";
  const char* DSD_OVERFLOW_INDEX_FILE = "/ov_dsd_idx.dat";

  #define MAX_OVERFLOW_LANGLEY_RECORDS 600
  const char* LANGLEY_OVERFLOW_DATA_FILE = "/ov_langley.dat";
  const char* LANGLEY_OVERFLOW_INDEX_FILE = "/ov_langley_idx.dat";

  #define MAX_OVERFLOW_GLORIA_RECORDS 250
  const char* GLORIA_OVERFLOW_DATA_FILE = "/ov_gloria.dat";
  const char* GLORIA_OVERFLOW_INDEX_FILE = "/ov_gloria_idx.dat";

  #define MAX_OVERFLOW_SEEDLING_RECORDS 300
  const char* SEEDLING_OVERFLOW_DATA_FILE = "/ov_seedling.dat";
  const char* SEEDLING_OVERFLOW_INDEX_FILE = "/ov_seedling_idx.dat";

  #define MAX_OVERFLOW_CHINAMPA_RECORDS 300
  const char* CHINAMPA_OVERFLOW_DATA_FILE = "/ov_chinampa.dat";
  const char* CHINAMPA_OVERFLOW_INDEX_FILE = "/ov_chinampa_idx.dat";

  #define MAX_OVERFLOW_COMMA_RECORDS 2000
  const char* COMMA_OVERFLOW_DATA_FILE = "/ov_comma.dat";
  const char* COMMA_OVERFLOW_INDEX_FILE = "/ov_comma_idx.dat";

  // Flash-wear assumptions -- both are conservative, typical-case numbers for
  // ESP32-module NOR flash, not measured against Annabelle's actual chip.
  // Worth confirming against the real flash datasheet if this estimate ever
  // needs to be trusted precisely rather than as a rough early-warning signal.
  #define FLASH_ERASE_BLOCK_SIZE 4096
  #define FLASH_RATED_ERASE_CYCLES 100000UL

  unsigned long _currentEpoch = 0;

  // load/save are shared (the index struct's shape is identical for every
  // type); the store/drain/getter logic per type stays explicit, one
  // function per type per operation, matching this file's existing
  // convention (see e.g. process*Queue()/enqueue*Data() above -- six
  // near-identical functions each, not a templated/generic one).
  bool loadOverflowIndex(const char* indexFile, OverflowIndex& index);
  bool saveOverflowIndex(const char* indexFile, const OverflowIndex& index);
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