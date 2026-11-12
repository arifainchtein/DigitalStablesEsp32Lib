// WeatherForecastManager.cpp
#include "WeatherForecastManager.h"

WeatherForecastManager::WeatherForecastManager(HardwareSerial& serial, double latitude, double longitude, const char* apiKey, int watchdogPin)
    : _HardSerial(serial), lat(latitude), lon(longitude), apiKey(apiKey), _watchdogPin(watchdogPin) {
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
            if(debug)_HardSerial.printf("Failed to parse JSON: %s\n", error.c_str());
        }
    } else {
        // Handle error
        if(debug)_HardSerial.printf("Error on HTTP request: %s\n", http.errorToString(httpCode).c_str());
    }

    // Invalidate the weather forecast in SolarInfo
    solarInfo->invalidateWeatherForecast();
    return false; // Indicate failure
}

bool WeatherForecastManager::downloadWeatherData() {
    if (extended) return downloadExtended();
    HTTPClient http;

    // cnt=8 limits response to 8 x 3-hour slots (~24h), keeping payload ~3KB
    String url = "http://api.openweathermap.org/data/2.5/forecast?lat=" + String(lat) + "&lon=" + String(lon) + "&appid=" + apiKey + "&units=metric&cnt=8";
    if(debug)_HardSerial.println(url);

    http.setTimeout(30000);  // 30s — avoids indefinite hang if OWM is slow
    http.setConnectTimeout(5000);  // and no long connect wait when the router/WAN is down (2026-10-17)
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode != 200) {
        if(debug)_HardSerial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }

    // Finish TCP read before parsing so WiFi stack frames don't overlap JSON parser frames
    String payload = http.getString();
    http.end();

    if (payload.length() == 0) {
        if(debug)_HardSerial.println("Empty payload");
        return false;
    }
    if(debug){ _HardSerial.print("payload len="); _HardSerial.println(payload.length()); }

    DynamicJsonDocument doc(9000);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        if(debug)_HardSerial.printf("JSON parse error: %s\n", error.c_str());
        return false;
    }

    JsonArray list = doc["list"].as<JsonArray>();
    int size = list.size();
    if(debug){ _HardSerial.print("list size="); _HardSerial.println(size); }

    for (int i = 0; i < size && i < 8; i++) {
        JsonObject forecast = list[i].as<JsonObject>();
        long dt = forecast["dt"].as<long>();
        forecasts[i].secondsTime = dt;
        time_t t = (time_t)dt;
        struct tm* localTime = localtime(&t);
        forecasts[i].hour = localTime ? localTime->tm_hour : 0;
        forecasts[i].temperature = forecast["main"]["temp"].as<float>();
        forecasts[i].humidity    = forecast["main"]["humidity"].as<float>();
        forecasts[i].cloudiness  = forecast["clouds"]["all"].as<int>();
        forecasts[i].pressure    = forecast["main"]["pressure"].as<float>();
        forecasts[i].rain        = forecast["rain"]["3h"] | 0.0f;
    }

    saveForecasts(forecasts);
    hasForecastData = true;
    lastDownloadMs = millis() | 1;   // never 0, which means "none yet"
    return true;
}

bool WeatherForecastManager::enableExtendedForecast(int maxSlots) {
    if (extended) return true;
    if (maxSlots < 8) maxSlots = 8;
    if (maxSlots > 40) maxSlots = 40;   // OpenWeather's 5 day / 3 hour forecast has 40
    WeatherForecastExtended* e = (WeatherForecastExtended*)calloc(1, sizeof(WeatherForecastExtended));
    WeatherForecastExtSlot* a = (WeatherForecastExtSlot*)calloc(maxSlots, sizeof(WeatherForecastExtSlot));
    WeatherForecastExtSlot* b = (WeatherForecastExtSlot*)calloc(maxSlots, sizeof(WeatherForecastExtSlot));
    if (!e || !a || !b) {
        free(e); free(a); free(b);
        if(debug)_HardSerial.println("enableExtendedForecast: no room on the heap, staying with 8 slots");
        return false;
    }
    e->slots = a;
    e->spare = b;
    e->maxSlots = (uint8_t)maxSlots;
    extended = e;
    return true;
}

// The long forecast, read one slot at a time (ArduinoJson's "one array element at a time"): the 40-slot
// response is ~16 KB, too much to hold whole next to everything else on Annabelle, and each slot alone is
// small. HTTP/1.0, so the stream is the plain JSON (no chunked encoding). Read into the spare slots and
// swapped in only when it all worked, so a failed download keeps the previous forecast whole.
bool WeatherForecastManager::downloadExtended() {
    HTTPClient http;
    String url = "http://api.openweathermap.org/data/2.5/forecast?lat=" + String(lat) + "&lon=" + String(lon) + "&appid=" + apiKey + "&units=metric&cnt=" + String(extended->maxSlots);
    if(debug)_HardSerial.println(url);
    http.useHTTP10(true);
    http.setTimeout(30000);
    http.setConnectTimeout(5000);
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode != 200) {
        if(debug)_HardSerial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }
    WiFiClient& stream = http.getStream();
    if (!stream.find("\"list\":[")) {
        if(debug)_HardSerial.println("forecast: no list in the response");
        http.end();
        return false;
    }

    JsonDocument filter;
    filter["dt"] = true;
    filter["main"]["temp"] = true;
    filter["main"]["humidity"] = true;
    filter["main"]["pressure"] = true;
    filter["clouds"]["all"] = true;
    filter["rain"]["3h"] = true;
    filter["wind"]["speed"] = true;
    filter["pop"] = true;
    filter["weather"][0]["id"] = true;

    JsonDocument doc;
    WeatherForecastExtSlot* out = extended->spare;
    int n = 0;
    do {
        DeserializationError error = deserializeJson(doc, stream, DeserializationOption::Filter(filter));
        if (error) {
            if(debug)_HardSerial.printf("forecast slot %d: JSON error %s\n", n, error.c_str());
            http.end();
            return false;
        }
        WeatherForecastExtSlot& s = out[n];
        s.secondsTime = doc["dt"].as<uint32_t>();
        s.temperature = doc["main"]["temp"].as<float>();
        s.humidity    = (uint8_t)constrain(doc["main"]["humidity"].as<int>(), 0, 100);
        s.pressure    = (uint16_t)constrain((int)lround(doc["main"]["pressure"].as<float>()), 0, 2000);
        s.cloudiness  = (uint8_t)constrain(doc["clouds"]["all"].as<int>(), 0, 100);
        s.rain        = doc["rain"]["3h"] | 0.0f;
        s.wind        = doc["wind"]["speed"] | 0.0f;
        s.pop         = (uint8_t)constrain((int)lround((doc["pop"] | 0.0f) * 100), 0, 100);
        s.code        = (uint16_t)(doc["weather"][0]["id"] | 0);
        if (s.secondsTime == 0) {
            if(debug)_HardSerial.printf("forecast slot %d: no time\n", n);
            http.end();
            return false;
        }
        n++;
    } while (n < extended->maxSlots && stream.findUntil(",", "]"));

    // "city" comes after "list": its "timezone" is the farm's offset from UTC, in seconds
    int32_t tz = extended->timezoneOffset;
    if (stream.find("\"timezone\":")) tz = (int32_t)stream.parseInt();
    http.end();

    // the first 8 for LoRa and Preferences, as the 8-slot download does
    for (int i = 0; i < n && i < 8; i++) {
        const WeatherForecastExtSlot& s = out[i];
        forecasts[i].secondsTime = s.secondsTime;
        time_t t = (time_t)s.secondsTime;
        struct tm* localTime = localtime(&t);
        forecasts[i].hour = localTime ? localTime->tm_hour : 0;
        forecasts[i].temperature = s.temperature;
        forecasts[i].humidity    = s.humidity;
        forecasts[i].cloudiness  = s.cloudiness;
        forecasts[i].pressure    = s.pressure;
        forecasts[i].rain        = s.rain;
    }
    saveForecasts(forecasts);
    hasForecastData = true;

    extended->spare = extended->slots;
    extended->slots = out;
    extended->count = (uint8_t)n;
    extended->timezoneOffset = tz;
    extended->downloads++;
    lastDownloadMs = millis() | 1;
    if(debug){ _HardSerial.print("forecast slots="); _HardSerial.println(n); }
    return true;
}

void WeatherForecastManager::saveForecasts(const WeatherForecast newForecasts[], int count) {
//    serial.println("First forecast before saving:");
//     serial.println(newForecasts[0].temperature);
//     serial.println(newForecasts[0].secondsTime);

    if (count > 8) count = 8;
    if (count < 0) count = 0;

    // Only the first `count` slots come from the caller (e.g. LoRa-delivered
    // updates only carry 4 of the 8 forecast slots due to packet size limits) —
    // the remaining slots keep whatever was last persisted/downloaded.
    memcpy(forecasts, newForecasts, sizeof(WeatherForecast) * count);

    size_t forecastSize = sizeof(WeatherForecast) * 8;
    preferences.begin(PREF_NAMESPACE, false);
    size_t written = preferences.putBytes(FORECAST_KEY, forecasts, forecastSize);
    // serial.print("Bytes written: ");
    // serial.println(written);
    preferences.end();
    hasForecastData = true;
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