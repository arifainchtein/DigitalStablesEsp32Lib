#ifndef LIBRARIES_DIGITALSTABLES_THERMALMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_THERMALMANAGER_H_
#include <OneWire.h>
#include <DallasTemperature.h>

class ThermalManager {
private:
    const int ONE_WIRE_BUS;
    OneWire oneWire;
    DallasTemperature sensors;
     HardwareSerial &_HardSerial;
    
    // Temperature thresholds for ESP32
    const float ESP32_CRITICAL_TEMP = 85.0;  // Absolute maximum
    const float ESP32_WARNING_TEMP = 75.0;   // Warning threshold
    const float TEMP_HYSTERESIS = 5.0;       // Temperature must drop this much below threshold to resume
    
    // Temperature tracking
    float currentTemp = 0.0;
    float lastTemp = 0.0;
    unsigned long lastTempCheck = 0;
    const unsigned long TEMP_CHECK_INTERVAL = 1000; // Check every second
    bool isOverheated = false;

public:
    ThermalManager(int pin, HardwareSerial& serial) : 
     _HardSerial(serial) ,
        ONE_WIRE_BUS(pin),
        oneWire(ONE_WIRE_BUS),
        sensors(&oneWire) {
        
        sensors.begin();
        sensors.setResolution(12); // 12-bit resolution
    }
    
    struct ThermalStatus {
        float temperature;
        bool isCritical;
        bool isWarning;
        float tempChange;  // Rate of change
    };
    
    ThermalStatus checkTemperature() {
        ThermalStatus status;
        _HardSerial.println("1tempera=");
        // if (millis() - lastTempCheck >= TEMP_CHECK_INTERVAL) {
            lastTemp = currentTemp;
            sensors.requestTemperatures();
            currentTemp = sensors.getTempCByIndex(0);
            lastTempCheck = millis();
            _HardSerial.print("tempera=");
            _HardSerial.println(currentTemp);
            
        // }
        
        status.temperature = currentTemp;
        status.isCritical = (currentTemp >= ESP32_CRITICAL_TEMP);
        status.isWarning = (currentTemp >= ESP32_WARNING_TEMP);
        status.tempChange = (currentTemp - lastTemp) * (1000.0 / TEMP_CHECK_INTERVAL); // °C/second
        
        return status;
    }
    
    bool shouldShutdown() {
        ThermalStatus status = checkTemperature();
        
        if (status.isCritical) {
            isOverheated = true;
        } else if (isOverheated && status.temperature < (ESP32_CRITICAL_TEMP - TEMP_HYSTERESIS)) {
            isOverheated = false;
        }
        
        return isOverheated;
    }
};

#endif