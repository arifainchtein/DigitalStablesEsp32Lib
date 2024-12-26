#include <SolarInfo.h>

// Sample weather forecast data (replace with actual API data)
WeatherForecast forecasts[] = {
        {0, 18.0, 20, 65, 1013},
        {3, 17.0, 30, 70, 1012},
        {6, 16.0, 10, 75, 1012},
        {9, 22.0, 15, 60, 1013},
        {12, 25.0, 5, 55, 1014},
        {15, 26.0, 25, 50, 1013},
        {18, 20.0, 15, 60, 1013},
        {21, 19.0, 20, 65, 1014}
    };
SolarInfo::SolarInfo(double l, double a) : latitude(l), altitude(a){}



TimeOfDayFactors SolarInfo::calculateTimeFactors(double hour, int dayOfYear, double latitude) {
    TimeOfDayFactors factors;
    
    // Calculate solar elevation
    double declination = calculateSolarDeclination(dayOfYear);
    double hourAngle = (hour - 12) * 15;
    double elevation = calculateSolarElevation(hour, declination);
    
    factors.elevation = elevation;

    // Air Mass calculation (more accurate version)
    if (elevation > 0) {
        factors.airMass = 1 / (sin(elevation * PI/180) + 
                        0.50572 * pow((6.07995 + elevation), -1.6364));
    } else {
        factors.airMass = 0;
    }

    // Rayleigh scattering (more prominent at sunrise/sunset)
    factors.scattering = exp(-0.1 * factors.airMass);

    // Atmospheric absorption
    factors.absorption = exp(-0.2 * factors.airMass);

    return factors;
}

double SolarInfo::getCloudFactor(int cloudiness) {
    if (cloudiness <= 0) return CLEAR_SKY_FACTOR;
    else if (cloudiness <= 30) return LIGHT_CLOUDS_FACTOR;
    else if (cloudiness <= 70) return MEDIUM_CLOUDS_FACTOR;
    else if (cloudiness <= 90) return HEAVY_CLOUDS_FACTOR;
    else return OVERCAST_FACTOR;
}

// double SolarInfo::calculateActualPower(double elevation, WeatherForecast weather) {
//     SolarInfo::TimeOfDayFactors timeFactors = calculateTimeFactors(weather.hour, 
//                                   getDayOfYear(2024, 3, 15), LAT);
    
//     if (timeFactors.elevation <= 0) return 0;

//     // Base irradiance with time-of-day effects
//     double baseIrradiance = STC_IRRADIANCE * 
//                            sin(timeFactors.elevation * PI/180) * 
//                            timeFactors.scattering * 
//                            timeFactors.absorption;

//     // Cloud effects
//     double cloudFactor = getCloudFactor(weather.cloudiness);
//     double actualIrradiance = baseIrradiance * cloudFactor;

//     // Temperature effects
//     double tempDiff = weather.temperature - STC_TEMPERATURE;
//     double tempCoefficient = 1.0 + (TEMP_COEFFICIENT * tempDiff);

//     // Final power calculation
//     double power = actualIrradiance * PANEL_AREA * tempCoefficient;

//     return min(power, PANEL_PEAK_WATTS);
// }

// Calculate actual power output with all factors
HourlySolarPowerData  SolarInfo::calculateActualPower(RTCInfoRecord& currentTimerRecord ) {
    HourlySolarPowerData data;

    uint8_t year = currentTimerRecord.year;
    uint8_t month = currentTimerRecord.month;
    uint8_t date = currentTimerRecord.date;
    uint8_t hour = currentTimerRecord.hour;
    uint8_t minute = currentTimerRecord.minute;

    double timeDecimal = hour + (minute / 60.0);
    WeatherForecast weather = getWeatherForHour(timeDecimal);
    
    double elevation = calculateSolarElevation(timeDecimal, 
                        calculateSolarDeclination(getDayOfYear(year, month, date)));

    if (elevation <= 0) return data;
    
    // Calculate base solar irradiance
    double baseIrradiance = STC_IRRADIANCE * sin(elevation * PI/180);
    
    // Apply atmospheric effects
    double transmission = calculateAtmosphericTransmission(elevation, weather);
    double actualIrradiance = baseIrradiance * transmission;



    // Calculate base efficiency
    double baseEfficiency = cos((90 - elevation) * PI/180);
    
    // Apply temperature adjustment
    double adjustedEfficiency = calculateTempAdjustedEfficiency(baseEfficiency, weather.temperature);
    
    // Calculate final power
    double power = actualIrradiance * PANEL_AREA * adjustedEfficiency;
    data.efficiency = adjustedEfficiency;
    data.actualPower =  min(power, PANEL_PEAK_WATTS);
    data.irradiance=actualIrradiance;
    data.temperature = weather.temperature;
    return data;
}

// Calculate day of year from date
int SolarInfo::getDayOfYear(int year, int month, int day)
{
    const int daysInMonth[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    int dayOfYear = daysInMonth[month - 1] + day;

    // Adjust for leap year
    if (month > 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
    {
        dayOfYear++;
    }
    return dayOfYear;
}

// Calculate solar irradiance based on elevation angle
double SolarInfo::calculateIrradiance(double elevation)
{
    if (elevation <= 0)
        return 0;
    // Approximate atmospheric effects
    double AM = 1 / sin(elevation * PI / 180); // Air Mass
    double irradiance = STC_IRRADIANCE * pow(0.7, AM) * sin(elevation * PI / 180);
    return irradiance;
}


// Calculate solar declination angle
double SolarInfo::calculateSolarDeclination(int dayOfYear)
{
    return 23.45 * sin(2 * PI * (284 + dayOfYear) / 365.0);
}

// Calculate sunrise hour (in 24h format)
double SolarInfo::calculateSunriseHour(double declination)
{
    double cosH = -tan(latitude * PI / 180) * tan(declination * PI / 180);
    if (cosH > 1 || cosH < -1)
        return -1; // No sunrise/sunset
    return 12.0 - (acos(cosH) * 180 / PI) / 15.0;
}

// Calculate solar elevation for specific hour
double SolarInfo::calculateSolarElevation(int hour, double declination)
{
    double hourAngle = (hour - 12) * 15;
    double sinElevation = sin(latitude * PI / 180) * sin(declination * PI / 180) +
                          cos(latitude * PI / 180) * cos(declination * PI / 180) *
                              cos(hourAngle * PI / 180);
    return asin(sinElevation) * 180 / PI;
}

// Calculate panel efficiency (%)
double SolarInfo::calculatePanelEfficiency(double elevation)
{
    if (elevation <= 0)
        return 0;
    return cos((90 - elevation) * PI / 180) * 100;
}

// Main function to get solar data for current day
DailySolarData SolarInfo::getDailySolarData(RTCInfoRecord& r){
    uint8_t year = r.year;
    uint8_t month=r.month;
    uint8_t date = r.date;
    struct tm timeinfo;
    DailySolarData data;

   

    int dayOfYear = getDayOfYear(year, month, date);
    double declination = calculateSolarDeclination(dayOfYear);

    // Calculate sunrise and sunset
    double sunrise = calculateSunriseHour(declination);
    data.sunrise = sunrise;
    data.sunset = 24 - sunrise; // Approximate sunset

   
    return data;
}

// Get interpolated weather data for specific hour
WeatherForecast SolarInfo::getWeatherForHour(double hour)
{
    WeatherForecast result;
    int forecastCount = sizeof(forecasts) / sizeof(forecasts[0]);

    // Find nearest forecasts
    int index = 0;
    while (index < forecastCount - 1 && forecasts[index + 1].hour <= hour)
    {
        index++;
    }

    if (index >= forecastCount - 1)
    {
        return forecasts[forecastCount - 1];
    }

    // Linear interpolation
    WeatherForecast before = forecasts[index];
    WeatherForecast after = forecasts[index + 1];
    double ratio = (hour - before.hour) / (after.hour - before.hour);

    result.temperature = before.temperature + (after.temperature - before.temperature) * ratio;
    result.cloudiness = before.cloudiness + (after.cloudiness - before.cloudiness) * ratio;
    result.humidity = before.humidity + (after.humidity - before.humidity) * ratio;
    result.pressure = before.pressure + (after.pressure - before.pressure) * ratio;

    return result;
}

// Calculate atmospheric transmission
double SolarInfo::calculateAtmosphericTransmission(double elevation, WeatherForecast weather)
{
    if (elevation <= 0)
        return 0;

    double AM = 1 / (sin(elevation * PI / 180) + 0.50572 * pow((6.07995 + elevation), -1.6364));

    // Pressure correction
    double pressure_correction = weather.pressure / 1013.25;
    AM *= pressure_correction;

    // Basic atmospheric transmission
    double transmission = pow(0.7, AM);

    // Humidity effect (simplified)
    transmission *= (1.0 - (weather.humidity / 100.0) * 0.1);

    // Cloud cover effect
    double cloud_factor = 1.0 - (weather.cloudiness / 100.0) * 0.75;

    // Turbidity effect
    transmission *= exp(-TURBIDITY_FACTOR * AM / 100);

    // Altitude effect
    transmission *= 1.0 + (altitude / 100000.0);

    return transmission * cloud_factor;
}

// Calculate temperature-adjusted panel efficiency
double SolarInfo::calculateTempAdjustedEfficiency(double baseEfficiency, double temperature)
{
    double tempDiff = temperature - STC_TEMPERATURE;
    double tempEffect = 1.0 + (TEMP_COEFFICIENT * tempDiff);
    return baseEfficiency * tempEffect;
}