#include "GloriaTankFlowPumpData.h"
#include "RosieData.h"
#include "PanchoTankFlowData.h"
#include "ChinampaData.h"
#include "DigitalStablesData.h"
#include "DataManager.h"

//JsonObject obj;
// GloriaTankFlowPumpData gloriaTankFlowPumpData;
// RosieData rosieData;
// PanchoTankFlowData panchoaTankFlowData;
// String keys[126];

String sn = "";
 DataManager::DataManager(HardwareSerial& serial, FS& fs) : _HardSerial(serial) , _fs(fs) , _initialized(false) { }


void DataManager::start() { 
  initializeDSDFile();
  initializeSeedlingMonitorFile();
  _initialized=true;
}


void DataManager::initializeSeedlingMonitorFile() { 
  if (_fs.exists(SEEDLING_DATA_FILE)) {
   _HardSerial.println("in initializeSeedlingMonitorFile,returning because file exists");
    return;
  }

  File file = _fs.open(SEEDLING_DATA_FILE, "w");
  if (!file) {
      _HardSerial.println("in init initializeSeedlingMonitorFile,Failed to initialize data file");
      return;
  }
  // Pre-allocate space for MAXIMUM_STORED_RECORDS
  DigitalStablesData dummy;
  for (int i = 0; i < MAXIMUM_STORED_RECORDS; i++) {
 //     file.write((uint8_t*)&dummy, sizeof(DigitalStablesData));
  }
  file.close();
}

void DataManager::initializeDSDFile() { 
  if (_fs.exists(DSD_DATA_FILE)) {
  if(debug) _HardSerial.println("in initializeDSDFile,returning because file exists");
    return;
  }

  File file = _fs.open(DSD_DATA_FILE, "w");
  if (!file) {
     if(debug) _HardSerial.println("in init,Failed to initialize data file");
      return;
  }
  // Pre-allocate space for MAXIMUM_STORED_RECORDS
  DigitalStablesData dummy;
  for (int i = 0; i < MAXIMUM_STORED_RECORDS; i++) {
 //     file.write((uint8_t*)&dummy, sizeof(DigitalStablesData));
  }
  file.close();
}

void DataManager::initializeChinampaFile() {
  if (_fs.exists(CHINAMPA_DATA_FILE)) {
   if(debug)_HardSerial.println("in initializeChinampaFile,returning because file exists");
    return;
  }

  File file = _fs.open(CHINAMPA_DATA_FILE, "w");
  if (!file) {
      if(debug)_HardSerial.println("in init,Failed to initialize data file");
      return;
  }
  // Pre-allocate space for MAXIMUM_STORED_RECORDS
  ChinampaData dummy;
  for (int i = 0; i < MAXIMUM_STORED_RECORDS; i++) {
 //     file.write((uint8_t*)&dummy, sizeof(DigitalStablesData));
  }
  file.close();
}

bool DataManager::loadSeedlingIndex(SeedlingIndex& index) {
    if (!_fs.exists(SEEDLING_INDEX_FILE)) {
        index.head = 0;
        index.tail = 0;
        index.count = 0;
        return true;
    }

    File file = _fs.open(SEEDLING_INDEX_FILE, "r");
    if (!file) return false;

    file.read((uint8_t*)&index, sizeof(SeedlingIndex));
    file.close();
    return true;
}

bool DataManager::loadDSDIndex(DSDIndex& index) {
    if (!_fs.exists(DSD_INDEX_FILE)) {
        index.head = 0;
        index.tail = 0;
        index.count = 0;
        return true;
    }

    File file = _fs.open(DSD_INDEX_FILE, "r");
    if (!file) return false;

    file.read((uint8_t*)&index, sizeof(DSDIndex));
    file.close();
    return true;
}

bool DataManager::saveDSDIndex(const DSDIndex& index) {
    File file = _fs.open(DSD_INDEX_FILE, "w");
    if (!file) return false;

    file.write((uint8_t*)&index, sizeof(DSDIndex));
    file.close();
    return true;
}

int DataManager::getDSDStoredCount() {
       if (!_initialized) return 0;
    
    if (_fs.exists(DSD_COUNT_FILE)) {
        File file = _fs.open(DSD_COUNT_FILE, "r");
        if (!file) return 0;
        String countStr = file.readString();
        file.close();
        return countStr.toInt();
    }
    return 0;
}

int DataManager::getSeedlingStoredCount() {
    if (!_initialized) return 0;
    if (_fs.exists(SEEDLING_COUNT_FILE)) {
        File file = _fs.open(SEEDLING_COUNT_FILE, "r");
        if (!file) return 0;
        String countStr = file.readString();
        file.close();
        return countStr.toInt();
    }
    return 0;
}
void DataManager::updateSeedlingStoredCount(int count) {
    if (!_initialized) return;
    File file = _fs.open(SEEDLING_COUNT_FILE, "w");
    if (!file) return;
    file.println(count);
    file.close();
}


// int DataManager::storeSeedlingMonitorData(SeedlingMonitorData& data) {
//   if (!_initialized){
//     _HardSerial.printf("Data Manager Not initialized in storesdeedling");
//     return -1;
//    } 
//    int count = getSeedlingStoredCount();
   
//     // Open file in append mode
//     File file = _fs.open(SEEDLING_DATA_FILE, "a");
//     if(!file) {
//         _HardSerial.println("Failed to open file for writing");
//         return -2;
//     }
  
//   // Write the struct data
//     file.write((uint8_t*)&data, sizeof(SeedlingMonitorData));
//     file.close();
//     if(debug)_HardSerial.println("line 152");
//     // Update count
//     updateSeedlingStoredCount(count + 1);
//      count = getSeedlingStoredCount();
//  if(debug)_HardSerial.print("line 156, count=");
//  if(debug)_HardSerial.println(count);
 
//     if(debug)_HardSerial.printf("Stored SeedlingMonitor entry %d\n", count);
//     return count;
// }

void DataManager::updateDSDStoredCount(int count) {
    if (!_initialized) return;
    File file = _fs.open(DSD_COUNT_FILE, "w");
    if (!file) return;
    file.println(count);
    file.close();
}

int DataManager::storeDSDData(DigitalStablesData& data) {
  if (!_initialized){
    _HardSerial.printf("Data Manager Not initialized");
    return -1;
   } 
   int count = getDSDStoredCount();
   
    // Open file in append mode
    File file = _fs.open(DSD_DATA_FILE, "a");
    if(!file) {
        _HardSerial.println("Failed to open file for writing");
        return -2;
    }
  
  // Write the struct data
    file.write((uint8_t*)&data, sizeof(DigitalStablesData));
    file.close();
    if(debug)_HardSerial.printf("point 7");
    // Update count
    updateDSDStoredCount(count + 1);
     count = getDSDStoredCount();

    if(debug)_HardSerial.printf("Stored entry %d\n", count);
    return count;
}



/*
int DataManager::storeDSDData(DigitalStablesData& data) {
    if (!_initialized) {
        _HardSerial.println("Data Manager Not initialized");
        return -1;
    }

    DSDIndex index;
    if (!loadDSDIndex(index)) {
        _HardSerial.println("Failed to load index");
        return -2;
    }

    // Open file in read-write mode
    File file = _fs.open(DSD_DATA_FILE, "r+");
    if (!file) {
        _HardSerial.println("in storedata Failed to open file for writing");
        return -3;
    }

    // If buffer is full, overwrite the oldest record (head)
    if (index.count >= MAXIMUM_STORED_RECORDS) {
        // Move head forward (circular buffer)
        index.head = (index.head + 1) % MAXIMUM_STORED_RECORDS;
        index.count--; // Decrement count (will be incremented below)
    }

    // Write the new data at the tail position
    int writePos = index.tail * sizeof(DigitalStablesData);
    file.seek(writePos);
    file.write((uint8_t*)&data, sizeof(DigitalStablesData));

    // Update tail and count
    index.tail = (index.tail + 1) % MAXIMUM_STORED_RECORDS;
    index.count++;

    file.close();

    // Save the updated index
    if (!saveDSDIndex(index)) {
        _HardSerial.println("Failed to save index");
        return -4;
    }

    if (debug) _HardSerial.printf("Stored entry. Count: %d\n", index.count);
    return index.count;
}
    */

/*
bool DataManager::readAllDSDData(DigitalStablesData* dataArray, int maxSize, int& actualSize) {
    if (!_initialized) return false;
    
    if (dataArray == nullptr) {
        if(debug)_HardSerial.println("Invalid array pointer");
        return false;
    }
    
    // Get actual number of stored entries
    actualSize = getDSDStoredCount();
    if (actualSize == 0) {
        if(debug)_HardSerial.println("No data stored");
        return false;
    }
    
    if (actualSize > maxSize) {
        if(debug)_HardSerial.printf("Warning: Only reading %d entries out of %d\n", maxSize, actualSize);
        actualSize = maxSize;
    }
    
    File file = _fs.open(DSD_DATA_FILE, "r");
    if(!file) {
        if(debug)_HardSerial.println("Failed to open file for reading");
        return false;
    }
    
    // Read entries into the array
    int entriesRead = 0;
    while(entriesRead < actualSize && 
          file.read((uint8_t*)&dataArray[entriesRead], sizeof(DigitalStablesData))) {
        entriesRead++;
    }
    
    file.close();
    
    if (entriesRead != actualSize) {
        if(debug)_HardSerial.printf("Warning: Expected %d entries but read %d\n", actualSize, entriesRead);
        actualSize = entriesRead;
    }
    
    return true;
}
    */
bool DataManager::readAllDSDData(DigitalStablesData* dataArray, int maxSize, int& actualSize) {
    if (!_initialized || !dataArray) return false;

    DSDIndex index;
    if (!loadDSDIndex(index)) return false;

    actualSize = (index.count < maxSize) ? index.count : maxSize;
    if (actualSize == 0) return false;

    File file = _fs.open(DSD_DATA_FILE, "r");
    if (!file) return false;

    for (int i = 0; i < actualSize; i++) {
        int readPos = ((index.head + i) % MAXIMUM_STORED_RECORDS) * sizeof(DigitalStablesData);
        file.seek(readPos);
        file.read((uint8_t*)&dataArray[i], sizeof(DigitalStablesData));
    }

    file.close();
    return true;
}

bool DataManager::readAllSeedlingMonitorData(SeedlingMonitorData* dataArray, int maxSize, int& actualSize) {
    if (!_initialized || !dataArray) return false;

    SeedlingIndex index;
    if (!loadSeedlingIndex(index)) return false;

    actualSize = (index.count < maxSize) ? index.count : maxSize;
    if (actualSize == 0) return false;

    File file = _fs.open(SEEDLING_DATA_FILE, "r");
    if (!file) return false;

    for (int i = 0; i < actualSize; i++) {
        int readPos = ((index.head + i) % MAXIMUM_STORED_RECORDS) * sizeof(SeedlingMonitorData);
        file.seek(readPos);
        file.read((uint8_t*)&dataArray[i], sizeof(SeedlingMonitorData));
    }

    file.close();
    return true;
}
  
    


void DataManager::printAllDSDData() {
       if (!_initialized) return;
    
    int count = getDSDStoredCount();
    
    if (count == 0) {
        if(debug)_HardSerial.println("No data stored");
        return;
    }
    
    File file = _fs.open(DSD_DATA_FILE, "r");
    if(!file) {
        if(debug)_HardSerial.println("Failed to open file for reading");
        return;
    }
    
    if(debug)_HardSerial.println("\n=== Stored Digital Stables Data ===");
    if(debug)_HardSerial.printf("Total entries: %d\n", count);
    
    DigitalStablesData readData;
    int entry = 0;
    
    while(file.read((uint8_t*)&readData, sizeof(DigitalStablesData))) {
        if(debug)_HardSerial.printf("\nEntry %d:\n", entry++);
        printDigitalStablesData(readData);
    }
    
    file.close();
    if(debug)_HardSerial.println("=== End of Data ===\n");
}

void DataManager::clearAllDSDData() {
     if (!_initialized) return;
    
    if(_fs.remove(DSD_DATA_FILE)) {
        updateDSDStoredCount(0);
        if(debug)_HardSerial.println("All data cleared");
    } else {
        if(debug)_HardSerial.println("Error clearing data");
    }
}



void DataManager::exportDSDCSV() {
     if (!_initialized) return;
    
    File file = _fs.open(DSD_DATA_FILE, "r");
    if(!file) {
        if(debug)_HardSerial.println("Failed to open file for reading");
        return;
    }
    // Print CSV header
       Serial.println(F("devicename,deviceshortname,groupidentifier,sensor1name,sensor2name,"
                  "serialnumber,devicetype,secondsTime,secondstimestring,dataSamplingSec,"
                  " temperature,rtcBatVolt,opMode,operatingstatus,ledBrightness, rssi, snr,flowRate,totalMilliLitres,flowRate2,totalMilliLitres2,"
                  "tank1PressurePsi,tank2PressurePsi,troughlevelminimumcm,troughlevelmaximumcm,panelVoltage,maximumScepticHeight,measuredHeight,latitude,longitude,altitude,"
                  "batteryVoltage,v50Voltage,batteryCurrent,estimatedRuntime,outdoortemperature,outdoorhumidity,lux, sleeptime,asyncdata,minimumEfficiencyForLed,minimumEfficiencyForWifi,panelCurrent"));

    DigitalStablesData data;
    while(file.read((uint8_t*)&data, sizeof(DigitalStablesData))) {
        // Serial.printf("%s,%s,%s,%s,%s,%s,%ld,%.2f,%.2f,%.2f,%.2f,%.2f,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%ld,%ld,%d,%d\n",
        //     readData.devicename,
        //     readData.deviceshortname,
        //     readData.groupidentifier,
        //     readData.sensor1name,
        //     readData.sensor2name,
        //     readData.deviceTypeId,
        //     readData.secondsTime,
        //     readData.temperature,
        //     readData.flowRate,
        //     readData.flowRate2,
        //     readData.tank1PressurePsi,
        //     readData.tank2PressurePsi,
        //     readData.latitude,
        //     readData.longitude,
        //     readData.altitude,
        //     readData.solarVoltage,
        //     readData.capacitorVoltage,
        //     readData.outdoortemperature,
        //     readData.outdoorhumidity,
        //     readData.lux,
        //     readData.sleepTime,
        //     readData.asyncdata,
	      //     readData.minimumEfficiencyForLed,
	      //     readData.minimumEfficiencyForWifi
        // );

    Serial.print( String(data.devicename));
    Serial.print("," + String(data.deviceshortname));
    Serial.print(", " + String(data.groupidentifier));
    Serial.print(", " + String(data.sensor1name));
    Serial.print("," + String(data.sensor2name));
    Serial.print(", " );
    for(int i = 0; i < 8; i++) {
        Serial.print(data.serialnumberarray[i], HEX);
    }
    
    Serial.print(", " + String(data.deviceTypeId));
    Serial.print("," + String(data.secondsTime));
    Serial.print("," +  TimeUtils::epochToString(data.secondsTime));
    
    Serial.print("," + String(data.dataSamplingSec));
    Serial.print(", " + String(data.temperature));
    Serial.print(", " + String(data.rtcBatVolt));
    Serial.print(", " + String(data.opMode));
    Serial.print(", " + String(data.operatingStatus));
    Serial.print(", " + String(data.ledBrightness));
    Serial.print(", " + String(data.rssi));
    Serial.print("," + String(data.snr));
    
    // Flow data
    Serial.print("," + String(data.flowRate));
    Serial.print("," + String(data.totalMilliLitres));
    Serial.print("," + String(data.flowRate2));
    Serial.print("," + String(data.totalMilliLitres2));
    
    // Tank data
    Serial.print(", " + String(data.tank1PressurePsi));
  //  Serial.print("," + String(data.tank1HeightMeters));
 //   Serial.print(", " + String(data.tank1maxvollit));
    
    Serial.print("," + String(data.tank2PressurePsi));
 //   Serial.print("," + String(data.tank2HeightMeters));
  //  Serial.print(", " + String(data.tank2maxvollit));
    
    Serial.print("," + String(data.troughlevelminimumcm));
    Serial.print("," + String(data.troughlevelmaximumcm));
    Serial.print("," + String(data.panelVoltage));
    Serial.print("," + String(data.maximumScepticHeight));
    Serial.print("," + String(data.measuredHeight));
    // Location data
    Serial.print("," + String(data.latitude) + ", " + String(data.longitude));
    Serial.print(", " + String(data.altitude));
    
    // Environmental data
    Serial.print("," + String(data.batteryVoltage));
    Serial.print(", " + String(data.v50Voltage));
    Serial.print(", " + String(data.batteryCurrent));
    Serial.print(", " + String(data.estimatedRuntime));

    Serial.print("," + String(data.outdoortemperature));
    Serial.print("," + String(data.outdoorhumidity));
    Serial.print("," + String(data.lux));
    
    // System settings
    Serial.print(", " + String(data.sleepTime));
     Serial.print(", " + String(data.asyncdata));
    Serial.print("," + String(data.minimumEfficiencyForLed));
    Serial.print(", " + String(data.minimumEfficiencyForWifi));
    Serial.println("," + String(data.panelCurrent));
    }
    
    file.close();
}
 void DataManager::printChinampaData(const ChinampaData& data) {

   Serial.println("Device Name: " + String(data.devicename));
    Serial.println("Device Short Name: " + String(data.deviceshortname));
    Serial.println("Group Identifier: " + String(data.groupidentifier));
   
    Serial.print("Serial Number: ");
    for(int i = 0; i < 8; i++) {
        Serial.print(data.serialnumberarray[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    Serial.println("Device Type ID: " + String(data.deviceTypeId));
    Serial.println("Seconds Time: " + String(data.secondsTime));
    Serial.println("Data Sampling Sec: " + String(data.dataSamplingSec));
    
    Serial.println("RTC Battery Voltage: " + String(data.rtcBatVolt));
    
    Serial.println("Pump Relay Status: " + String(data.pumprelaystatus));
    Serial.println("Fish Tank Outflow Solenoid Relay Status: " + String(data.fishtankoutflowsolenoidrelaystatus));
    
    Serial.println("minimumFishTankLevel: " + String(data.minimumFishTankLevel));
    Serial.println("maximumFishTankLevel: " + String(data.maximumFishTankLevel));
    Serial.println("fishTankMeasuredHeight: " + String(data.fishTankMeasuredHeight));
    Serial.println("fishTankHeight: " + String(data.fishTankHeight));
    

    Serial.println("alertstatus: " + String(data.alertstatus));
    Serial.println("alertcode: " + String(data.alertcode));
    
    Serial.println("maximumSumpTroughLevel: " + String(data.maximumSumpTroughLevel));
    Serial.println("sumpTroughMeasuredHeight: " + String(data.sumpTroughMeasuredHeight));
    Serial.println("sumpTroughHeight: " + String(data.sumpTroughHeight));
    Serial.println("Alarm Status: " + String(data.sumpTroughHeight));
  
    Serial.println("fishtankoutflowflowRate: " + String(data.fishtankoutflowflowRate));
    Serial.println("pumpflowRate: " + String(data.pumpflowRate));
  
     Serial.println("Microcontroller Temperature: " + String(data.microtemperature));
    Serial.println("RTC Battery Voltage: " + String(data.rtcBatVolt));
   
    Serial.println("RSSI: " + String(data.rssi));
    Serial.println("SNR: " + String(data.snr));
   }

void DataManager::printSeedlingStoreData(const SeedlingMonitorData& data) {

    Serial.println("Device Name: " + String(data.devicename));
    Serial.println("Device Short Name: " + String(data.deviceshortname));
    Serial.println("Group Identifier: " + String(data.groupidentifier));
   
    Serial.print("Serial Number: ");
    for(int i = 0; i < 8; i++) {
        Serial.print(data.serialnumberarray[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    Serial.println("Device Type ID: " + String(data.deviceTypeId));
    Serial.println("Seconds Time: " + String(data.secondsTime));
    Serial.println("Data Sampling Sec: " + String(data.dataSamplingSec));
    Serial.println("Temperature: " + String(data.temperature));
    Serial.println("RTC Battery Voltage: " + String(data.rtcBatVolt));
    Serial.println("Operating Status: " + String(data.operatingStatus));
    
    Serial.println("RSSI: " + String(data.rssi));
    Serial.println("SNR: " + String(data.snr));
    
    
    // Location data
    Serial.println("Location: " + String(data.latitude) + ", " + String(data.longitude));
   
    // Environmental data
    Serial.println("Outdoor Temperature: " + String(data.greenhouseTemp));
    Serial.println("Outdoor Humidity: " + String(data.greenhouseHum));
    
   }

void DataManager::printDigitalStablesData(const DigitalStablesData& data) {
    Serial.println("Device Name: " + String(data.devicename));
    Serial.println("Device Short Name: " + String(data.deviceshortname));
    Serial.println("Group Identifier: " + String(data.groupidentifier));
    Serial.println("Sensor 1 Name: " + String(data.sensor1name));
    Serial.println("Sensor 2 Name: " + String(data.sensor2name));
    
    Serial.print("Serial Number: ");
    for(int i = 0; i < 8; i++) {
        Serial.print(data.serialnumberarray[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    Serial.println("Device Type ID: " + String(data.deviceTypeId));
    Serial.println("Seconds Time: " + String(data.secondsTime));
    Serial.println("Data Sampling Sec: " + String(data.dataSamplingSec));
    Serial.println("Temperature: " + String(data.temperature));
    Serial.println("RTC Battery Voltage: " + String(data.rtcBatVolt));
    Serial.println("Operating Mode: " + String(data.opMode));
    Serial.println("Operating Status: " + String(data.operatingStatus));
    
    String functionname="";
   if(data.currentFunctionValue== FUN_1_FLOW){
    functionname="FUN_1_FLOW";
   }else if(data.currentFunctionValue== FUN_2_FLOW){
    functionname="FUN_2_FLOW";
   }else if(data.currentFunctionValue== FUN_1_FLOW_1_TANK){
    functionname="FUN_1_FLOW_1_TANK";
   }else if(data.currentFunctionValue== FUN_1_TANK){
    functionname="FUN_1_TANK";
   }else if(data.currentFunctionValue== FUN_2_TANK){
    functionname="FUN_2_TANK";
   }else if(data.currentFunctionValue== DAFFODIL_SCEPTIC_TANK){
    functionname="DAFFODIL_SCEPTIC_TANK";
   }else if(data.currentFunctionValue== DAFFODIL_WATER_TROUGH){
    functionname="DAFFODIL_WATER_TROUGH";
   }else if(data.currentFunctionValue== DAFFODIL_TEMP_SOILMOISTURE){
    functionname="DAFFODIL_TEMP_SOILMOISTURE";
   }else if(data.currentFunctionValue== DAFFODIL_LIGHT_DETECTOR){
    functionname="DAFFODIL_LIGHT_DETECTOR";
   }else if(data.currentFunctionValue== VOLTAGE_MONITOR){
    functionname="VOLTAGE_MONITOR";
   }else if(data.currentFunctionValue== DAFFODIL_WATER_TROUGH_TANK1){
    functionname="DAFFODIL_WATER_TROUGH_TANK1";
   }
    Serial.println("Current Function Value: " + functionname);
    
    Serial.println("RSSI: " + String(data.rssi));
    Serial.println("SNR: " + String(data.snr));
     Serial.println("LoRa Active: " + String(data.loraActive));


    Serial.println("trough Level Minimum (cm): " + String(data.troughlevelminimumcm));
    Serial.println("trough Level Maximum (cm): " + String(data.troughlevelmaximumcm));
    Serial.println("Measured Height (cm): " + String(data.measuredHeight));
    Serial.println("Maximum Height Height (cm): " + String(data.maximumScepticHeight));
    // Flow data
    Serial.println("Flow Rate 1: " + String(data.flowRate));
    Serial.println("Total Millilitres 1: " + String(data.totalMilliLitres));
    Serial.println("Flow Rate 2: " + String(data.flowRate2));
    Serial.println("Total Millilitres 2: " + String(data.totalMilliLitres2));
    
    // Tank data
    Serial.println("Tank 1 Pressure (PSI): " + String(data.tank1PressurePsi));
    //Serial.println("Tank 1 Height (m): " + String(data.tank1HeightMeters));
    Serial.println("Tank 1 Max Volume (L): " + String(data.tank1maxvollit));
    
    Serial.println("Tank 2 Pressure (PSI): " + String(data.tank2PressurePsi));
    //Serial.println("Tank 2 Height (m): " + String(data.tank2HeightMeters));
    Serial.println("Tank 2 Max Volume (L): " + String(data.tank2maxvollit));
    
    // Location data
    Serial.println("Location: " + String(data.latitude) + ", " + String(data.longitude));
    Serial.println("Altitude: " + String(data.altitude));
    
    // Environmental data
    Serial.println("Battery Voltage: " + String(data.batteryVoltage));
    Serial.println("V50 Voltage: " + String(data.v50Voltage));
    Serial.println("Battery Current: " + String(data.batteryCurrent));
    Serial.println("Panel Voltage: " + String(data.panelVoltage));
    Serial.println("Panel Current: " + String(data.panelCurrent));
    Serial.println("Outdoor Temperature: " + String(data.outdoortemperature));
    Serial.println("Outdoor Humidity: " + String(data.outdoorhumidity));
    Serial.println("Lux: " + String(data.lux));
    
    // System settings
    Serial.println("Sleep Time (s): " + String(data.sleepTime));
    Serial.println("Min Efficiency LED: " + String(data.minimumEfficiencyForLed));
    Serial.println("Min Efficiency WiFi: " + String(data.minimumEfficiencyForWifi));
     Serial.println("asyncdata: " + String(data.asyncdata));
}



void DataManager::enqueueChinampaData(ChinampaData data){
   if (chinampaCounters.itemCount < MAX_QUEUE_SIZE)
  {
    chinampaCounters.rear = (chinampaCounters.rear + 1) % MAX_QUEUE_SIZE;
    chinampaQueue[chinampaCounters.rear].data = data;
    chinampaCounters.itemCount++;
  } else {
    overflowChinampaData(data);
  }
    if (debug){
    if(debug)_HardSerial.print("after storing  chinampadata  chinampaCounters.itemCount=");
    if(debug)_HardSerial.println(chinampaCounters.itemCount);

    }
}

void DataManager::processChinampaDataQueue()
{
  while (chinampaCounters.itemCount > 0)
  {
    chinampaDataSerializer.pushToSerial(_HardSerial, chinampaQueue[chinampaCounters.front].data);
    chinampaCounters.front = (chinampaCounters.front + 1) % MAX_QUEUE_SIZE;
    chinampaCounters.itemCount--;
  }

  // now clear
  chinampaCounters.front=0;
  chinampaCounters.rear = -1;
  chinampaCounters.itemCount = 0;

}

void DataManager::clearAllChinampaData() {
     if (!_initialized) return;
    
    if(_fs.remove(CHINAMPA_DATA_FILE)) {
        updateChinampaStoredCount(0);
        if(debug)_HardSerial.println("All data cleared");
    } else {
        if(debug)_HardSerial.println("Error clearing data");
    }


}

void DataManager::updateChinampaStoredCount(int count) {
    if (!_initialized) return;
    File file = _fs.open(CHINAMPA_COUNT_FILE, "w");
    file.println(count);
    file.close();
}

 void DataManager::storeChinampaData(ChinampaData &chinampaData){
   sn = "";
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    sn += String(chinampaData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(chinampaData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
  if (debug){
    if(debug)_HardSerial.print("adding a chinampaData  serialNumber=");
    if(debug)_HardSerial.print(sn);
    if(debug)_HardSerial.print(" l=");
    if(debug)_HardSerial.print(sn.length());
    if(debug)_HardSerial.print(" checksum=");
    if(debug)_HardSerial.println(checksum);
    if(debug)_HardSerial.print(" chinampaData.checksum=");
    if(debug)_HardSerial.println(chinampaData.checksum);
  }
  enqueueChinampaData(chinampaData);
  if (chinampaData.checksum == checksum && (sn.length() == 15 || sn.length() == 14))
  {
    if (debug)
    {
      DynamicJsonDocument json(1800);
      generateChinampaData(chinampaData, json);
      serializeJsonPretty(json, _HardSerial);
  }
  else
  {
    if (debug)
      if(debug)_HardSerial.println(" chinampaData rejected pulse serialnumne=");
    if (debug)
      if(debug)_HardSerial.println(sn);
  }
 }
 }


void DataManager::enqueueCommaRecord(CommaRecord data) {
  if (commaCounters.itemCount < MAX_COMMA_QUEUE_SIZE) {
    commaCounters.rear = (commaCounters.rear + 1) % MAX_COMMA_QUEUE_SIZE;
    commaQueue[commaCounters.rear].data = data;
    commaCounters.itemCount++;
  } else {
    overflowCommaRecord(data);
  }
  if (debug) {
    _HardSerial.print("enqueueCommaRecord itemCount=");
    _HardSerial.println(commaCounters.itemCount);
  }
}

void DataManager::storeCommaRecord(CommaRecord &r) {
  String sn = "";
  for (uint8_t i = 0; i < 8; i++) {
    sn += String(r.serialnumber[i], HEX);
  }
  if (debug) {
    _HardSerial.print("storeCommaRecord device=");
    _HardSerial.print(r.devicename);
    _HardSerial.print(" sn=");
    _HardSerial.print(sn);
    _HardSerial.print(" idx=");
    _HardSerial.print(r.index);
    _HardSerial.print("/");
    _HardSerial.println(r.total);
  }
  enqueueCommaRecord(r);
}

void DataManager::processCommaRecordQueue() {
  while (commaCounters.itemCount > 0) {
    commaRecordSerializer.pushToSerial(_HardSerial, commaQueue[commaCounters.front].data);
    commaCounters.front = (commaCounters.front + 1) % MAX_COMMA_QUEUE_SIZE;
    commaCounters.itemCount--;
  }
  commaCounters.front = 0;
  commaCounters.rear = -1;
  commaCounters.itemCount = 0;
}

void DataManager::clearAllCommaRecords() {
  commaCounters.front = 0;
  commaCounters.rear = -1;
  commaCounters.itemCount = 0;
  if (debug) _HardSerial.println("clearAllCommaRecords: queue reset");
}

void DataManager::enqueueLangleyData(LangleyData data) {
  if (langleyCounters.itemCount < MAX_QUEUE_SIZE) {
    langleyCounters.rear = (langleyCounters.rear + 1) % MAX_QUEUE_SIZE;
    langleyQueue[langleyCounters.rear].data = data;
    langleyCounters.itemCount++;
  } else {
    overflowLangleyData(data);
  }
  if (debug) {
    _HardSerial.print("enqueueLangleyData itemCount=");
    _HardSerial.println(langleyCounters.itemCount);
  }
}

void DataManager::storeLangleyData(LangleyData &p) {
  if (debug) {
    _HardSerial.print("storeLangleyData device=");
    _HardSerial.println(p.devicename);
  }
  enqueueLangleyData(p);
}

void DataManager::processLangleyQueue() {
  while (langleyCounters.itemCount > 0) {
    langleyDataSerializer.pushToSerial(_HardSerial, langleyQueue[langleyCounters.front].data);
    langleyCounters.front = (langleyCounters.front + 1) % MAX_QUEUE_SIZE;
    langleyCounters.itemCount--;
  }
  langleyCounters.front = 0;
  langleyCounters.rear = -1;
  langleyCounters.itemCount = 0;
}

int DataManager::getPendingQueueItemCount()
{
  return dsCounters.itemCount + gloriaCounters.itemCount + seedCounters.itemCount +
         chinampaCounters.itemCount + commaCounters.itemCount + langleyCounters.itemCount +
         getDSDOverflowCount() + getLangleyOverflowCount() + getGloriaOverflowCount() +
         getSeedlingOverflowCount() + getChinampaOverflowCount() + getCommaOverflowCount();
}

void DataManager::processGloriaQueue()
{
  while (gloriaCounters.itemCount > 0)
  {
    gloriaTankFlowPumpSerializer.pushToSerial(_HardSerial, gloriaQueue[gloriaCounters.front].data);
    gloriaCounters.front = (gloriaCounters.front + 1) % MAX_QUEUE_SIZE;
    gloriaCounters.itemCount--;
  }
}

void DataManager::processDigitalStablesDataQueue()
{
  while (dsCounters.itemCount > 0)
  {
    digitalStablesDataSerializer.pushToSerial(_HardSerial, dsQueue[dsCounters.front].data);
    dsCounters.front = (dsCounters.front + 1) % MAX_DSD_QUEUE_SIZE;
    dsCounters.itemCount--;
  }

    // now clear
  dsCounters.front=0;
  dsCounters.rear = -1;
  dsCounters.itemCount = 0;
}



void DataManager::processSeedlingMonitorDataQueue()
{
   if (debug){
    if(debug)_HardSerial.print("in processSeedlingMonitorDataQueue  seedCounters.itemCount=");
    if(debug)_HardSerial.println(seedCounters.itemCount);

    }
  while (seedCounters.itemCount > 0)
  {
     if(debug)_HardSerial.print("in processSeedlingMonitorDataQueue  pushing=");
    seedlingMonitorDataSerializer.pushToSerial(_HardSerial, seedQueue[seedCounters.front].data);
    seedCounters.front = (seedCounters.front + 1) % MAX_QUEUE_SIZE;
    seedCounters.itemCount--;
  }
}



void DataManager::enqueueSeedlingData(SeedlingMonitorData data)
{
  if (seedCounters.itemCount < MAX_QUEUE_SIZE)
  {
    seedCounters.rear = (seedCounters.rear + 1) % MAX_QUEUE_SIZE;
    seedQueue[seedCounters.rear].data = data;
    seedCounters.itemCount++;
  } else {
    overflowSeedlingData(data);
  }
    if (debug){
    if(debug)_HardSerial.print("after storing  SeedlingMonitorData  seedCounters.itemCount=");
    if(debug)_HardSerial.println(seedCounters.itemCount);

    }
}

void DataManager::enqueueDSData(DigitalStablesData data)
{
  if (dsCounters.itemCount < MAX_DSD_QUEUE_SIZE)
  {
    dsCounters.rear = (dsCounters.rear + 1) % MAX_DSD_QUEUE_SIZE;
    dsQueue[dsCounters.rear].data = data;
    dsCounters.itemCount++;
  } else {
    //
    // RAM queue is full -- fall through to the flash-backed overflow tier
    // instead of silently discarding. See conversation 2026-08-04
    // (ChinampaMonitor scale-up planning, 20+ Daffodil deployment).
    //
    overflowDSDData(data);
  }
    if (debug){
    if(debug)_HardSerial.print("after storing  digitalStablesData  dsCounters.itemCount=");
    if(debug)_HardSerial.println(dsCounters.itemCount);

    }
}

bool DataManager::loadOverflowIndex(const char* indexFile, OverflowIndex& index) {
    if (!_fs.exists(indexFile)) {
        index = OverflowIndex();
        return true;
    }
    File file = _fs.open(indexFile, "r");
    if (!file) return false;
    file.read((uint8_t*)&index, sizeof(OverflowIndex));
    file.close();
    //
    // Sanity check against the largest per-type capacity (Comma, 2000) --
    // this helper is shared-shape across all six types so it doesn't know
    // its caller's exact capacity, but any legitimate count/head/tail for
    // ANY type must fall within this generous bound. Catches stale or
    // incompatible flash content at this path (e.g. leftover data from
    // before this struct existed, or a previous struct layout with a
    // different size) that would otherwise be trusted blindly -- a garbage
    // count in the millions runs the process*Overflow() loops (no yield()
    // inside) long enough to starve the watchdog and force a silent reboot
    // mid-transmission. Self-heals by resetting and persisting a clean
    // index so this doesn't re-trigger on the next boot.
    //
    if (index.count < 0 || index.count > 2000 || index.head < 0 || index.tail < -1) {
        if (debug) _HardSerial.println("loadOverflowIndex: corrupted index detected, resetting");
        index = OverflowIndex();
        saveOverflowIndex(indexFile, index);
    }
    return true;
}

bool DataManager::saveOverflowIndex(const char* indexFile, const OverflowIndex& index) {
    File file = _fs.open(indexFile, "w");
    if (!file) return false;
    file.write((uint8_t*)&index, sizeof(OverflowIndex));
    file.close();
    return true;
}

//
// Drop-oldest circular buffer on flash. Only called from enqueueDSData() above
// when the RAM queue is already full, so this is the rare path, not the
// routine one -- see the endurance note on MAX_OVERFLOW_DSD_RECORDS in
// DataManager.h. "dropped" only increments once this store is ALSO full,
// i.e. the backlog has outgrown both tiers -- real, permanent loss.
//
void DataManager::overflowDSDData(DigitalStablesData &data) {
  if (!_initialized) return;

  OverflowIndex index;
  if (!loadOverflowIndex(DSD_OVERFLOW_INDEX_FILE, index)) {
    if (debug) _HardSerial.println("overflowDSDData: failed to load index");
    return;
  }

  bool evicting = (index.count >= MAX_OVERFLOW_DSD_RECORDS);

  // "r+" needs the file to already exist; create it empty first if this is
  // the very first overflow write since boot/format.
  if (!_fs.exists(DSD_OVERFLOW_DATA_FILE)) {
    File create = _fs.open(DSD_OVERFLOW_DATA_FILE, "w");
    if (create) create.close();
  }

  File file = _fs.open(DSD_OVERFLOW_DATA_FILE, "r+");
  if (!file) {
    if (debug) _HardSerial.println("overflowDSDData: failed to open data file");
    return;
  }

  int writeSlot = (index.tail + 1) % MAX_OVERFLOW_DSD_RECORDS;
  file.seek(writeSlot * sizeof(DigitalStablesData));
  file.write((uint8_t*)&data, sizeof(DigitalStablesData));
  file.close();

  index.tail = writeSlot;
  if (evicting) {
    // Already full -- this write just overwrote the oldest slot, so head
    // moves forward too and that old record is now permanently gone.
    index.head = (index.head + 1) % MAX_OVERFLOW_DSD_RECORDS;
    index.dropped++;
  } else {
    index.count++;
  }

  // Flash-wear tracking -- every overflow write erases/programs a flash
  // block regardless of whether it was an eviction, so this counts all of
  // them, not just the dropped ones.
  index.writeCount++;
  if (index.firstWriteEpoch == 0 && _currentEpoch > 0) {
    index.firstWriteEpoch = _currentEpoch;
  }

  saveOverflowIndex(DSD_OVERFLOW_INDEX_FILE, index);

  if (debug) {
    _HardSerial.print("overflowDSDData: count=");
    _HardSerial.print(index.count);
    _HardSerial.print(" dropped=");
    _HardSerial.println(index.dropped);
  }
}

//
// Drains the overflow store over Serial, oldest-first, same wire format as
// processDigitalStablesDataQueue()'s pushToSerial() calls. Call this AFTER
// processDigitalStablesDataQueue() so RAM (newest arrivals) goes out before
// flash overflow (older backlog) -- a minor chronological inversion during
// backlog recovery, accepted since every record carries its own secondsTime.
// Resets count/head/tail to empty once drained, but deliberately leaves
// "dropped" alone -- that's a permanent record of loss that already
// happened, not current queue state.
//
void DataManager::processDSDOverflow() {
  OverflowIndex index;
  if (!loadOverflowIndex(DSD_OVERFLOW_INDEX_FILE, index)) return;
  if (index.count == 0) return;

  File file = _fs.open(DSD_OVERFLOW_DATA_FILE, "r");
  if (!file) return;

  DigitalStablesData record;
  for (int i = 0; i < index.count; i++) {
    int slot = (index.head + i) % MAX_OVERFLOW_DSD_RECORDS;
    file.seek(slot * sizeof(DigitalStablesData));
    file.read((uint8_t*)&record, sizeof(DigitalStablesData));
    digitalStablesDataSerializer.pushToSerial(_HardSerial, record);
    yield(); // let the watchdog/WiFi stack breathe on a long backlog drain
  }
  file.close();

  index.head = 0;
  index.tail = -1;
  index.count = 0;
  saveOverflowIndex(DSD_OVERFLOW_INDEX_FILE, index);
}

int DataManager::getDSDOverflowCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(DSD_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.count;
}

unsigned long DataManager::getDSDDroppedCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(DSD_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.dropped;
}

void DataManager::setCurrentEpoch(unsigned long epoch) {
  _currentEpoch = epoch;
}

long DataManager::getDSDFlashHealthDaysRemaining() {
  OverflowIndex index;
  if (!loadOverflowIndex(DSD_OVERFLOW_INDEX_FILE, index)) return -1;
  if (index.writeCount == 0 || index.firstWriteEpoch == 0 || _currentEpoch <= index.firstWriteEpoch) {
    return -1;
  }

  unsigned long elapsedSeconds = _currentEpoch - index.firstWriteEpoch;
  // Need a real sample window before extrapolating -- a handful of writes in
  // the first few minutes would produce a wildly noisy rate estimate.
  if (elapsedSeconds < 3600) return -1;

  double writesPerSecond = (double)index.writeCount / (double)elapsedSeconds;
  double fileSizeBytes = (double)MAX_OVERFLOW_DSD_RECORDS * sizeof(DigitalStablesData);
  double blocksUsed = ceil(fileSizeBytes / FLASH_ERASE_BLOCK_SIZE);
  if (blocksUsed < 1) blocksUsed = 1;

  double avgCyclesPerBlockSoFar = (double)index.writeCount / blocksUsed;
  double cyclesPerBlockPerSecond = writesPerSecond / blocksUsed;
  if (cyclesPerBlockPerSecond <= 0) return -1;

  double remainingCycles = (double)FLASH_RATED_ERASE_CYCLES - avgCyclesPerBlockSoFar;
  if (remainingCycles <= 0) return 0; // already past the rated estimate

  double secondsRemaining = remainingCycles / cyclesPerBlockPerSecond;
  return (long)(secondsRemaining / 86400.0);
}

//
// Langley -- same shape as the DSD block above.
//
void DataManager::overflowLangleyData(LangleyData &data) {
  if (!_initialized) return;

  OverflowIndex index;
  if (!loadOverflowIndex(LANGLEY_OVERFLOW_INDEX_FILE, index)) {
    if (debug) _HardSerial.println("overflowLangleyData: failed to load index");
    return;
  }

  bool evicting = (index.count >= MAX_OVERFLOW_LANGLEY_RECORDS);

  if (!_fs.exists(LANGLEY_OVERFLOW_DATA_FILE)) {
    File create = _fs.open(LANGLEY_OVERFLOW_DATA_FILE, "w");
    if (create) create.close();
  }

  File file = _fs.open(LANGLEY_OVERFLOW_DATA_FILE, "r+");
  if (!file) {
    if (debug) _HardSerial.println("overflowLangleyData: failed to open data file");
    return;
  }

  int writeSlot = (index.tail + 1) % MAX_OVERFLOW_LANGLEY_RECORDS;
  file.seek(writeSlot * sizeof(LangleyData));
  file.write((uint8_t*)&data, sizeof(LangleyData));
  file.close();

  index.tail = writeSlot;
  if (evicting) {
    index.head = (index.head + 1) % MAX_OVERFLOW_LANGLEY_RECORDS;
    index.dropped++;
  } else {
    index.count++;
  }

  index.writeCount++;
  if (index.firstWriteEpoch == 0 && _currentEpoch > 0) {
    index.firstWriteEpoch = _currentEpoch;
  }

  saveOverflowIndex(LANGLEY_OVERFLOW_INDEX_FILE, index);
}

void DataManager::processLangleyOverflow() {
  OverflowIndex index;
  if (!loadOverflowIndex(LANGLEY_OVERFLOW_INDEX_FILE, index)) return;
  if (index.count == 0) return;

  File file = _fs.open(LANGLEY_OVERFLOW_DATA_FILE, "r");
  if (!file) return;

  LangleyData record;
  for (int i = 0; i < index.count; i++) {
    int slot = (index.head + i) % MAX_OVERFLOW_LANGLEY_RECORDS;
    file.seek(slot * sizeof(LangleyData));
    file.read((uint8_t*)&record, sizeof(LangleyData));
    langleyDataSerializer.pushToSerial(_HardSerial, record);
    yield(); // let the watchdog/WiFi stack breathe on a long backlog drain
  }
  file.close();

  index.head = 0;
  index.tail = -1;
  index.count = 0;
  saveOverflowIndex(LANGLEY_OVERFLOW_INDEX_FILE, index);
}

int DataManager::getLangleyOverflowCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(LANGLEY_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.count;
}

unsigned long DataManager::getLangleyDroppedCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(LANGLEY_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.dropped;
}

long DataManager::getLangleyFlashHealthDaysRemaining() {
  OverflowIndex index;
  if (!loadOverflowIndex(LANGLEY_OVERFLOW_INDEX_FILE, index)) return -1;
  if (index.writeCount == 0 || index.firstWriteEpoch == 0 || _currentEpoch <= index.firstWriteEpoch) {
    return -1;
  }

  unsigned long elapsedSeconds = _currentEpoch - index.firstWriteEpoch;
  if (elapsedSeconds < 3600) return -1;

  double writesPerSecond = (double)index.writeCount / (double)elapsedSeconds;
  double fileSizeBytes = (double)MAX_OVERFLOW_LANGLEY_RECORDS * sizeof(LangleyData);
  double blocksUsed = ceil(fileSizeBytes / FLASH_ERASE_BLOCK_SIZE);
  if (blocksUsed < 1) blocksUsed = 1;

  double avgCyclesPerBlockSoFar = (double)index.writeCount / blocksUsed;
  double cyclesPerBlockPerSecond = writesPerSecond / blocksUsed;
  if (cyclesPerBlockPerSecond <= 0) return -1;

  double remainingCycles = (double)FLASH_RATED_ERASE_CYCLES - avgCyclesPerBlockSoFar;
  if (remainingCycles <= 0) return 0;

  double secondsRemaining = remainingCycles / cyclesPerBlockPerSecond;
  return (long)(secondsRemaining / 86400.0);
}

//
// Gloria -- same shape as the DSD block above.
//
void DataManager::overflowGloriaData(GloriaTankFlowPumpData &data) {
  if (!_initialized) return;

  OverflowIndex index;
  if (!loadOverflowIndex(GLORIA_OVERFLOW_INDEX_FILE, index)) {
    if (debug) _HardSerial.println("overflowGloriaData: failed to load index");
    return;
  }

  bool evicting = (index.count >= MAX_OVERFLOW_GLORIA_RECORDS);

  if (!_fs.exists(GLORIA_OVERFLOW_DATA_FILE)) {
    File create = _fs.open(GLORIA_OVERFLOW_DATA_FILE, "w");
    if (create) create.close();
  }

  File file = _fs.open(GLORIA_OVERFLOW_DATA_FILE, "r+");
  if (!file) {
    if (debug) _HardSerial.println("overflowGloriaData: failed to open data file");
    return;
  }

  int writeSlot = (index.tail + 1) % MAX_OVERFLOW_GLORIA_RECORDS;
  file.seek(writeSlot * sizeof(GloriaTankFlowPumpData));
  file.write((uint8_t*)&data, sizeof(GloriaTankFlowPumpData));
  file.close();

  index.tail = writeSlot;
  if (evicting) {
    index.head = (index.head + 1) % MAX_OVERFLOW_GLORIA_RECORDS;
    index.dropped++;
  } else {
    index.count++;
  }

  index.writeCount++;
  if (index.firstWriteEpoch == 0 && _currentEpoch > 0) {
    index.firstWriteEpoch = _currentEpoch;
  }

  saveOverflowIndex(GLORIA_OVERFLOW_INDEX_FILE, index);
}

void DataManager::processGloriaOverflow() {
  OverflowIndex index;
  if (!loadOverflowIndex(GLORIA_OVERFLOW_INDEX_FILE, index)) return;
  if (index.count == 0) return;

  File file = _fs.open(GLORIA_OVERFLOW_DATA_FILE, "r");
  if (!file) return;

  GloriaTankFlowPumpData record;
  for (int i = 0; i < index.count; i++) {
    int slot = (index.head + i) % MAX_OVERFLOW_GLORIA_RECORDS;
    file.seek(slot * sizeof(GloriaTankFlowPumpData));
    file.read((uint8_t*)&record, sizeof(GloriaTankFlowPumpData));
    gloriaTankFlowPumpSerializer.pushToSerial(_HardSerial, record);
    yield(); // let the watchdog/WiFi stack breathe on a long backlog drain
  }
  file.close();

  index.head = 0;
  index.tail = -1;
  index.count = 0;
  saveOverflowIndex(GLORIA_OVERFLOW_INDEX_FILE, index);
}

int DataManager::getGloriaOverflowCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(GLORIA_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.count;
}

unsigned long DataManager::getGloriaDroppedCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(GLORIA_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.dropped;
}

long DataManager::getGloriaFlashHealthDaysRemaining() {
  OverflowIndex index;
  if (!loadOverflowIndex(GLORIA_OVERFLOW_INDEX_FILE, index)) return -1;
  if (index.writeCount == 0 || index.firstWriteEpoch == 0 || _currentEpoch <= index.firstWriteEpoch) {
    return -1;
  }

  unsigned long elapsedSeconds = _currentEpoch - index.firstWriteEpoch;
  if (elapsedSeconds < 3600) return -1;

  double writesPerSecond = (double)index.writeCount / (double)elapsedSeconds;
  double fileSizeBytes = (double)MAX_OVERFLOW_GLORIA_RECORDS * sizeof(GloriaTankFlowPumpData);
  double blocksUsed = ceil(fileSizeBytes / FLASH_ERASE_BLOCK_SIZE);
  if (blocksUsed < 1) blocksUsed = 1;

  double avgCyclesPerBlockSoFar = (double)index.writeCount / blocksUsed;
  double cyclesPerBlockPerSecond = writesPerSecond / blocksUsed;
  if (cyclesPerBlockPerSecond <= 0) return -1;

  double remainingCycles = (double)FLASH_RATED_ERASE_CYCLES - avgCyclesPerBlockSoFar;
  if (remainingCycles <= 0) return 0;

  double secondsRemaining = remainingCycles / cyclesPerBlockPerSecond;
  return (long)(secondsRemaining / 86400.0);
}

//
// Seedling -- same shape as the DSD block above.
//
void DataManager::overflowSeedlingData(SeedlingMonitorData &data) {
  if (!_initialized) return;

  OverflowIndex index;
  if (!loadOverflowIndex(SEEDLING_OVERFLOW_INDEX_FILE, index)) {
    if (debug) _HardSerial.println("overflowSeedlingData: failed to load index");
    return;
  }

  bool evicting = (index.count >= MAX_OVERFLOW_SEEDLING_RECORDS);

  if (!_fs.exists(SEEDLING_OVERFLOW_DATA_FILE)) {
    File create = _fs.open(SEEDLING_OVERFLOW_DATA_FILE, "w");
    if (create) create.close();
  }

  File file = _fs.open(SEEDLING_OVERFLOW_DATA_FILE, "r+");
  if (!file) {
    if (debug) _HardSerial.println("overflowSeedlingData: failed to open data file");
    return;
  }

  int writeSlot = (index.tail + 1) % MAX_OVERFLOW_SEEDLING_RECORDS;
  file.seek(writeSlot * sizeof(SeedlingMonitorData));
  file.write((uint8_t*)&data, sizeof(SeedlingMonitorData));
  file.close();

  index.tail = writeSlot;
  if (evicting) {
    index.head = (index.head + 1) % MAX_OVERFLOW_SEEDLING_RECORDS;
    index.dropped++;
  } else {
    index.count++;
  }

  index.writeCount++;
  if (index.firstWriteEpoch == 0 && _currentEpoch > 0) {
    index.firstWriteEpoch = _currentEpoch;
  }

  saveOverflowIndex(SEEDLING_OVERFLOW_INDEX_FILE, index);
}

void DataManager::processSeedlingOverflow() {
  OverflowIndex index;
  if (!loadOverflowIndex(SEEDLING_OVERFLOW_INDEX_FILE, index)) return;
  if (index.count == 0) return;

  File file = _fs.open(SEEDLING_OVERFLOW_DATA_FILE, "r");
  if (!file) return;

  SeedlingMonitorData record;
  for (int i = 0; i < index.count; i++) {
    int slot = (index.head + i) % MAX_OVERFLOW_SEEDLING_RECORDS;
    file.seek(slot * sizeof(SeedlingMonitorData));
    file.read((uint8_t*)&record, sizeof(SeedlingMonitorData));
    seedlingMonitorDataSerializer.pushToSerial(_HardSerial, record);
    yield(); // let the watchdog/WiFi stack breathe on a long backlog drain
  }
  file.close();

  index.head = 0;
  index.tail = -1;
  index.count = 0;
  saveOverflowIndex(SEEDLING_OVERFLOW_INDEX_FILE, index);
}

int DataManager::getSeedlingOverflowCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(SEEDLING_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.count;
}

unsigned long DataManager::getSeedlingDroppedCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(SEEDLING_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.dropped;
}

long DataManager::getSeedlingFlashHealthDaysRemaining() {
  OverflowIndex index;
  if (!loadOverflowIndex(SEEDLING_OVERFLOW_INDEX_FILE, index)) return -1;
  if (index.writeCount == 0 || index.firstWriteEpoch == 0 || _currentEpoch <= index.firstWriteEpoch) {
    return -1;
  }

  unsigned long elapsedSeconds = _currentEpoch - index.firstWriteEpoch;
  if (elapsedSeconds < 3600) return -1;

  double writesPerSecond = (double)index.writeCount / (double)elapsedSeconds;
  double fileSizeBytes = (double)MAX_OVERFLOW_SEEDLING_RECORDS * sizeof(SeedlingMonitorData);
  double blocksUsed = ceil(fileSizeBytes / FLASH_ERASE_BLOCK_SIZE);
  if (blocksUsed < 1) blocksUsed = 1;

  double avgCyclesPerBlockSoFar = (double)index.writeCount / blocksUsed;
  double cyclesPerBlockPerSecond = writesPerSecond / blocksUsed;
  if (cyclesPerBlockPerSecond <= 0) return -1;

  double remainingCycles = (double)FLASH_RATED_ERASE_CYCLES - avgCyclesPerBlockSoFar;
  if (remainingCycles <= 0) return 0;

  double secondsRemaining = remainingCycles / cyclesPerBlockPerSecond;
  return (long)(secondsRemaining / 86400.0);
}

//
// Chinampa -- same shape as the DSD block above.
//
void DataManager::overflowChinampaData(ChinampaData &data) {
  if (!_initialized) return;

  OverflowIndex index;
  if (!loadOverflowIndex(CHINAMPA_OVERFLOW_INDEX_FILE, index)) {
    if (debug) _HardSerial.println("overflowChinampaData: failed to load index");
    return;
  }

  bool evicting = (index.count >= MAX_OVERFLOW_CHINAMPA_RECORDS);

  if (!_fs.exists(CHINAMPA_OVERFLOW_DATA_FILE)) {
    File create = _fs.open(CHINAMPA_OVERFLOW_DATA_FILE, "w");
    if (create) create.close();
  }

  File file = _fs.open(CHINAMPA_OVERFLOW_DATA_FILE, "r+");
  if (!file) {
    if (debug) _HardSerial.println("overflowChinampaData: failed to open data file");
    return;
  }

  int writeSlot = (index.tail + 1) % MAX_OVERFLOW_CHINAMPA_RECORDS;
  file.seek(writeSlot * sizeof(ChinampaData));
  file.write((uint8_t*)&data, sizeof(ChinampaData));
  file.close();

  index.tail = writeSlot;
  if (evicting) {
    index.head = (index.head + 1) % MAX_OVERFLOW_CHINAMPA_RECORDS;
    index.dropped++;
  } else {
    index.count++;
  }

  index.writeCount++;
  if (index.firstWriteEpoch == 0 && _currentEpoch > 0) {
    index.firstWriteEpoch = _currentEpoch;
  }

  saveOverflowIndex(CHINAMPA_OVERFLOW_INDEX_FILE, index);
}

void DataManager::processChinampaOverflow() {
  OverflowIndex index;
  if (!loadOverflowIndex(CHINAMPA_OVERFLOW_INDEX_FILE, index)) return;
  if (index.count == 0) return;

  File file = _fs.open(CHINAMPA_OVERFLOW_DATA_FILE, "r");
  if (!file) return;

  ChinampaData record;
  for (int i = 0; i < index.count; i++) {
    int slot = (index.head + i) % MAX_OVERFLOW_CHINAMPA_RECORDS;
    file.seek(slot * sizeof(ChinampaData));
    file.read((uint8_t*)&record, sizeof(ChinampaData));
    chinampaDataSerializer.pushToSerial(_HardSerial, record);
    yield(); // let the watchdog/WiFi stack breathe on a long backlog drain
  }
  file.close();

  index.head = 0;
  index.tail = -1;
  index.count = 0;
  saveOverflowIndex(CHINAMPA_OVERFLOW_INDEX_FILE, index);
}

int DataManager::getChinampaOverflowCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(CHINAMPA_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.count;
}

unsigned long DataManager::getChinampaDroppedCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(CHINAMPA_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.dropped;
}

long DataManager::getChinampaFlashHealthDaysRemaining() {
  OverflowIndex index;
  if (!loadOverflowIndex(CHINAMPA_OVERFLOW_INDEX_FILE, index)) return -1;
  if (index.writeCount == 0 || index.firstWriteEpoch == 0 || _currentEpoch <= index.firstWriteEpoch) {
    return -1;
  }

  unsigned long elapsedSeconds = _currentEpoch - index.firstWriteEpoch;
  if (elapsedSeconds < 3600) return -1;

  double writesPerSecond = (double)index.writeCount / (double)elapsedSeconds;
  double fileSizeBytes = (double)MAX_OVERFLOW_CHINAMPA_RECORDS * sizeof(ChinampaData);
  double blocksUsed = ceil(fileSizeBytes / FLASH_ERASE_BLOCK_SIZE);
  if (blocksUsed < 1) blocksUsed = 1;

  double avgCyclesPerBlockSoFar = (double)index.writeCount / blocksUsed;
  double cyclesPerBlockPerSecond = writesPerSecond / blocksUsed;
  if (cyclesPerBlockPerSecond <= 0) return -1;

  double remainingCycles = (double)FLASH_RATED_ERASE_CYCLES - avgCyclesPerBlockSoFar;
  if (remainingCycles <= 0) return 0;

  double secondsRemaining = remainingCycles / cyclesPerBlockPerSecond;
  return (long)(secondsRemaining / 86400.0);
}

//
// Comma -- same shape as the DSD block above. Records are tiny (~32 bytes),
// so this store affords much deeper history (2000 records) at negligible
// flash cost -- see the capacity table in DataManager.h.
//
void DataManager::overflowCommaRecord(CommaRecord &data) {
  if (!_initialized) return;

  OverflowIndex index;
  if (!loadOverflowIndex(COMMA_OVERFLOW_INDEX_FILE, index)) {
    if (debug) _HardSerial.println("overflowCommaRecord: failed to load index");
    return;
  }

  bool evicting = (index.count >= MAX_OVERFLOW_COMMA_RECORDS);

  if (!_fs.exists(COMMA_OVERFLOW_DATA_FILE)) {
    File create = _fs.open(COMMA_OVERFLOW_DATA_FILE, "w");
    if (create) create.close();
  }

  File file = _fs.open(COMMA_OVERFLOW_DATA_FILE, "r+");
  if (!file) {
    if (debug) _HardSerial.println("overflowCommaRecord: failed to open data file");
    return;
  }

  int writeSlot = (index.tail + 1) % MAX_OVERFLOW_COMMA_RECORDS;
  file.seek(writeSlot * sizeof(CommaRecord));
  file.write((uint8_t*)&data, sizeof(CommaRecord));
  file.close();

  index.tail = writeSlot;
  if (evicting) {
    index.head = (index.head + 1) % MAX_OVERFLOW_COMMA_RECORDS;
    index.dropped++;
  } else {
    index.count++;
  }

  index.writeCount++;
  if (index.firstWriteEpoch == 0 && _currentEpoch > 0) {
    index.firstWriteEpoch = _currentEpoch;
  }

  saveOverflowIndex(COMMA_OVERFLOW_INDEX_FILE, index);
}

void DataManager::processCommaOverflow() {
  OverflowIndex index;
  if (!loadOverflowIndex(COMMA_OVERFLOW_INDEX_FILE, index)) return;
  if (index.count == 0) return;

  File file = _fs.open(COMMA_OVERFLOW_DATA_FILE, "r");
  if (!file) return;

  CommaRecord record;
  for (int i = 0; i < index.count; i++) {
    int slot = (index.head + i) % MAX_OVERFLOW_COMMA_RECORDS;
    file.seek(slot * sizeof(CommaRecord));
    file.read((uint8_t*)&record, sizeof(CommaRecord));
    commaRecordSerializer.pushToSerial(_HardSerial, record);
    yield(); // let the watchdog/WiFi stack breathe on a long backlog drain
  }
  file.close();

  index.head = 0;
  index.tail = -1;
  index.count = 0;
  saveOverflowIndex(COMMA_OVERFLOW_INDEX_FILE, index);
}

int DataManager::getCommaOverflowCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(COMMA_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.count;
}

unsigned long DataManager::getCommaDroppedCount() {
  OverflowIndex index;
  if (!loadOverflowIndex(COMMA_OVERFLOW_INDEX_FILE, index)) return 0;
  return index.dropped;
}

long DataManager::getCommaFlashHealthDaysRemaining() {
  OverflowIndex index;
  if (!loadOverflowIndex(COMMA_OVERFLOW_INDEX_FILE, index)) return -1;
  if (index.writeCount == 0 || index.firstWriteEpoch == 0 || _currentEpoch <= index.firstWriteEpoch) {
    return -1;
  }

  unsigned long elapsedSeconds = _currentEpoch - index.firstWriteEpoch;
  if (elapsedSeconds < 3600) return -1;

  double writesPerSecond = (double)index.writeCount / (double)elapsedSeconds;
  double fileSizeBytes = (double)MAX_OVERFLOW_COMMA_RECORDS * sizeof(CommaRecord);
  double blocksUsed = ceil(fileSizeBytes / FLASH_ERASE_BLOCK_SIZE);
  if (blocksUsed < 1) blocksUsed = 1;

  double avgCyclesPerBlockSoFar = (double)index.writeCount / blocksUsed;
  double cyclesPerBlockPerSecond = writesPerSecond / blocksUsed;
  if (cyclesPerBlockPerSecond <= 0) return -1;

  double remainingCycles = (double)FLASH_RATED_ERASE_CYCLES - avgCyclesPerBlockSoFar;
  if (remainingCycles <= 0) return 0;

  double secondsRemaining = remainingCycles / cyclesPerBlockPerSecond;
  return (long)(secondsRemaining / 86400.0);
}

int DataManager::getDSDQueueCount() { return dsCounters.itemCount; }
int DataManager::getGloriaQueueCount() { return gloriaCounters.itemCount; }
int DataManager::getSeedlingQueueCount() { return seedCounters.itemCount; }
int DataManager::getChinampaQueueCount() { return chinampaCounters.itemCount; }
int DataManager::getCommaQueueCount() { return commaCounters.itemCount; }
int DataManager::getLangleyQueueCount() { return langleyCounters.itemCount; }

void DataManager::enqueueGloriaData(GloriaTankFlowPumpData data)
{
  if (gloriaCounters.itemCount < MAX_QUEUE_SIZE)
  {
    gloriaCounters.rear = (gloriaCounters.rear + 1) % MAX_QUEUE_SIZE;
    gloriaQueue[gloriaCounters.rear].data = data;
    gloriaCounters.itemCount++;
  } else {
    overflowGloriaData(data);
  }
}

void DataManager::storeDigitalStablesData(DigitalStablesData &digitalStablesData)
{
  sn = "";
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    sn += String(digitalStablesData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(digitalStablesData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
  if (debug){
    if(debug)_HardSerial.print("adding a digitalStablesData  serialNumber=");
    if(debug)_HardSerial.print(sn);
    if(debug)_HardSerial.print(" l=");
    if(debug)_HardSerial.print(sn.length());
    if(debug)_HardSerial.print(" checksum=");
    if(debug)_HardSerial.println(checksum);
    if(debug)_HardSerial.print(" digitalStablesData.checksum=");
    if(debug)_HardSerial.println(digitalStablesData.checksum);
  }
  enqueueDSData(digitalStablesData);
  if (digitalStablesData.checksum == checksum && (sn.length() == 15 || sn.length() == 14))
  {

    

    if (debug)
    {
      DynamicJsonDocument json(1800);
      generateDigitalStablesData(digitalStablesData, json);
      serializeJsonPretty(json, _HardSerial);
      if(debug)_HardSerial.print(" number of devices=");
      if(debug)_HardSerial.println(completeObject.size());
    }
  }
  else
  {
    if (debug)
      if(debug)_HardSerial.println(" daffodilData rejected pulse serialnumne=");
    if (debug)
      if(debug)_HardSerial.println(sn);
  }
}



void DataManager::storeSeedlingMonitorData(SeedlingMonitorData &seedlingMonitorData)
{
  sn = "";
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    sn += String(seedlingMonitorData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(seedlingMonitorData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
  if (debug){
    if(debug)_HardSerial.print("adding a seedlingMonitorData  serialNumber=");
    if(debug)_HardSerial.print(sn);
    if(debug)_HardSerial.print(" l=");
    if(debug)_HardSerial.print(sn.length());
    if(debug)_HardSerial.print(" checksum=");
    if(debug)_HardSerial.println(checksum);
    if(debug)_HardSerial.print(" seedlingMonitorData.checksum=");
    if(debug)_HardSerial.println(seedlingMonitorData.checksum);
  }
  enqueueSeedlingData(seedlingMonitorData);
  if (seedlingMonitorData.checksum == checksum && (sn.length() == 15 || sn.length() == 14))
  {
    if (debug)
    {
      DynamicJsonDocument json(1800);
      generateSeedlingMonitorData(seedlingMonitorData, json);
      serializeJsonPretty(json, _HardSerial);
      if(debug)_HardSerial.print(" number of devices=");
      if(debug)_HardSerial.println(completeObject.size());
    }
  else
  {
    if (debug)
      if(debug)_HardSerial.println(" seedlingMonitorData rejected pulse serialnumne=");
    if (debug)
      if(debug)_HardSerial.println(sn);
  }
}
}

void DataManager::generateChinampaData(ChinampaData &chinampaData, DynamicJsonDocument &json)
{

  json["devicename"] = chinampaData.devicename;
  json["deviceshortname"] = chinampaData.deviceshortname;
  json["secondsTime"] = chinampaData.secondsTime;
  json["dataSamplingSec"] = chinampaData.dataSamplingSec;
   json["rtcBatVolt"] = chinampaData.rtcBatVolt;
  json["digitalStablesUpload"] = chinampaData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = chinampaData.secondsSinceLastPulse;
  
  json["pumprelaystatus"]=chinampaData.pumprelaystatus;
  
    json["fishtankoutflowsolenoidrelaystatus"]=chinampaData.fishtankoutflowsolenoidrelaystatus;
  
    json["minimumFishTankLevel"]=chinampaData.minimumFishTankLevel;
     json["maximumFishTankLevel"]=chinampaData.maximumFishTankLevel;
    json["fishTankMeasuredHeight"]=chinampaData.fishTankMeasuredHeight;
   json["fishTankHeight"]=chinampaData.fishTankHeight;
    json["minimumSumpTroughLevel"]=chinampaData.minimumSumpTroughLevel;
    json["sumpTroughMeasuredHeight"]=chinampaData.sumpTroughMeasuredHeight;
    json["sumpTroughHeight"]=chinampaData.sumpTroughHeight;
    json["sumpTroughStaleDataSeconds"]=chinampaData.sumpTroughStaleDataSeconds;
    json["fishTankStaleDataSeconds"]=chinampaData.fishTankStaleDataSeconds;
    json["alertstatus"]=chinampaData.alertstatus;
    json["pumpflowRate"]=chinampaData.pumpflowRate;
    json["microtemperature"]=chinampaData.microtemperature;
  
  //  json["soft_ap_ssid"] = sn;
  json["serialnumber"] = sn;
  json["sentBy"] = sn;
  json["internetAvailable"] = chinampaData.internetAvailable;
  json["ipAddress"] = chinampaData.ipAddress;
  json["totp"] = chinampaData.totpcode;
  json["deviceTypeId"] = chinampaData.deviceTypeId;
  json["dsLastUpload"] = chinampaData.dsLastUpload;
  json["latitude"] = chinampaData.latitude;
  json["longitude"] = chinampaData.longitude;
}

void DataManager::generateSeedlingMonitorData(SeedlingMonitorData &seedlingMonitorData, DynamicJsonDocument &json)
{

  json["devicename"] = seedlingMonitorData.devicename;
  json["deviceshortname"] = seedlingMonitorData.deviceshortname;
  json["secondsTime"] = seedlingMonitorData.secondsTime;
  json["dataSamplingSec"] = seedlingMonitorData.dataSamplingSec;
  json["temperature"] = seedlingMonitorData.temperature;
  json["rtcBatVolt"] = seedlingMonitorData.rtcBatVolt;
  json["operatingStatus"] = seedlingMonitorData.operatingStatus;
  json["digitalStablesUpload"] = seedlingMonitorData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = seedlingMonitorData.secondsSinceLastPulse;
  json["currentFunctionValue"] = seedlingMonitorData.currentFunctionValue;
  json["greenhouseTemp"] = seedlingMonitorData.greenhouseTemp;
  json["outdoorTemperature"] = seedlingMonitorData.outdoorTemperature;
  json["greenhouseHum"] = seedlingMonitorData.greenhouseHum;
  //  json["soft_ap_ssid"] = sn;
  json["serialnumber"] = sn;
  json["sentBy"] = sn;
  json["internetAvailable"] = seedlingMonitorData.internetAvailable;
  json["ipAddress"] = seedlingMonitorData.ipAddress;
  json["totp"] = seedlingMonitorData.totpcode;
  json["deviceTypeId"] = seedlingMonitorData.deviceTypeId;
  json["dsLastUpload"] = seedlingMonitorData.dsLastUpload;
  json["latitude"] = seedlingMonitorData.latitude;
  json["longitude"] = seedlingMonitorData.longitude;
}


void DataManager::generateDigitalStablesData(DigitalStablesData &digitalStablesData, DynamicJsonDocument &json)
{

  json["devicename"] = digitalStablesData.devicename;
  json["deviceshortname"] = digitalStablesData.deviceshortname;
  // json["groupidentifier"] = digitalStablesData.groupidentifier;
  json["secondsTime"] = digitalStablesData.secondsTime;
  json["dataSamplingSec"] = digitalStablesData.dataSamplingSec;
  json["currentFunctionValue"] = digitalStablesData.currentFunctionValue;
  json["temperature"] = digitalStablesData.temperature;
  json["rtcBatVolt"] = digitalStablesData.rtcBatVolt;
  json["opMode"] = digitalStablesData.opMode;
  json["rtcBatVolt"] = digitalStablesData.rtcBatVolt;
  json["operatingStatus"] = digitalStablesData.operatingStatus;
  //  json["sleepPingMinutes"] = digitalStablesData.sleepPingMinutes;
  json["digitalStablesUpload"] = digitalStablesData.digitalStablesUpload;
 // json["secondsSinceLastPulse"] = digitalStablesData.secondsSinceLastPulse;
  json["currentFunctionValue"] = digitalStablesData.currentFunctionValue;
  json["outdoortemperature"] = digitalStablesData.outdoortemperature;
  json["outdoorhumidity"] = digitalStablesData.outdoorhumidity;
  // json["minimumSepticHeight"] = digitalStablesData.minimumSepticHeight;
  json["maximumScepticHeight"] = digitalStablesData.maximumScepticHeight;
  json["panelVoltage"] = digitalStablesData.panelVoltage;
  json["panelCurrent"] = digitalStablesData.panelCurrent;
  json["batteryVoltage"] = digitalStablesData.batteryVoltage;
  json["v50Voltage"] = digitalStablesData.v50Voltage;
  json["batteryCurrent"] = digitalStablesData.batteryCurrent;
  json["estimatedRuntime"] = digitalStablesData.estimatedRuntime;
  //  json["soft_ap_ssid"] = sn;
  json["serialnumber"] = sn;
  json["sentBy"] = sn;

  // json["apAddress"] = apAddress;
  // json["hostname"] = hostname;
  // json["stationmode"] = stationmode;
  // json["ssid"] = ssid;
  // json["ssids"] = ssids;
  // json["lora"] = lora;
  json["wifiStatus"] = digitalStablesData.wifiStatus;
  // json["internetPingTime"] = internetPingTime;
  json["ipAddress"] = digitalStablesData.ipAddress;
  json["totp"] = digitalStablesData.totpcode;
  json["deviceTypeId"] = digitalStablesData.deviceTypeId;
  json["dsLastUpload"] = digitalStablesData.dsLastUpload;
  json["latitude"] = digitalStablesData.latitude;
  json["longitude"] = digitalStablesData.longitude;
}

void DataManager::storeGloria(GloriaTankFlowPumpData &gloriaTankFlowPumpData)
{
  sn = "";
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    sn += String(gloriaTankFlowPumpData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(gloriaTankFlowPumpData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
  if (gloriaTankFlowPumpData.checksum == checksum && (sn.length() == 13 || sn.length() == 15 || sn.length() == 14))
  {
    // if(serialNumber.length()==15){

    enqueueGloriaData(gloriaTankFlowPumpData);

  if (debug)
    {
      DynamicJsonDocument json(1800);
      generateGloriaTankFlowPumpWebData(gloriaTankFlowPumpData, json);
      serializeJsonPretty(json, _HardSerial);
      if(debug)_HardSerial.print(" number of devices=");
      if(debug)_HardSerial.println(completeObject.size());
    }
  }
  else
  {
    if (debug)
      _HardSerial.print("gloria rejected pulse serialnumne=");
    if (debug)
      _HardSerial.println(sn);
    if (debug)
      _HardSerial.print("gloriatata checksum=");
    if (debug)
      _HardSerial.println(gloriaTankFlowPumpData.checksum);
    if (debug)
      _HardSerial.print("checksum=");
    if (debug)
      _HardSerial.println(checksum);
  }
}

void DataManager::generateGloriaTankFlowPumpWebData(GloriaTankFlowPumpData &gloriaTankFlowPumpData, DynamicJsonDocument &json)
{
  json["devicename"] = gloriaTankFlowPumpData.devicename;
  json["deviceshortname"] = gloriaTankFlowPumpData.deviceshortname;
  json["flow1name"] = gloriaTankFlowPumpData.flow1name;
  json["flow2name"] = gloriaTankFlowPumpData.flow2name;
  json["tank1name"] = gloriaTankFlowPumpData.tank1name;
  json["tank2name"] = gloriaTankFlowPumpData.tank2name;
  json["currentFunctionValue"] = gloriaTankFlowPumpData.currentFunctionValue;
  json["flow1name"] = gloriaTankFlowPumpData.flow1name;
  json["flow2name"] = gloriaTankFlowPumpData.flow2name;
  json["tank1name"] = gloriaTankFlowPumpData.tank1name;
  json["tank2name"] = gloriaTankFlowPumpData.tank2name;
  // json["groupidentifier"] = gloriaTankFlowPumpData.groupidentifier;
  json["secondsTime"] = gloriaTankFlowPumpData.secondsTime;
  json["dataSamplingSec"] = gloriaTankFlowPumpData.dataSamplingSec;
  json["currentFunctionValue"] = gloriaTankFlowPumpData.currentFunctionValue;
  json["temperature"] = gloriaTankFlowPumpData.temperature;
  json["rtcBatVolt"] = gloriaTankFlowPumpData.rtcBatVolt;
  json["opMode"] = gloriaTankFlowPumpData.opMode;
  json["rssi"] = gloriaTankFlowPumpData.rssi;
  json["snr"] = gloriaTankFlowPumpData.snr;
  json["flowrate"] = gloriaTankFlowPumpData.flowRate;
  json["totalmilliLitres"] = gloriaTankFlowPumpData.totalMilliLitres;
  json["flowrate2"] = gloriaTankFlowPumpData.flowRate2;
  json["totalmilliLitres2"] = gloriaTankFlowPumpData.totalMilliLitres2;
  json["tank1pressurePsi"] = gloriaTankFlowPumpData.tank1PressurePsi;
  // json["tank1pressureVolts"] = gloriaTankFlowPumpData.tank1PressureVolts;
  // json["tank1waterLevel"] = gloriaTankFlowPumpData.tank1WaterLevel;
  json["tank1heightMeters"] = gloriaTankFlowPumpData.tank1HeightMeters;
  json["tank2pressurePsi"] = gloriaTankFlowPumpData.tank2PressurePsi;
  // json["tank2pressureVolts"] = gloriaTankFlowPumpData.tank2PressureVolts;
  // json["tank2waterLevel"] = gloriaTankFlowPumpData.tank2WaterLevel;
  json["tank2heightMeters"] = gloriaTankFlowPumpData.tank2HeightMeters;
  json["qfactor1"] = gloriaTankFlowPumpData.qfactor1;
  json["qfactor2"] = gloriaTankFlowPumpData.qfactor2;
  json["rtcBatVolt"] = gloriaTankFlowPumpData.rtcBatVolt;
  json["operatingStatus"] = gloriaTankFlowPumpData.operatingStatus;
  json["sleepPingMinutes"] = gloriaTankFlowPumpData.sleepPingMinutes;
  json["digitalStablesUpload"] = gloriaTankFlowPumpData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = gloriaTankFlowPumpData.secondsSinceLastPulse;
  json["checksum"] = gloriaTankFlowPumpData.checksum;
  json["serialnumber"] = sn;
  json["sentBy"] = sn;

  // json["soft_ap_ssid"] = gloriaTankFlowPumpData.soft_ap_ssid;
  // json["apAddress"] = gloriaTankFlowPumpData.apAddress;
  // json["hostname"] = gloriaTankFlowPumpData.hostname;
  // json["stationmode"] = gloriaTankFlowPumpData.stationmode;
  // json["ssid"] = gloriaTankFlowPumpData.ssid;
  json["loraActive"] = gloriaTankFlowPumpData.loraActive;
  json["internetAvailable"] = gloriaTankFlowPumpData.internetAvailable;
  // json["internetPingTime"] = gloriaTankFlowPumpData.internetPingTime;
  json["ipAddress"] = gloriaTankFlowPumpData.ipAddress;
  // json["totp"] = gloriaTankFlowPumpData.totpcode;
  json["deviceTypeId"] = gloriaTankFlowPumpData.deviceTypeId;
  json["dsLastUpload"] = gloriaTankFlowPumpData.dsLastUpload;
  json["latitude"] = gloriaTankFlowPumpData.latitude;
  json["longitude"] = gloriaTankFlowPumpData.longitude;
}

/*

void DataManager::storePancho(PanchoTankFlowData &panchoTankFlowData)
{
  sn = "";
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    sn += String(panchoTankFlowData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(panchoTankFlowData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
 if(debug) _HardSerial.print("adding a pancho serialNumber=");
 if(debug) _HardSerial.print(sn);
  if (panchoTankFlowData.checksum == checksum && sn.length() == 15)
  {
    // if(serialNumber.length()==15){
    obj = completeObject.createNestedObject(sn);
    generatePanchoTankFlowDataWebData(panchoTankFlowData, obj);

   if(debug) _HardSerial.print("  number of devices=");
   if(debug) _HardSerial.println(completeObject.size());
  }
  else
  {
   if(debug) _HardSerial.print("  pancho rejected pulse serialnumne=");
   if(debug) _HardSerial.println(sn);
  }
}

void DataManager::storeDaffodil(DaffodilData &daffodilData)
{
  sn = "";
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    sn += String(daffodilData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(daffodilData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
 if(debug) _HardSerial.print("adding a daffodilData  serialNumber=");
 if(debug) _HardSerial.print(sn);
 if(debug) _HardSerial.print(" l=");
 if(debug) _HardSerial.print(sn.length());
 if(debug) _HardSerial.print(" checksum=");
 if(debug) _HardSerial.println(checksum);
 if(debug) _HardSerial.print(" daffodilData.checksum=");
 if(debug) _HardSerial.println(daffodilData.checksum);

  if (daffodilData.checksum == checksum && (sn.length() == 15 || sn.length() == 14))
  {
    DynamicJsonDocument json(1800);
    generateDaffodilWebData(daffodilData, json);

     if(debug) serializeJsonPretty(json, _HardSerial);

   if(debug) _HardSerial.print(" number of devices=");
   if(debug) _HardSerial.println(completeObject.size());
  }
  else
  {
   if(debug) _HardSerial.println(" daffodilData rejected pulse serialnumne=");
   if(debug) _HardSerial.println(sn);
  }
}

void DataManager::storeRosie(RosieData &rosieData)
{
  sn = "";
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    sn += String(rosieData.serialnumberarray[i], HEX);
    checksum += static_cast<uint8_t>(rosieData.serialnumberarray[i]);
  }
  checksum &= 0xFF;
  if (rosieData.checksum == checksum && sn.length() == 15)
  {
    // if(){
    obj = completeObject.createNestedObject(sn);
    generateRosieWebData(rosieData, obj);
     if(debug) serializeJsonPretty(obj, _HardSerial);

   if(debug) _HardSerial.print("adding a rosie  serialNumber=");
   if(debug) _HardSerial.print(sn);
   if(debug) _HardSerial.print(" number of devices=");
   if(debug) _HardSerial.println(completeObject.size());
  }
  else
  {
   if(debug) _HardSerial.println(" rosie rejected pulse serialnumne=");
   if(debug) _HardSerial.println(sn);
  }
}



void DataManager::generateDaffodilWebData(DaffodilData &daffodilData, DynamicJsonDocument &json)
{

  json["devicename"] = daffodilData.devicename;
  json["deviceshortname"] = daffodilData.deviceshortname;
  json["groupidentifier"] = daffodilData.groupidentifier;
  json["secondsTime"] = daffodilData.secondsTime;
  json["dataSamplingSec"] = daffodilData.dataSamplingSec;
  json["currentFunctionValue"] = daffodilData.currentFunctionValue;
  json["temperature"] = daffodilData.temperature;
  json["rtcBatVolt"] = daffodilData.rtcBatVolt;
  json["opMode"] = daffodilData.opMode;
  json["rtcBatVolt"] = daffodilData.rtcBatVolt;
  json["operatingStatus"] = daffodilData.operatingStatus;
  json["sleepPingMinutes"] = daffodilData.sleepPingMinutes;
  json["digitalStablesUpload"] = daffodilData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = daffodilData.secondsSinceLastPulse;
  json["currentFunctionValue"] = daffodilData.currentFunctionValue;
  json["outdoortemperature"] = daffodilData.outdoortemperature;
  json["outdoorhumidity"] = daffodilData.outdoorhumidity;
  json["minimumSepticHeight"] = daffodilData.minimumSepticHeight;
  json["maximumScepticHeight"] = daffodilData.maximumScepticHeight;
  json["scepticAvailablePercentage"] = daffodilData.scepticAvailablePercentage;
  json["batteryVoltage"] = daffodilData.capacitorVoltage;
  //  json["soft_ap_ssid"] = sn;
  json["serialnumber"] = sn;
  json["sentBy"] = sn;

  // json["apAddress"] = apAddress;
  // json["hostname"] = hostname;
  // json["stationmode"] = stationmode;
  // json["ssid"] = ssid;
  // json["ssids"] = ssids;
  // json["lora"] = lora;
  json["internetAvailable"] = daffodilData.internetAvailable;
  // json["internetPingTime"] = internetPingTime;
  json["ipAddress"] = daffodilData.ipAddress;
  json["totp"] = daffodilData.totpcode;
  json["deviceTypeId"] = daffodilData.deviceTypeId;
  json["dsLastUpload"] = daffodilData.dsLastUpload;
  json["latitude"] = daffodilData.latitude;
  json["longitude"] = daffodilData.longitude;
}

void DataManager::generateRosieWebData(RosieData &rosieData, JsonObject &json)
{

  json["devicename"] = rosieData.devicename;
  json["deviceshortname"] = rosieData.deviceshortname;
  json["flow1name"] = rosieData.flow1name;
  json["flow2name"] = rosieData.flow2name;
  json["tank1name"] = rosieData.tank1name;
  // json["tank2name"] = rosieData.tank2name;

  json["currentFunctionValue"] = rosieData.currentFunctionValue;
  json["flow1name"] = rosieData.flow1name;
  json["flow2name"] = rosieData.flow2name;
  json["tank1name"] = rosieData.tank1name;
  json["tank2name"] = rosieData.tank2name;
  // json["groupidentifier"] = rosieData.groupidentifier;
  json["secondsTime"] = rosieData.secondsTime;
  json["dataSamplingSec"] = rosieData.dataSamplingSec;
  json["currentFunctionValue"] = rosieData.currentFunctionValue;
  json["temperature"] = rosieData.temperature;
  json["rtcBatVolt"] = rosieData.rtcBatVolt;
  json["opMode"] = rosieData.opMode;
  json["rssi"] = rosieData.rssi;
  json["snr"] = rosieData.snr;

  json["flowrate"] = rosieData.flowRate;
  json["totalmilliLitres"] = rosieData.totalMilliLitres;
  json["flowrate2"] = rosieData.flowRate2;
  json["totalmilliLitres2"] = rosieData.totalMilliLitres2;
  json["tank1pressurePsi"] = rosieData.tank1PressurePsi;
  // json["tank1pressureVolts"] = rosieData.tank1PressureVolts;
  //  json["tank1waterLevel"] = rosieData.tank1WaterLevel;
  json["tank1heightMeters"] = rosieData.tank1HeightMeters;
  json["tank2pressurePsi"] = rosieData.tank2PressurePsi;
  // json["tank2pressureVolts"] = rosieData.tank2PressureVolts;
  // json["tank2waterLevel"] = rosieData.tank2WaterLevel;
  json["tank2heightMeters"] = rosieData.tank2HeightMeters;
  json["qfactor1"] = rosieData.qfactor1;
  json["qfactor2"] = rosieData.qfactor2;
  json["rtcBatVolt"] = rosieData.rtcBatVolt;
  json["operatingStatus"] = rosieData.operatingStatus;
  json["sleepPingMinutes"] = rosieData.sleepPingMinutes;
  json["digitalStablesUpload"] = rosieData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = rosieData.secondsSinceLastPulse;
  json["serialnumber"] = sn;
  json["checksum"] = rosieData.checksum;
  json["sentBy"] = sn;

  // json["hostname"] = rosieData.hostname;
  // json["stationmode"] = rosieData.stationmode;
  // json["ssid"] = rosieData.ssid;
  json["loraActive"] = rosieData.loraActive;
  json["internetAvailable"] = rosieData.internetAvailable;
  // json["internetPingTime"] = rosieData.internetPingTime;
  json["ipAddress"] = rosieData.ipAddress;
  // json["totp"] = rosieData.totpcode;

  json["deviceTypeId"] = rosieData.deviceTypeId;
  json["dsLastUpload"] = rosieData.dsLastUpload;
  json["latitude"] = rosieData.latitude;
  json["longitude"] = rosieData.longitude;
  json["solarVoltage"] = rosieData.solarVoltage;
  json["batteryVoltage"] = rosieData.capacitorVoltage;
}



void DataManager::generatePanchoTankFlowDataWebData(PanchoTankFlowData &panchoTankFlowData, JsonObject &json)
{
  json["devicename"] = panchoTankFlowData.devicename;
  json["deviceshortname"] = panchoTankFlowData.deviceshortname;
  json["flow1name"] = panchoTankFlowData.flow1name;
  json["flow2name"] = panchoTankFlowData.flow2name;
  json["tank1name"] = panchoTankFlowData.tank1name;
  json["tank2name"] = panchoTankFlowData.tank2name;
  json["currentFunctionValue"] = panchoTankFlowData.currentFunctionValue;
  json["flow1name"] = panchoTankFlowData.flow1name;
  json["flow2name"] = panchoTankFlowData.flow2name;
  json["tank1name"] = panchoTankFlowData.tank1name;
  json["tank2name"] = panchoTankFlowData.tank2name;
  json["groupidentifier"] = panchoTankFlowData.groupidentifier;
  json["secondsTime"] = panchoTankFlowData.secondsTime;
  json["dataSamplingSec"] = panchoTankFlowData.dataSamplingSec;
  json["currentFunctionValue"] = panchoTankFlowData.currentFunctionValue;
  json["temperature"] = panchoTankFlowData.temperature;
  json["rtcBatVolt"] = panchoTankFlowData.rtcBatVolt;
  json["opMode"] = panchoTankFlowData.opMode;
  json["rssi"] = panchoTankFlowData.rssi;
  json["snr"] = panchoTankFlowData.snr;
  json["flowrate"] = panchoTankFlowData.flowRate;
  json["totalmilliLitres"] = panchoTankFlowData.totalMilliLitres;
  json["flowrate2"] = panchoTankFlowData.flowRate2;
  json["totalmilliLitres2"] = panchoTankFlowData.totalMilliLitres2;
  json["tank1pressurePsi"] = panchoTankFlowData.tank1PressurePsi;
  json["tank1pressureVolts"] = panchoTankFlowData.tank1PressureVolts;
  json["tank1waterLevel"] = panchoTankFlowData.tank1WaterLevel;
  json["tank1heightMeters"] = panchoTankFlowData.tank1HeightMeters;
  json["tank2pressurePsi"] = panchoTankFlowData.tank2PressurePsi;
  json["tank2pressureVolts"] = panchoTankFlowData.tank2PressureVolts;
  json["tank2waterLevel"] = panchoTankFlowData.tank2WaterLevel;
  json["tank2heightMeters"] = panchoTankFlowData.tank2HeightMeters;
  json["qfactor1"] = panchoTankFlowData.qfactor1;
  json["qfactor2"] = panchoTankFlowData.qfactor2;
  json["rtcBatVolt"] = panchoTankFlowData.rtcBatVolt;
  json["operatingStatus"] = panchoTankFlowData.operatingStatus;
  json["digitalStablesUpload"] = panchoTankFlowData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = panchoTankFlowData.secondsSinceLastPulse;
  json["checksum"] = panchoTankFlowData.checksum;
  json["serialnumber"] = sn;
  // json["sentBy"] = sn;
  // json["soft_ap_ssid"] = panchoTankFlowData.soft_ap_ssid;
  // json["apAddress"] = panchoTankFlowData.apAddress;
  // json["hostname"] = panchoTankFlowData.hostname;
  // json["stationmode"] = panchoTankFlowData.stationmode;
  // json["ssid"] = panchoTankFlowData.ssid;
  json["loraActive"] = panchoTankFlowData.loraActive;
  json["internetAvailable"] = panchoTankFlowData.internetAvailable;
  // json["internetPingTime"] = panchoTankFlowData.internetPingTime;
  json["ipAddress"] = panchoTankFlowData.ipAddress;
  // json["totp"] = panchoTankFlowData.totpcode;
  json["deviceTypeId"] = panchoTankFlowData.deviceTypeId;
  json["dsLastUpload"] = panchoTankFlowData.dsLastUpload;
  json["latitude"] = panchoTankFlowData.latitude;
  json["longitude"] = panchoTankFlowData.longitude;
}
*/