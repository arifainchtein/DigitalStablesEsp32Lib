#ifndef SolarInfo_h
#define SolarInfo_h
#include "Arduino.h"
#include <math.h>
#include <time.h>
#include <RTCInfoRecord.h>
#include <SolarPowerData.h>

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
public:
    
    SolarInfo(double latitude, double altitude);
     DailySolarData getDailySolarData(RTCInfoRecord& r);
    int getDayOfYear(int year, int month, int day);
    HourlySolarPowerData calculateActualPower(RTCInfoRecord& r);
    WeatherForecast getWeatherForHour(double hour);

private:
    double latitude;
    double altitude = 420;
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