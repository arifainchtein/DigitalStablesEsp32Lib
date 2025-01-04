// WeatherForecastManager.h
#ifndef WEATHERFORECASTMANAGER_H
#define WEATHERFORECASTMANAGER_H

#include "SolarPowerData.h"
#include "SolarInfo.h" // Include SolarInfo to pass forecasts to it
#include <WiFi.h> // Include WiFi library for network operations
#include <HTTPClient.h> // Include HTTP client for API calls
#include <TimeLib.h>
#include <time.h>
#include <ctime>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <RTCInfoRecord.h>
#define WATCHDOG_WDI 18

class WeatherForecastManager {
public:
    WeatherForecastManager(RTCInfoRecord& currentTimerRecord, double latitude, double longitude,const char* apiKey);
    boolean downloadWeatherData(SolarInfo* solarInfo, HardwareSerial& serial); // Method to fetch weather data and update SolarInfo
    WeatherForecast* getForecasts();
    WeatherForecast getForecast(int index);
    bool hasValidForecasts();
    void loadForecasts();
    bool isWeatherDataStale(RTCInfoRecord& currentTimerRecord);

private:
    WeatherForecast forecasts[8];
     bool hasForecastData = false;  // New flag to track valid data
     
    Preferences preferences;
    const char* PREF_NAMESPACE = "weather";
    const char* FORECAST_KEY = "forecasts";
   // void configTime(long gmtOffset_sec, long daylightOffset_sec, const char* ntpServer); // Configure time
    double lat; // Latitude
    double lon; // Longitude
    const char* ntpServer = "pool.ntp.org";
    String apiKey; // Store the API key
    long getDaylightOffset(RTCInfoRecord& timerRecord) ;
    void saveForecasts(const WeatherForecast newForecasts[8]);
};

#endif // WEATHERFORECASTMANAGER_H