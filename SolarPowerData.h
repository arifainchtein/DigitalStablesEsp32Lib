    
#ifndef SOLARPOWERDATA_H
#define SOLARPOWERDATA_H

    #pragma pack(push, 1)
    struct WeatherForecast
    {
        int hour;
        double temperature;
        int cloudiness;  // percentage
        double humidity; // percentage
        double pressure; // hPa
        long secondsTime=0L;
        float rain=0.0;  // mm over the 3h forecast window (OWM rain.3h)
    };
    #pragma pack(pop)


    struct WeatherForecastUpdate{
        long totpcode=0;
        WeatherForecast forecasts[4];
    };

    // Overnight battery forecast sent by Annabelle from Cerebellum GraveyardShift task.
    // Fields mirror buildAnnabelleCommand() in GraveyardShift.java.
    // Total size: 19 bytes (must stay unique relative to other LoRa packet structs).
    #pragma pack(push, 1)
    struct GraveyardShiftUpdate {
        long     totpcode           = 0;  // TOTP auth code
        uint16_t voltAtSunset_mV    = 0;  // voltageAtSunset * 1000
        uint8_t  socAtSunset        = 0;  // voltage-based SOC % at sunset (0-100)
        uint8_t  coulombSocAtSunset = 0;  // coulomb-counted SOC % at sunset (0-100)
        uint16_t coulombMahAtSunset = 0;  // coulomb mAh remaining at sunset
        uint16_t dischargeRate_mV_hr= 0;  // discharge rate in mV/hr (dischargeRate * 1000)
        uint8_t  nightHours_x6      = 0;  // night duration in 10-min units (nightHours * 6)
        uint8_t  txCycles           = 0;  // predicted TX cycles from sunset to sunrise
        uint16_t voltAtSunrise_mV   = 0;  // projected voltage at sunrise * 1000
        uint8_t  socAtSunrise       = 0;  // projected SOC % at sunrise (0-100)
        uint8_t  anchorReliable     = 0;  // 1 if coulomb anchor is reliable, 0 = voltage fallback
        uint8_t  checksum           = 0;  // XOR checksum
    };
    #pragma pack(pop)

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
        double efficiency   = 0;
        double actualPower  = 0;
        double irradiance   = 0;
        double temperature  = 0;
    };
   

    struct TimeOfDayFactors
    {
        double elevation;  // Solar elevation angle
        double airMass;    // Atmospheric mass light must travel through
        double scattering; // Rayleigh scattering
        double absorption; // Atmospheric absorption
    };
    #endif