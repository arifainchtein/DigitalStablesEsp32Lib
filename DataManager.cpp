#include <Preferences.h>
#include <stdio.h>
#include <string.h>
#include <esp_partition.h>
#include "GloriaTankFlowPumpData.h"
#include "RosieData.h"
#include "PanchoaTankFlowData.h"
#include "DataManager.h"
#include <nvs.h>       // Access to NVS
#define MAXKEYS 200    // Max. number of NVS keys in table
#define NAME "Headers" // Namespace used in NVS, NULL for all keys

char nvskeys[MAXKEYS][NVS_KEY_NAME_MAX_SIZE]; // Space for NVS keys

Preferences preferences;
GloriaTankFlowPumpData gloriaTankFlowPumpData;
RosieData rosieData;
PanchoaTankFlowData panchoaTankFlowData;
String keys[126];
#define DEBUG_BUFFER_SIZE 130
#define DEBUG true

DataManager::DataManager(HardwareSerial &serial) : _HardSerial(serial) {}

void DataManager::start()
{
}

template <typename TStore>
void DataManager::store(String name, const TStore &store)
{
  preferences.begin("Headers", false);
  int size = name.length() + 1;
  char buf[size];
  // name.toCharArray(buf, size);
  int dsize = sizeof(store);
  preferences.putUInt(name.c_str(), dsize);
  preferences.end();
  Serial.print("storing key,size:");
  Serial.println(size);
  Serial.print("storing data22,size:");
  Serial.println(dsize);

  preferences.begin("DeviceData", false);
  preferences.putBytes(name.c_str(), &store, dsize);
  preferences.end();
}

void DataManager::getDeviceData(JsonArray &array)
{
  // StaticJsonDocument<200> doc;
  // JsonArray array = doc.to<JsonArray>();
  JsonObject obj;
  preferences.begin("DeviceData", false);
  for (int i = 0; i < keycounter; i++)
  {
    if (strlen(keys[i].c_str()) > 0)
    {
      obj = array.createNestedObject();
      int size = readHeader(keys[i]);
      if (size == sizeof(panchoaTankFlowData))
      {
        preferences.getBytes(keys[i], &panchoaTankFlowData, sizeof(panchoaTankFlowData));
        generateRosieWebData(rosieData, obj);
      }
      else if (size == sizeof(rosieData))
      {
        preferences.getBytes(keys[i], &rosieData, sizeof(rosieData));
        generateRosieWebData(rosieData, obj);
      }
      else if (size == sizeof(gloriaTankFlowPumpData))
      {
        preferences.getBytes(keys[i], &gloriaTankFlowPumpData, sizeof(gloriaTankFlowPumpData));
        generateGloriaTankFlowPumpWebData(gloriaTankFlowPumpData, obj);
      }
    }
  }
  preferences.end();
}

void DataManager::readKeys()
{
  nvs_iterator_t it;     // Iterator for NVS
  nvs_entry_info_t info; // Info in entry
  uint16_t nvsinx = 0;
  keycounter = 0;                                 // Index in nvskey table
                                                  // Serial.println("reading key point 1");
  it = nvs_entry_find("nvs", NAME, NVS_TYPE_ANY); // Get first entry
  while (it)
  {
    nvs_entry_info(it, &info); // Get info on this entry
                               // Serial.print("reading key point 2=");
                               // Serial.println(info.type );
                               // Serial.println(info.key );
                               // Serial.println(info.namespace_name );
                               //  Serial.println(nvsinx);
                               //    Serial.print("in readdata keycounter=" );
                               //     Serial.println(keycounter);
                               //      Serial.print("in readdata contet=" );
                               //       Serial.print(keys[keycounter] );
    keys[keycounter] = info.key;
    keycounter++;

    ESP_LOGI(TAG, "%s::%s type=%d",
             info.namespace_name, info.key, info.type);
    //  if ( info.type == NVS_TYPE_STR ){                            // Only string are used

    strcpy(nvskeys[nvsinx], info.key);
    //     Serial.print("nvsinx =");
    //       Serial.println( nvsinx);
    //     Serial.print("contents =");
    //
    //      Serial.println( nvskeys[nvsinx]);                       // Save key in table
    if (++nvsinx == MAXKEYS)
    {
      nvsinx--; // Prevent excessive index
    }
    // }
    it = nvs_entry_next(it);
  }
  nvs_release_iterator(it);  // Release resource
  nvskeys[nvsinx][0] = '\0'; // Empty key at the end
  ESP_LOGI(TAG, "Read %d keys from NVS", nvsinx);
  return keycounter;
}

int DataManager::readHeader(String name)
{
  preferences.begin("Headers", false);
  int nsize = name.length() + 1;
  char buf[nsize];
  Serial.print("readheaders data,size:");
  Serial.println(nsize);
  name.toCharArray(buf, nsize);
  unsigned int size = preferences.getUInt(buf, 0);
  preferences.end();
  return size;
}

uint16_t DataManager::getTotalDataSize()
{
  readKeys();
  uint16_t totalDataSize = 0;
  // Serial.print("getTotalDataSize keycounter:");
  // Serial.println(keycounter);
  for (int i = 0; i < keycounter; i++)
  {
    // if (strlen( keys[i].c_str()) > 0) {
    totalDataSize = totalDataSize + readHeader(keys[i]);
    // Serial.print("i=");
    // Serial.println(i);
    // Serial.println(keys[i]);
    // Serial.println( readHeader(keys[i]));
    //  }
  }
  return totalDataSize;
}

void DataManager::generateRosieWebData(RosieData &rosieData, DynamicJsonDocument &json)
{
  json["currentFunctionValue"] = rosieData.currentFunctionValue;
  json["flow1name"] = rosieData.flow1name;
  json["flow2name"] = rosieData.flow2name;
  json["tank1name"] = rosieData.tank1name;
  json["tank2name"] = rosieData.tank2name;
  json["groupidentifier"] = rosieData.groupidentifier;
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
  json["tank1pressureVolts"] = rosieData.tank1PressureVolts;
  json["tank1waterLevel"] = rosieData.tank1WaterLevel;
  json["tank1heightMeters"] = rosieData.tank1HeightMeters;
  json["tank2pressurePsi"] = rosieData.tank2PressurePsi;
  json["tank2pressureVolts"] = rosieData.tank2PressureVolts;
  json["tank2waterLevel"] = rosieData.tank2WaterLevel;
  json["tank2heightMeters"] = rosieData.tank2HeightMeters;
  json["qfactor1"] = rosieData.qfactor1;
  json["qfactor2"] = rosieData.qfactor2;
  json["rtcBatVolt"] = rosieData.rtcBatVolt;
  json["operatingStatus"] = rosieData.operatingStatus;
  json["sleepPingMinutes"] = rosieData.sleepPingMinutes;
  json["digitalStablesUpload"] = rosieData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = rosieData.secondsSinceLastPulse;
  json["soft_ap_ssid"] = soft_ap_ssid;
  json["serialnumber"] = serialNumber;
  json["sentBy"] = serialNumber;

  json["apAddress"] = apAddress;
  json["hostname"] = hostname;
  json["stationmode"] = stationmode;
  json["ssid"] = ssid;
  json["ssids"] = ssids;
  json["lora"] = lora;
  json["internetAvailable"] = internetAvailable;
  json["internetPingTime"] = internetPingTime;
  json["ipAddress"] = ipAddress;
  json["totp"] = totpcode;
  json["deviceTypeId"] = rosieData.deviceTypeId;
  json["dsLastUpload"] = rosieData.dsLastUpload;
  json["latitude"] = rosieData.latitude;
  json["longitude"] = rosieData.longitude;
}

void DataManager::generateGloriaTankFlowPumpWebData(GloriaTankFlowPumpData &gloriaTankFlowPumpData, DynamicJsonDocument &json)
{
  json["currentFunctionValue"] = gloriaTankFlowPumpData.currentFunctionValue;
  json["flow1name"] = gloriaTankFlowPumpData.flow1name;
  json["flow2name"] = gloriaTankFlowPumpData.flow2name;
  json["tank1name"] = gloriaTankFlowPumpData.tank1name;
  json["tank2name"] = gloriaTankFlowPumpData.tank2name;
  json["groupidentifier"] = gloriaTankFlowPumpData.groupidentifier;
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
  json["tank1pressureVolts"] = gloriaTankFlowPumpData.tank1PressureVolts;
  json["tank1waterLevel"] = gloriaTankFlowPumpData.tank1WaterLevel;
  json["tank1heightMeters"] = gloriaTankFlowPumpData.tank1HeightMeters;
  json["tank2pressurePsi"] = gloriaTankFlowPumpData.tank2PressurePsi;
  json["tank2pressureVolts"] = gloriaTankFlowPumpData.tank2PressureVolts;
  json["tank2waterLevel"] = gloriaTankFlowPumpData.tank2WaterLevel;
  json["tank2heightMeters"] = gloriaTankFlowPumpData.tank2HeightMeters;
  json["qfactor1"] = gloriaTankFlowPumpData.qfactor1;
  json["qfactor2"] = gloriaTankFlowPumpData.qfactor2;
  json["rtcBatVolt"] = gloriaTankFlowPumpData.rtcBatVolt;
  json["operatingStatus"] = gloriaTankFlowPumpData.operatingStatus;
  json["sleepPingMinutes"] = gloriaTankFlowPumpData.sleepPingMinutes;
  json["digitalStablesUpload"] = gloriaTankFlowPumpData.digitalStablesUpload;
  json["secondsSinceLastPulse"] = gloriaTankFlowPumpData.secondsSinceLastPulse;
  json["soft_ap_ssid"] = soft_ap_ssid;
  json["serialnumber"] = serialNumber;
  json["sentBy"] = serialNumber;

  json["apAddress"] = apAddress;
  json["hostname"] = hostname;
  json["stationmode"] = stationmode;
  json["ssid"] = ssid;
  json["ssids"] = ssids;
  json["lora"] = lora;
  json["internetAvailable"] = internetAvailable;
  json["internetPingTime"] = internetPingTime;
  json["ipAddress"] = ipAddress;
  json["totp"] = totpcode;
  json["deviceTypeId"] = gloriaTankFlowPumpData.deviceTypeId;
  json["dsLastUpload"] = gloriaTankFlowPumpData.dsLastUpload;
  json["latitude"] = gloriaTankFlowPumpData.latitude;
  json["longitude"] = gloriaTankFlowPumpData.longitude;
}

void DataManager::generatePanchoTankFlowDataWebData(PanchoTankFlowData &panchoTankFlowData, DynamicJsonDocument &json)
{
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
  json["soft_ap_ssid"] = soft_ap_ssid;
  json["serialnumber"] = serialNumber;
  json["sentBy"] = serialNumber;

  json["apAddress"] = apAddress;
  json["hostname"] = hostname;
  json["stationmode"] = stationmode;
  json["ssid"] = ssid;
  json["ssids"] = ssids;
  json["lora"] = lora;
  json["internetAvailable"] = internetAvailable;
  json["internetPingTime"] = internetPingTime;
  json["ipAddress"] = ipAddress;
  json["totp"] = totpcode;
  json["deviceTypeId"] = panchoTankFlowData.deviceTypeId;
  json["dsLastUpload"] = panchoTankFlowData.dsLastUpload;
  json["latitude"] = panchoTankFlowData.latitude;
  json["longitude"] = panchoTankFlowData.longitude;
}