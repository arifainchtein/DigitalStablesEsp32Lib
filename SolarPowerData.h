    
#ifndef SOLARPOWERDATA_H
#define SOLARPOWERDATA_H
    struct WeatherForecast
    {
        int hour;
        double temperature;
        int cloudiness;  // percentage
        double humidity; // percentage
        double pressure; // hPa
    };
    

    struct DailySolarData
    {
        double sunrise;
        double sunset;
    };
    

    struct HourlySolarPowerData
    {
        double efficiency;
        double actualPower;
        double irradiance;
        double temperature;
    };
   

    struct TimeOfDayFactors
    {
        double elevation;  // Solar elevation angle
        double airMass;    // Atmospheric mass light must travel through
        double scattering; // Rayleigh scattering
        double absorption; // Atmospheric absorption
    };
    #endif