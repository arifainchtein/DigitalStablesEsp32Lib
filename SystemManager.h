#ifndef LIBRARIES_DIGITALSTABLES_SYSTEMMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_SYSTEMMANAGER_H_

class SystemManager {
private:
    ThermalManager thermalManager;
    const uint64_t SLEEP_TIME_US = 30e6; // 30 seconds
    
    void goToSleep() {
        Serial.println("CRITICAL TEMPERATURE! Going to sleep...");
        Serial.flush();
        
        // Prepare for sleep
        // Save any critical data here
        
        // Configure wake-up timer
        esp_sleep_enable_timer_wakeup(SLEEP_TIME_US);
        
        // Go to deep sleep
        esp_deep_sleep_start();
    }

public:
    SystemManager(int tempPin) : thermalManager(tempPin) {}
    
    void update() {
        auto status = thermalManager.checkTemperature();
        
        // Print temperature status
        if (status.isWarning) {
            Serial.print("WARNING - High Temperature: ");
            Serial.print(status.temperature);
            Serial.println("°C");
        }
        
        // Check if we need to shut down
        if (thermalManager.shouldShutdown()) {
            goToSleep();
        }
        
        // Optional: Implement preventive measures when approaching critical temperature
        if (status.temperature > (ESP32_WARNING_TEMP + 5)) {
            // Reduce system load
            // e.g., reduce LED brightness, slow down operations, etc.
        }
    }
};
#endif