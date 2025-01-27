#ifndef SolarInfo_h
#define SolarInfo_h
#include "Arduino.h"
#include <math.h>
#include <time.h>
#include <RTCInfoRecord.h>
#include <SolarPowerData.h>
#include <Dusk2Dawn.h>
#include <TimeLib.h>
#include <TimeUtils.h>

// Constants

//const double LON = 144.47472222; // Your longitude

// Solar Panel Specifications
const double PANEL_PEAK_WATTS = 10.0;
const double PANEL_WIDTH = 180.0;
const double PANEL_HEIGHT = 120.0;
const double PANEL_AREA = (PANEL_WIDTH * PANEL_HEIGHT) / 1000000.0;
const double TEMP_COEFFICIENT = -0.004; // Typical value: -0.4% per °C above STC
const double STC_TEMPERATURE = 25.0;    // Standard Test Condition temperature

// Atmospheric Constants
const double STC_IRRADIANCE = 1000.0; // W/m²
const double TURBIDITY_FACTOR = 2.0;  // Atmospheric turbidity (2=clear, 5=smoggy)

const double CLEAR_SKY_FACTOR = 1.0;     // No clouds
const double LIGHT_CLOUDS_FACTOR = 0.7;  // 1-30% clouds
const double MEDIUM_CLOUDS_FACTOR = 0.4; // 31-70% clouds
const double HEAVY_CLOUDS_FACTOR = 0.2;  // 71-90% clouds
const double OVERCAST_FACTOR = 0.1;      // 91-100% clouds


class SolarInfo
{

    protected:
		HardwareSerial& _HardSerial;
    boolean debug=false;
    
public:
    
    SolarInfo( HardwareSerial& serial,double latitude,double longitude, double altitude);
    DailySolarData getDailySolarData(RTCInfoRecord& r);
    void calculateDailySolarPowerSchedule(DailySolarPowerSchedule schedules[], int year, int month, int date);
    void calculateDailySolarPowerSchedule(DailySolarPowerSchedule schedules[], RTCInfoRecord& r);
    void setWeatherForecast(WeatherForecast forecasts[], int size);
    int getDayOfYear(int year, int month, int day);
    HourlySolarPowerData calculateActualPower(RTCInfoRecord& r);
    HourlySolarPowerData calculateActualPower(int year, int month, int date, int hour, int minute);
    WeatherForecast getWeatherForHour(double hour);
   void invalidateWeatherForecast()    ; // New method to handle invalidation

private:
   // DailySolarPowerSchedule dailySolarPowerSchedule[48];
    WeatherForecast* weatherForecasts; // Pointer to store the array
    int forecastSize; // To store the size of the array
     bool weatherDataAvailable; 
     // char* timezoneinfo;
       bool isForecastValid; 
    double latitude;
    double altitude = 420;
    double longitude;
    Dusk2Dawn* dusk2Dawn;
    long gmtOffset_sec;
    int daylightOffset_sec;
    double calculateSolarDeclination(int dayOfYear);
    double calculateIrradiance(double elevation);
   
    double calculateSunriseHour(double declination);
    double calculateSolarElevation(int hour, double declination);
    double calculatePanelEfficiency(double elevation);
   
   
    double calculateAtmosphericTransmission(double elevation, WeatherForecast weather);
    double calculateTempAdjustedEfficiency(double baseEfficiency, double temperature);
    TimeOfDayFactors calculateTimeFactors(double hour, int dayOfYear, double latitude);

    double getCloudFactor(int cloudiness);
};

#endif