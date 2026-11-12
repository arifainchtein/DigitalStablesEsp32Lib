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

// The long forecast (2026-11-12): up to 40 OpenWeather 3-hour slots (5 days), for a gateway (Annabelle) to
// pass to its Teleonome, which shows it and sends it on to devices.digitalstables.com. One download per
// farm, so every screen shows the forecast the field devices were sent. On the heap, made by
// enableExtendedForecast(), so a device that never calls it pays one pointer. The first 8 slots still go
// into forecasts[] (LoRa, Preferences) exactly as before.
struct WeatherForecastExtSlot {
    uint32_t secondsTime;  // OWM "dt", UTC
    float temperature;     // C
    float rain;            // mm over the 3h window
    float wind;            // m/s
    uint16_t pressure;     // hPa
    uint16_t code;         // OWM weather condition id (800 clear, 5xx rain, ...)
    uint8_t humidity;      // %
    uint8_t cloudiness;    // %
    uint8_t pop;           // probability of precipitation, %
};
struct WeatherForecastExtended {
    WeatherForecastExtSlot* slots;   // the forecast
    WeatherForecastExtSlot* spare;   // the next download is read into this one, then they swap
    uint8_t maxSlots;
    uint8_t count;                   // 0 until the first download since the reset
    int32_t timezoneOffset;          // OWM city.timezone, seconds east of UTC
    uint32_t downloads;              // successful downloads since the reset: a new one when this changes
};

class WeatherForecastManager {
public:
    WeatherForecastManager(HardwareSerial& serial, double latitude, double longitude, const char* apiKey, int watchdogPin = -1);
    void initialize(RTCInfoRecord& currentTimerRecord);
    boolean downloadWeatherData(SolarInfo* solarInfo); // For Solar Devices 
    boolean downloadWeatherData(); // For Non Solar Devices
    WeatherForecast* getForecasts();
    WeatherForecast getForecast(int index);
    bool hasValidForecasts();
    void loadForecasts(HardwareSerial& serial);
    bool isWeatherDataStale(RTCInfoRecord& currentTimerRecord);
    void saveForecasts(const WeatherForecast newForecasts[], int count = 8);
    // The long forecast (see WeatherForecastExtended). After this, downloadWeatherData() asks OpenWeather for
    // maxSlots slots (one call, as before) and reads them one at a time from the stream, so the response is
    // never held whole. False when the heap had no room: downloads stay as before (8 slots).
    bool enableExtendedForecast(int maxSlots);
    // nullptr unless enabled
    const WeatherForecastExtended* getExtendedForecast() { return extended; }
    // millis() of the last successful download, 0 when none since the reset
    unsigned long getLastDownloadMs() { return lastDownloadMs; }

private:
    WeatherForecastExtended* extended = nullptr;
    unsigned long lastDownloadMs = 0;
    bool downloadExtended();
HardwareSerial& _HardSerial;
    int _watchdogPin;
    WeatherForecast forecasts[8];
     bool hasForecastData = false;  // New flag to track valid data
     bool debug=true;
    Preferences preferences;
    const char* PREF_NAMESPACE = "weather";
    const char* FORECAST_KEY = "forecasts";
   // void configTime(long gmtOffset_sec, long daylightOffset_sec, const char* ntpServer); // Configure time
    double lat; // Latitude
    double lon; // Longitude
    const char* ntpServer = "pool.ntp.org";
    String apiKey; // Store the API key
    long getDaylightOffset(RTCInfoRecord& timerRecord) ;
   
};

#endif // WEATHERFORECASTMANAGER_H