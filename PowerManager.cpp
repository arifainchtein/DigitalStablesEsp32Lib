// Initialize power management
#include <PowerManager.h>
RTC_DATA_ATTR int PowerManager::historyIndex = 0;

PowerManager::PowerManager(HardwareSerial &serial, ADS1115 &a, SolarInfo& s,double la, double lo, float c, float cu) :
 _HardSerial(serial), ads(a), solarInfo(s),latitude(la), longitude(lo), capacitorValue(c), currentPerLed(cu) {}

void PowerManager::begin()
{
     setCpuFrequencyMhz(80);
}

void PowerManager::updatePowerHistory(RTCInfoRecord& currentTimerRecord )
{
    PowerHistory current;
    current.dayOfYear = getDayOfYear(currentTimerRecord);
    current.hour = currentTimerRecord.hour;
    HourlySolarPowerData hourlySolarPowerData=solarInfo.calculateActualPower(currentTimerRecord  );
    current.powerGenerated = hourlySolarPowerData.actualPower;
    current.efficiency = hourlySolarPowerData.efficiency;
    current.irradiance = hourlySolarPowerData.irradiance;

    WeatherForecast weather = solarInfo.getWeatherForHour(current.hour);

    current.cloudiness = weather.cloudiness;
    current.temperature = weather.temperature;

    powerHistory[historyIndex] = current;
    historyIndex = (historyIndex + 1) % 24;
}

// float PowerManager::getPredictedPower(int hour)
// {
//     // Find similar conditions in history
//     float totalPower = 0;
//     int count = 0;

//     for (int i = 0; i < 24; i++)
//     {
//         if (powerHistory[i].hour == hour)
//         {
//             float weightedPower = powerHistory[i].powerGenerated;
//             // Apply weather similarity weight
//             weightedPower *= getWeatherSimilarity(
//                 powerHistory[i].cloudiness,
//                 powerHistory[i].temperature);
//             totalPower += weightedPower;
//             count++;
//         }
//     }

//     return count > 0 ? totalPower / count : 0;
// }

unsigned long PowerManager::calculateOptimalSleepTime(RTCInfoRecord& currentTimerRecord)
{
    
    // 1. Calculate time until sunrise tomorrow
    int currentHour = currentTimerRecord.hour;
    int currentMinute = currentTimerRecord.minute;

    // Get sunrise/sunset times based on location and day of year
    DailySolarData dailySolarData=solarInfo.getDailySolarData(currentTimerRecord);
    double sunriseHour = dailySolarData.sunrise;
    int minutesToSunrise;

    if (currentHour >= sunriseHour)
    {
        // Calculate for next day's sunrise
        minutesToSunrise = (24 - currentHour + sunriseHour) * 60 - currentMinute;
    }
    else
    {
        minutesToSunrise = (sunriseHour - currentHour) * 60 - currentMinute;
    }

    // 2. Calculate available energy
    double currentVoltage = getCapacitorVoltage();
    double availableEnergy = 0.5 * capacitorValue * (currentVoltage * currentVoltage - MIN_OPERATING_VOLTAGE * MIN_OPERATING_VOLTAGE);

    // 3. Calculate energy needed for periodic LoRa transmissions
    int totalTransmissions = (minutesToSunrise / 60) * TRANSMISSIONS_PER_HOUR;
    double energyPerTransmission = POWER_CONSUMPTION_LORA * (LORA_TRANSMISSION_TIME_MS / 1000.0);
    double totalTransmissionEnergy = energyPerTransmission * totalTransmissions;

    // 4. Calculate remaining energy for sleep
    double energyForSleep = availableEnergy - totalTransmissionEnergy;
    if (energyForSleep <= 0)
    {
        // Not enough energy for full night operation
        return 60 * 1000000UL; // Sleep for 1 minute and check again
    }

    // 5. Calculate maximum sleep time between transmissions
    double maxSleepTimeSeconds = energyForSleep / (POWER_CONSUMPTION_SLEEP * totalTransmissions);

    // 6. Convert to microseconds, ensure it's not longer than time between scheduled transmissions
    unsigned long sleepTimeUs = min(
        (unsigned long)(maxSleepTimeSeconds * 1000000UL),
        (3600UL * 1000000UL) / TRANSMISSIONS_PER_HOUR // Max 1 hour if hourly transmission
    );

    return sleepTimeUs;
}

  int PowerManager::getDayOfYear(RTCInfoRecord& currentTimerRecord) {
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int day = currentTimerRecord.date;
    int month = currentTimerRecord.month;
    int year = currentTimerRecord.year;

    int dayOfYear = day;
    
    // Check for leap year
    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    
    // Add days from previous months
    for(int i = 0; i < month - 1; i++) {
        dayOfYear += daysInMonth[i];
    }
    
    // Add leap day if needed
    if(month > 2 && isLeapYear) {
        dayOfYear++;
    }
    
    return dayOfYear;
}


// Calculate safe voltage thresholds
PowerManager::PowerThresholds PowerManager::calculateSafeThresholds(uint8_t numLeds)
{
    PowerThresholds powerThresholds;
    // For LoRa transmission
    float minLoraTxVoltage = ESP32_MIN_VOLTAGE;
    float testVoltage = ESP32_MIN_VOLTAGE;

    while (testVoltage <= 5.0)
    {
        float drop = calculateVoltageDrop(testVoltage, LORA_TX_CURRENT, TX_DURATION);
        if ((testVoltage - drop) > ESP32_MIN_VOLTAGE)
        {
            minLoraTxVoltage = testVoltage;
            break;
        }
        testVoltage += 0.1;
    }

    float minLedVoltage = ESP32_MIN_VOLTAGE;
    testVoltage = ESP32_MIN_VOLTAGE;

    while (testVoltage <= 5.0)
    {
        float drop = calculateVoltageDrop(testVoltage, numLeds * LED_CURRENT, 5.0);
        if ((testVoltage - drop) > ESP32_MIN_VOLTAGE)
        {
            minLedVoltage = testVoltage;
            break;
        }
        testVoltage += 0.1;
    }
    powerThresholds.minLoraTxVoltage = minLoraTxVoltage;
    powerThresholds.minLedVoltage = minLedVoltage;
    powerThresholds.voltageDropDuringTx = calculateVoltageDrop(minLoraTxVoltage, LORA_TX_CURRENT, TX_DURATION);

    // _HardSerial.println("Safe Operating Thresholds:");
    // _HardSerial.print("Minimum voltage for LoRa TX: ");
    // _HardSerial.print(minLoraTxVoltage, 2);
    // _HardSerial.println("V");
    // _HardSerial.print("Voltage drop during TX: ");
    // _HardSerial.print(calculateVoltageDrop(minLoraTxVoltage, LORA_TX_CURRENT, TX_DURATION), 2);
    // _HardSerial.println("V");
    // _HardSerial.print("Minimum voltage for LED: ");
    // _HardSerial.print(minLedVoltage, 2);
    // _HardSerial.println("V");
    return powerThresholds;
}

// Calculate voltage drop for a given current draw
float PowerManager::calculateVoltageDrop(float startVoltage, float current, float duration)
{
    // _HardSerial.print("current=");
    // _HardSerial.println(current);

    // // Using capacitor discharge equation: V = V0 * e^(-t/RC)
    // // Where R = V/I
    if (current == 0)
        return 0;
    if (duration == 0)
        return 0;

    float R = startVoltage / current;
    float RC = R * CAP_CAPACITANCE;
    float dropVoltage = startVoltage * (1 - exp(-duration / RC));
    // _HardSerial.print("dropVoltage=");
    //   _HardSerial.println(dropVoltage);
    return dropVoltage;
}

// Read and update voltage with smoothing
float PowerManager::getCapacitorVoltage()
{
    int16_t val_3 = ads.readADC(3);
    float f = ads.toVoltage(1); //  voltage factor
    float rawVoltage = val_3 * f;
    if (voltageEMA == 0)
        voltageEMA = rawVoltage;

    voltageEMA = (EMA_ALPHA * rawVoltage) + ((1.0 - EMA_ALPHA) * voltageEMA);
    currentVoltage = voltageEMA;
    lastVoltageCheck = millis();

    return currentVoltage;
}

float PowerManager::calculateTotalLEDCurrent(const uint8_t numLeds,const uint8_t brightnesses)
{
    float totalCurrent = 0;
    for (int i = 0; i < numLeds; i++)
    {
        totalCurrent += (brightnesses / 255.0) * currentPerLed;
    }
    return totalCurrent;
}

// Calculate safe LED brightness based on voltage
uint8_t PowerManager::calculateSafeLEDBrightness(uint8_t numLeds)
{
    double currentVoltage = getCapacitorVoltage();
    return calculateSafeLEDBrightness( currentVoltage,  numLeds);
}
uint8_t PowerManager::calculateSafeLEDBrightness(float startingVoltage, uint8_t numLeds)
{
    //
    // check the different levels of brightness
    float ledCurrent=numLeds*currentPerLed*LED_FULL_BRIGHTNESS/255;
    float afterLedDrop = predictVoltageDrop(startingVoltage,ledCurrent, 0.100);;
    if(afterLedDrop >= MIN_OPERATING_VOLTAGE){
        return LED_FULL_BRIGHTNESS;
    }

    ledCurrent=numLeds*currentPerLed*LED_MEDIUM_BRIGHTNESS/255;
     afterLedDrop = predictVoltageDrop(startingVoltage,ledCurrent, 0.100);;
    
    if(afterLedDrop >= MIN_OPERATING_VOLTAGE){
        return LED_MEDIUM_BRIGHTNESS;
    }

    ledCurrent=numLeds*currentPerLed*LED_DIM_BRIGHTNESS/255;
     afterLedDrop = predictVoltageDrop(startingVoltage,ledCurrent, 0.100);;
    
    if(afterLedDrop >= MIN_OPERATING_VOLTAGE){
        return LED_DIM_BRIGHTNESS;
    }

    ledCurrent=numLeds*currentPerLed*LED_OFF_BRIGHTNESS/255;
     afterLedDrop = predictVoltageDrop(startingVoltage,ledCurrent, 0.100);;
    
    if(afterLedDrop >= MIN_OPERATING_VOLTAGE){
        return LED_OFF_BRIGHTNESS;
    }
}

// Predict voltage drop for an operation
float PowerManager::predictVoltageDrop(float startingVoltage, float current, float duration)
{
    float R = startingVoltage / current;
    float RC = R * CAP_CAPACITANCE;
    return startingVoltage * (1 - exp(-duration / RC));
}

// Check if LoRa transmission is safe
// returns the brightness at which the leds must be set
uint8_t PowerManager::isLoraTxSafe(uint8_t numLeds)
{
    double currentVoltage = getCapacitorVoltage();
    float loraDrop = predictVoltageDrop(currentVoltage,LORA_TX_CURRENT, 0.100);
    float afterLoraDrop = currentVoltage-loraDrop;
    if(MIN_OPERATING_VOLTAGE>afterLoraDrop){
        return LORA_TX_NOT_ALLOWED;
    }
    return calculateSafeLEDBrightness(afterLoraDrop,  numLeds);
}



PowerManager::SystemStatus PowerManager::getStatus()
{
    SystemStatus status;
    status.voltage = getCapacitorVoltage();
    status.ledBrightness = ledBrightness;
    status.loraEnabled = loraEnabled;
    status.predictedLoRaDrop = predictVoltageDrop(status.voltage,LORA_TX_CURRENT, 0.100);
    status.capacitorEnergy = 0.5 * CAP_CAPACITANCE * currentVoltage * currentVoltage;
    
    float usableEnergy = status.capacitorEnergy * (1 - pow(MIN_OPERATING_VOLTAGE/currentVoltage, 2));
    status.usableEnergy=usableEnergy;

    status.fullPowerTime = usableEnergy / ESP32_POWER.active_wifi;
    status.reducedPowerTime = usableEnergy / ESP32_POWER.active_no_wifi;
    status.lightSleepTime = usableEnergy / ESP32_POWER.light_sleep;
    status.deepSleepTime = usableEnergy / ESP32_POWER.deep_sleep;

    return status;
}

void PowerManager::disableBluetooth() { 
  esp_bluedroid_disable(); 
  esp_bluedroid_deinit(); 
  esp_bt_controller_disable(); 
  esp_bt_controller_deinit(); 
  esp_bt_mem_release(ESP_BT_MODE_BTDM); 
}
void PowerManager::enableBluetooth() {
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    esp_bluedroid_init();
    esp_bluedroid_enable();
}

// WIFI_PS_NONE: No power save
// WIFI_PS_MIN_MODEM: Minimum power save
// WIFI_PS_MAX_MODEM: Maximum power save (recommended for best power savings)

void PowerManager::enableModemSleep() {
    // Set power save mode to WIFI_PS_MAX_MODEM
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
    powerSetting=ESP32_MODEM_SLEEP_CURRENT;
}

// Disable modem sleep if needed
void PowerManager::disableModemSleep() {
    esp_wifi_set_ps(WIFI_PS_NONE);
     powerSetting=ESP32_MODEM_SLEEP_CURRENT;
}

void PowerManager::enterLightSleep(uint32_t sleepTime_ms) {
    // Configure wake up sources
    esp_sleep_enable_timer_wakeup(sleepTime_ms * 1000); // Convert to microseconds
    
    // Optional: Enable GPIO wakeup
    // esp_sleep_enable_gpio_wakeup();
    // gpio_wakeup_enable(GPIO_NUM_X, GPIO_INTR_LOW_LEVEL);
    powerSetting=ESP32_LIGHT_SLEEP_CURRENT;
    // Enter light sleep
    esp_light_sleep_start();
}