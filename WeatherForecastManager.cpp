// WeatherForecastManager.cpp
#include "WeatherForecastManager.h"

WeatherForecastManager::WeatherForecastManager(HardwareSerial& serial, double latitude, double longitude, const char* apiKey) 
    : _HardSerial(serial), lat(latitude), lon(longitude), apiKey(apiKey) { // Initialize the API key
   //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void WeatherForecastManager::initialize(RTCInfoRecord& currentTimerRecord) {
    long gmtOffset_sec = currentTimerRecord.timezoneOffset;  // Get timezone offset  
    int daylightOffset_sec = getDaylightOffset(currentTimerRecord); // Get daylight savings offset

    // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

bool WeatherForecastManager::isWeatherDataStale(RTCInfoRecord& currentTimerRecord) {
   if(!hasForecastData)return true;
    uint8_t forecastSize = sizeof(forecasts);
    int index = 0;
    while (index < forecastSize - 1 && forecasts[index + 1].secondsTime <= currentTimerRecord.epoch) {
        index++;
    }
    // Check if the latest forecast is in the past
    if (forecasts[index].secondsTime < currentTimerRecord.epoch) {
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

bool WeatherForecastManager::downloadWeatherData(SolarInfo* solarInfo) {
    // Create an HTTP client
    HTTPClient http;
    
    // Construct the API URL using latitude and longitude
   
    String url = "http://api.openweathermap.org/data/2.5/forecast?lat=" + String(lat) + "&lon=" + String(lon) + "&appid=" + apiKey + "&units=metric";
   // serial.print("line 70, wfm, url=");
  //  serial.println(url);
    
    http.begin(url);
    //serial.println("line 77=");
    int httpCode = http.GET();
  //  serial.print("line 75, wfm, httpCode=");
 //   serial.println(httpCode);
    if (httpCode > 0) {
        // Parse the JSON response
        String payload = http.getString();
        // Create an array to hold the forecasts
        WeatherForecast forecasts[8]; // 8 forecasts per day 
        int size =  2*payload.length();;

        // Parse the JSON response
        DynamicJsonDocument doc(size); // Adjust size based on expected JSON response
        DeserializationError error = deserializeJson(doc, payload);
        // serial.println(error.c_str()); 
        // serial.print("line 103,  Error=");
        // serial.println(error.c_str()); 
        String jsonString; 
        if (!error) {
            JsonArray list = doc["list"].as<JsonArray>();
            size = list.size();
            // serial.print("line 106, No Error, list size=");
            // serial.println(size);
            for (int i = 0; i < size && i < 8; i++) {
                JsonObject forecast = list[i].as<JsonObject>();
                forecasts[i].secondsTime = forecast["dt"].as<long>(); // Assuming dt is the timestamp
                forecasts[i].temperature = forecast["main"]["temp"].as<float>(); // Assuming you have a temperature field
                forecasts[i].cloudiness = forecast["clouds"][0]["all"]; // Weather description
                forecasts[i].pressure = forecast["main"]["pressure"].as<float>(); // Assuming you have a temperature field
                serializeJsonPretty(forecast, jsonString); 
             //   serial.println(jsonString);
            }
            solarInfo->setWeatherForecast(forecasts, size);
             saveForecasts(forecasts);
            return true; // Indicate success
        } else {
            _HardSerial.printf("Failed to parse JSON: %s\n", error.c_str());
        }
    } else {
        // Handle error
        _HardSerial.printf("Error on HTTP request: %s\n", http.errorToString(httpCode).c_str());
    }

    // Invalidate the weather forecast in SolarInfo
    solarInfo->invalidateWeatherForecast();
    return false; // Indicate failure
}

bool WeatherForecastManager::downloadWeatherData() {
    // Create an HTTP client
    HTTPClient http;
    
    // Construct the API URL using latitude and longitude
   
    String url = "http://api.openweathermap.org/data/2.5/forecast?lat=" + String(lat) + "&lon=" + String(lon) + "&appid=" + apiKey + "&units=metric";
   _HardSerial.print("line 70, wfm, url=");
   _HardSerial.println(url);
    
    http.begin(url);
    //serial.println("line 77=");
    int httpCode = http.GET();
  //  serial.print("line 75, wfm, httpCode=");
 //   serial.println(httpCode);
    if (httpCode > 0) {
        // Parse the JSON response
        String payload = http.getString();
        // Create an array to hold the forecasts
       
        int size =  2*payload.length();;

        // Parse the JSON response
        DynamicJsonDocument doc(size); // Adjust size based on expected JSON response
        DeserializationError error = deserializeJson(doc, payload);
        // serial.println(error.c_str()); 
        // serial.print("line 103,  Error=");
        // serial.println(error.c_str()); 
        String jsonString; 
        if (!error) {
            JsonArray list = doc["list"].as<JsonArray>();
            size = list.size();
            _HardSerial.print("line 106, No Error, list size=");
            _HardSerial.println(size);
            for (int i = 0; i < size && i < 8; i++) {
                JsonObject forecast = list[i].as<JsonObject>();
                forecasts[i].secondsTime = forecast["dt"].as<long>(); // Assuming dt is the timestamp
                forecasts[i].temperature = forecast["main"]["temp"].as<float>(); // Assuming you have a temperature field
                forecasts[i].cloudiness = forecast["clouds"][0]["all"]; // Weather description
                forecasts[i].pressure = forecast["main"]["pressure"].as<float>(); // Assuming you have a temperature field
                serializeJsonPretty(forecast, jsonString); 
                _HardSerial.println(jsonString);
            }
             saveForecasts(forecasts);
             hasForecastData=true;
            return true; // Indicate success
        } else {
            _HardSerial.printf("Failed to parse JSON: %s\n", error.c_str());
        }
    } else {
        // Handle error
        _HardSerial.printf("Error on HTTP request: %s\n", http.errorToString(httpCode).c_str());
    }

    // Invalidate the weather forecast in SolarInfo
    return false; // Indicate failure
}

void WeatherForecastManager::saveForecasts(const WeatherForecast newForecasts[8]) { 
//    serial.println("First forecast before saving:");
//     serial.println(newForecasts[0].temperature);
//     serial.println(newForecasts[0].secondsTime);
    
    size_t forecastSize = sizeof(WeatherForecast) * 8;
    // serial.print("Saving forecast size: ");
    // serial.println(forecastSize);

  
    memcpy(forecasts, newForecasts, forecastSize);
    preferences.begin(PREF_NAMESPACE, false); 
    size_t written = preferences.putBytes(FORECAST_KEY, newForecasts, forecastSize); 
    // serial.print("Bytes written: "); 
    // serial.println(written); 
    preferences.end(); 
}

//  void WeatherForecastManager::saveForecasts(const WeatherForecast newForecasts[8]) {
//         memcpy(forecasts, newForecasts, sizeof(forecasts));
//         // Save to flash
//         preferences.begin(PREF_NAMESPACE, false);  // false = RW mode
//         preferences.putBytes(FORECAST_KEY, newForecasts, sizeof(WeatherForecast) * 8);
//         preferences.end();
//     }

    void WeatherForecastManager::loadForecasts(HardwareSerial& serial) { 
        preferences.begin(PREF_NAMESPACE, true); 
        bool iskey = preferences.isKey(FORECAST_KEY); 
        serial.print("Key exists: "); 
        serial.println(iskey);

        if (!iskey) { 
            preferences.end(); 
            hasForecastData = false; 
            return; 
        }

        size_t forecastSize = sizeof(WeatherForecast) * 8; 
        size_t readBytes = preferences.getBytes(FORECAST_KEY, forecasts, forecastSize); 
        serial.print("Bytes read: "); 
        serial.println(readBytes);

        // Print first forecast after loading 
        serial.println("First forecast after loading:"); 
        serial.println(forecasts[0].temperature); 
        serial.println(forecasts[0].secondsTime);

        preferences.end(); hasForecastData = (readBytes > 0); 
    }

    // void WeatherForecastManager::loadForecasts() {
    //     //  preferences.begin(PREF_NAMESPACE, false);
    //     // preferences.clear();
    //     //  preferences.end();

    //      preferences.begin(PREF_NAMESPACE, true);
    //     bool iskey = preferences.isKey(FORECAST_KEY);

    //     if (!iskey) {
    //         preferences.end();
    //         hasForecastData = false;
    //         // for (int i = 0; i < 8; i++) {
    //         //     forecasts[i] = WeatherForecast();
    //         // }
    //         return;
    //     }
    //     size_t readBytes = preferences.getBytes(FORECAST_KEY, forecasts, sizeof(forecasts));
    //     preferences.end();
    //     hasForecastData = (readBytes > 0);
    //     // if (!hasForecastData) {
    //     //     for (int i = 0; i < 8; i++) {
    //     //         forecasts[i] = WeatherForecast();
    //     //     }
    //     // }
    // }

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