#ifndef LIBRARIES_DIGITALSTABLES_POWERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_POWERMANAGER_H_
#include <Arduino.h>
#include "ADS1X15.h"
#include "RTCInfoRecord.h"
#include <SolarInfo.h>
#include <SolarPowerData.h>

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
//#include <esp_bluedroid_api.h>

class PowerManager
{

public:

   const uint8_t LED_FULL_BRIGHTNESS = 255;
    const uint8_t LED_MEDIUM_BRIGHTNESS = 125;
    const uint8_t LED_DIM_BRIGHTNESS =  50;
    const uint8_t LED_OFF_BRIGHTNESS =  1;
    const uint8_t LORA_TX_NOT_ALLOWED =  0;
    boolean debug=false;
    struct PowerThresholds
    {
        float minLoraTxVoltage;
        float minLedVoltage;
        float voltageDropDuringTx; // percentage
    };

    struct PowerHistory
    {
        int dayOfYear;
        int hour;
        float powerGenerated;
        int cloudiness;
        float temperature;
        double efficiency;
        double actualPower;
        double irradiance;
    };

     struct SystemStatus
    {
        float voltage;
        uint8_t ledBrightness;
        bool loraEnabled;
        float predictedLoRaDrop;
        float capacitorEnergy;
        
        float fullPowerTime;
        float reducedPowerTime;
        float lightSleepTime;
        float deepSleepTime;
        float usableEnergy;
    };

struct PowerProfile {
    float active_wifi;      // Active with WiFi/BT
    float active_no_wifi;   // Active without WiFi/BT
    float light_sleep;      // Light sleep mode
    float deep_sleep;       // Deep sleep mode
};

    const PowerProfile ESP32_POWER = {
    .active_wifi = 1.0,     // ~300mA @ 3.3V
    .active_no_wifi = 0.25, // ~75mA @ 3.3V
    .light_sleep = 0.0026,  // ~0.8mA @ 3.3V
    .deep_sleep = 0.000033  // ~10µA @ 3.3V
};

    double latitude;
    double longitude;
    SolarInfo solarInfo;
    float capacitorValue=3.0;
    float currentPerLed=20;

    PowerManager(HardwareSerial &serial, ADS1115 &ads,SolarInfo& s, double lat, double lon, float capval, float curPerL);
    // PowerManager() :
    //     currentVoltage(0),
    //     lastVoltageCheck(0),
    //     lastTransmission(0),
    //     loraEnabled(false),
    //     ledBrightness(0),
    //     voltageEMA(0) {}
    void begin();
    //float calculateVoltageDrop(float startVoltage, float current, float duration);
    PowerThresholds calculateSafeThresholds(uint8_t numLeds);

    float getCapacitorVoltage();
    uint8_t calculateSafeLEDBrightness(uint8_t numberleds);
    uint8_t calculateSafeLEDBrightness(float startingVoltage,uint8_t numberleds);
    
    float predictVoltageDrop(float startingVoltage, float current, float duration);
    uint8_t isLoraTxSafe(uint8_t numberleds,RTCInfoRecord& currentTimerRecord);
    void disableBluetooth();
    void enableBluetooth();
    void enableModemSleep();
    void disableModemSleep();
    void enterLightSleep(uint32_t sleepTime_ms);
    unsigned long calculateOptimalSleepTime(RTCInfoRecord& currentTimerRecord);
    SystemStatus getStatus();

private:
    HardwareSerial &_HardSerial;
    ADS1115 &ads;
    // Power Management Constants
    const float VOLTAGE_DIVIDER_RATIO = 2.0; // If using voltage divider
    const float ADC_REFERENCE = 3.3;         // ESP32 ADC reference voltage
    const float ADC_RESOLUTION = 4095.0;     // 12-bit ADC

    // Voltage Thresholds
    const float LORA_TX_THRESHOLD = 3.8;
    const float MIN_OPERATING_VOLTAGE = 3.6;
    
    const double CAPACITOR_MAX_VOLTAGE = 5.0;      // V
    const double POWER_CONSUMPTION_SLEEP = 0.0001; // W in sleep mode
    const double POWER_CONSUMPTION_LORA = 0.12;     // W during LoRa transmission
    const int LORA_TRANSMISSION_TIME_MS = .100;     // ms per transmission
    const int TRANSMISSIONS_PER_HOUR = 1;

    // Power Consumption Parameters
    const float CAP_CAPACITANCE = 3.0;       // Farads
    const float LORA_TX_CURRENT = 0.120;     // 120mA
    const float LED_MAX_CURRENT = 0.020;     // 20mA
    const float SYSTEM_BASE_CURRENT = 0.050; // 50mA base system current
    const float ESP32_MIN_VOLTAGE = 3.0;     // Minimum ESP32 operating voltage
    const float LED_CURRENT = 0.020;         // 20mA per LED
    const float TX_DURATION = 0.100;         // 100ms LoRa transmission
    const float ESP32_LIGHT_SLEEP_CURRENT = 1; 
    const float ESP32_MODEM_SLEEP_CURRENT = 25;
    const float ESP32_WIFI_CURRENT = 25;
    float powerSetting;

    // LED Configuration
    const int NUM_LEDS = 10;
    const float LED_CURRENT_PER_LED = 0.020;    // 20mA per LED at full brightness
    const float MAX_MOSFET_CURRENT = 0.200;     // Example: 200mA max MOSFET current
    const float LED_FULL_THRESHOLD = 4.75;
    const float LED_DIM_THRESHOLD = 4.25;

 

    const float ESP32_OPERATING_VOLTAGE = 3.3;  // V
    const float ESP32_AVERAGE_CURRENT = 150;    // mA (this varies based on what features you're using)
    //const float ESP32_POWER = ESP32_OPERATING_VOLTAGE * (ESP32_AVERAGE_CURRENT/1000.0);  // Watts


    float currentVoltage;
    unsigned long lastVoltageCheck;
    unsigned long lastTransmission;
    bool loraEnabled;
    uint8_t ledBrightness;

    // Exponential moving average for voltage smoothing
    float voltageEMA;
    const float EMA_ALPHA = 0.1;
    // LEDManager ledManager;

    static RTC_DATA_ATTR PowerHistory powerHistory[24]; // Last 24 hours
    static RTC_DATA_ATTR int historyIndex;
    void updatePowerHistory(RTCInfoRecord& currentTimerRecord);
    float getPredictedPower(int hour);
    int getDayOfYear(RTCInfoRecord& currentTimerRecord);
    float calculateTotalLEDCurrent(const uint8_t numLeds,const uint8_t brightnesses);

};
#endif