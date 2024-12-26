class VoltageMonitor {
private:
    const float CRITICAL_VOLTAGE = 2.8;  // Slightly above MIN_OPERATING_VOLTAGE
    const float WARNING_VOLTAGE = 3.0;
    RTC_DATA_ATTR float lastVoltage = 0;
    RTC_DATA_ATTR unsigned long totalSleepTime = 0;
    
public:
    void configureVoltageMonitor() {
        // Configure ESP32 ULP to monitor voltage during sleep
        // This is hardware specific and requires ULP assembly
        ulp_configure_voltage_monitoring(CRITICAL_VOLTAGE);
    }
    
    bool shouldWakeUp(float currentVoltage) {
        // Check voltage trend
        float voltageDropRate = (lastVoltage - currentVoltage) / totalSleepTime;
        float predictedTimeToEmpty = (currentVoltage - MIN_OPERATING_VOLTAGE) / voltageDropRate;
        
        // Wake up if:
        // 1. Voltage is below warning level
        // 2. Voltage is dropping faster than expected
        // 3. Predicted time to empty is less than safety margin
        return (currentVoltage <= WARNING_VOLTAGE) ||
               (voltageDropRate > EXPECTED_DROP_RATE * 1.2) ||
               (predictedTimeToEmpty < SAFETY_MARGIN_MINUTES * 60);
    }
    
    void updateStats(float voltage, unsigned long sleepTime) {
        lastVoltage = voltage;
        totalSleepTime = sleepTime;
    }
};