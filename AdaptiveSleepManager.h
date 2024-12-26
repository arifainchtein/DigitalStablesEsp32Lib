class AdaptiveSleepManager {
private:
    static const int HISTORY_SIZE = 10;
    RTC_DATA_ATTR float voltageHistory[HISTORY_SIZE];
    RTC_DATA_ATTR float sleepTimeHistory[HISTORY_SIZE];
    RTC_DATA_ATTR int historyIndex = 0;
    
public:
    void updateHistory(float preVolts, float postVolts, unsigned long sleepTime) {
        float voltageDrop = preVolts - postVolts;
        voltageHistory[historyIndex] = voltageDrop;
        sleepTimeHistory[historyIndex] = sleepTime;
        historyIndex = (historyIndex + 1) % HISTORY_SIZE;
    }
    
    unsigned long calculateOptimalSleep() {
        float currentVoltage = readCapacitorVoltage();
        float averageDropRate = calculateAverageDropRate();
        
        // Calculate safe sleep time based on voltage drop rate
        float safeVoltageMargin = currentVoltage - MIN_OPERATING_VOLTAGE;
        unsigned long proposedSleepTime = (safeVoltageMargin / averageDropRate) * 0.8; // 20% safety margin
        
        // Adjust based on time until sunrise
        TimeInfo time = getCurrentTime();
        unsigned long timeToSunrise = calculateTimeToSunrise(time);
        
        return min(proposedSleepTime, timeToSunrise);
    }
    
    float calculateAverageDropRate() {
        float totalRate = 0;
        int validSamples = 0;
        
        for(int i = 0; i < HISTORY_SIZE; i++) {
            if(sleepTimeHistory[i] > 0) {
                totalRate += voltageHistory[i] / sleepTimeHistory[i];
                validSamples++;
            }
        }
        
        return validSamples > 0 ? totalRate / validSamples : 0;
    }
};