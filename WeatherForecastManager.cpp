// WeatherForecastManager.cpp
#include "WeatherForecastManager.h"

WeatherForecastManager::WeatherForecastManager(RTCInfoRecord& currentTimerRecord, double latitude, double longitude, const char* apiKey) 
    : lat(latitude), lon(longitude), apiKey(apiKey) { // Initialize the API key
    long  gmtOffset_sec = currentTimerRecord.timezoneOffset;  // Melbourne is UTC+10
     int daylightOffset_sec = getDaylightOffset(currentTimerRecord); // 1 hour during daylight savings

   //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}


bool WeatherForecastManager::isWeatherDataStale(RTCInfoRecord& currentTimerRecord) {
   if(!hasForecastData)return true;

    // Get the current epoch time from RTCInfoRecord
    struct tm timeInfo;
    timeInfo.tm_year = currentTimerRecord.year - 1900; // Year since 1900
    timeInfo.tm_mon = currentTimerRecord.month - 1;    // Month from 0 to 11
    timeInfo.tm_mday = currentTimerRecord.date;         // Day of the month
    timeInfo.tm_hour = currentTimerRecord.hour;         // Hour
    timeInfo.tm_min = currentTimerRecord.minute;        // Minute
    timeInfo.tm_sec = 0;                                // Seconds
    timeInfo.tm_isdst = -1;                             // Not considering daylight saving
    uint8_t forecastSize = sizeof(forecasts);
    // Convert to epoch time
    time_t currentTime = mktime(&timeInfo);
   // Find the latest valid forecast
    int index = 0;

     for (int i = 0; i < 8; i++) {
                forecasts[i] = WeatherForecast();
            }

    while (index < forecastSize - 1 && forecasts[index + 1].secondsTime <= currentTime) {
        index++;
    }
    // Check if the latest forecast is in the past
    if (forecasts[index].secondsTime < currentTime) {
        return true; // Data is stale
    }
    return false; // Data is not stale
}

  
    

 long WeatherForecastManager::getDaylightOffset(RTCInfoRecord& timerRecord) {
        struct tm timeInfo;
        timeInfo.tm_year = timerRecord.year - 1900;
        timeInfo.tm_mon = timerRecord.month - 1;
        timeInfo.tm_mday = timerRecord.date;
        timeInfo.tm_hour = timerRecord.hour;
        timeInfo.tm_min = timerRecord.minute;
        timeInfo.tm_sec = timerRecord.second;

        timeInfo.tm_isdst = -1; // Let mktime determine if DST is in effect
        mktime(&timeInfo); // Set tm_isdst based on the local time

        return (timeInfo.tm_isdst > 0) ? 3600 : 0; // 1 hour if DST is in effect
    }

bool WeatherForecastManager::downloadWeatherData(SolarInfo* solarInfo, HardwareSerial& serial) {
    // Create an HTTP client
    HTTPClient http;
    
    // Construct the API URL using latitude and longitude
   
    String url = "http://api.openweathermap.org/data/2.5/forecast?lat=" + String(lat) + "&lon=" + String(lon) + "&appid=" + apiKey + "&units=metric";
serial.print("line 70, wfm, url=");
serial.println(url);
    // Make the API call
    http.begin(url);
     digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    serial.println("line 77=");
    int httpCode = http.GET();
     digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
serial.print("line 75, wfm, httpCode=");
serial.println(httpCode);
    // Check if the request was successful
    if (httpCode > 0) {
        // Parse the JSON response
        String payload = http.getString();
         digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
serial.print("line 81, wfm, payload=");
serial.println(payload);
        // Create an array to hold the forecasts
        WeatherForecast forecasts[8]; // 8 forecasts per day 
        int size = 0;

        // Parse the JSON response
        DynamicJsonDocument doc(1024 * 10); // Adjust size based on expected JSON response
        DeserializationError error = deserializeJson(doc, payload);
serial.print("line 87, wfm, error=");

        if (!error) {
            JsonArray list = doc["list"].as<JsonArray>();
            size = list.size();

            for (int i = 0; i < size && i < 8; i++) {
                 digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
                JsonObject forecast = list[i].as<JsonObject>();
                forecasts[i].secondsTime = forecast["dt"].as<long>(); // Assuming dt is the timestamp
                forecasts[i].temperature = forecast["main"]["temp"].as<float>(); // Assuming you have a temperature field
                forecasts[i].cloudiness = forecast["clouds"][0]["all"]; // Weather description
                forecasts[i].pressure = forecast["main"]["pressure"].as<float>(); // Assuming you have a temperature field
                serial.print("safmreading forecast to parse JSON:");
                serial.print(forecasts[i].cloudiness);
            
            }
            // Set the weather forecasts in the SolarInfo object
            // store them in the preferences
             digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
            solarInfo->setWeatherForecast(forecasts, size);
             digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
            saveForecasts(forecasts);
 digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
            return true; // Indicate success
        } else {
            serial.printf("Failed to parse JSON: %s\n", error.c_str());
        }
    } else {
        // Handle error
        Serial.printf("Error on HTTP request: %s\n", http.errorToString(httpCode).c_str());
    }

    // Invalidate the weather forecast in SolarInfo
    solarInfo->invalidateWeatherForecast();
    return false; // Indicate failure
}

 void WeatherForecastManager::saveForecasts(const WeatherForecast newForecasts[8]) {
        memcpy(forecasts, newForecasts, sizeof(forecasts));
        // Save to flash
        preferences.begin(PREF_NAMESPACE, false);  // false = RW mode
        preferences.putBytes(FORECAST_KEY, newForecasts, sizeof(WeatherForecast) * 8);
        preferences.end();
    }

    
    void WeatherForecastManager::loadForecasts() {
        //  preferences.begin(PREF_NAMESPACE, false);
        // preferences.clear();
        //  preferences.end();

         preferences.begin(PREF_NAMESPACE, true);
        bool iskey = preferences.isKey(FORECAST_KEY);

        if (!iskey) {
            preferences.end();
            hasForecastData = false;
            // for (int i = 0; i < 8; i++) {
            //     forecasts[i] = WeatherForecast();
            // }
            return;
        }
        size_t readBytes = preferences.getBytes(FORECAST_KEY, forecasts, sizeof(forecasts));
        preferences.end();
        hasForecastData = (readBytes > 0);
        // if (!hasForecastData) {
        //     for (int i = 0; i < 8; i++) {
        //         forecasts[i] = WeatherForecast();
        //     }
        // }
    }

    WeatherForecast* WeatherForecastManager::getForecasts() {
          if (!hasForecastData) {
            return nullptr;  // Return null if no valid forecasts
        }
        return forecasts;
    }

    bool WeatherForecastManager::hasValidForecasts() {
        return hasForecastData;
    }

    WeatherForecast WeatherForecastManager::getForecast(int index) {
        if (index >= 0 && index < 8) {
            return forecasts[index];
        }
        return WeatherForecast();
    }