    
#ifndef SOLARPOWERDATA_H
#define SOLARPOWERDATA_H
    struct WeatherForecast
    {
        int hour;
        double temperature;
        int cloudiness;  // percentage
        double humidity; // percentage
        double pressure; // hPa
        long secondsTime=0L;
    };
    

    struct DailySolarData
    {
        double sunrise;
        double sunset;
        char sunrisetime[6];
        char sunsettime[6];
        
    };
    
    struct DailySolarPowerSchedule
    {
        long time;
        double efficiency;
        float power;
        
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