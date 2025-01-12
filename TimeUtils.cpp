#include "TimeUtils.h"
// Initialize static members 
int TimeUtils::baseOffset = 0; 
int TimeUtils::dstOffset = 0; 
DSTRule TimeUtils::startDST = {0, 0, 0, 0}; 
DSTRule TimeUtils::endDST = {0, 0, 0, 0};


String TimeUtils::epochToString(unsigned long epoch) {
    // First convert epoch to UTC
    unsigned long seconds = epoch;
    uint8_t second = seconds % 60;
    seconds /= 60;
    uint8_t minute = seconds % 60;
    seconds /= 60;
    uint8_t hour = seconds % 24;
    unsigned long days = seconds / 24;

    // Convert days to year/month/day
    int year = 1970;
    while (true) {
        unsigned long daysInYear = 365;
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            daysInYear = 366;
        }
        if (days < daysInYear) {
            break;
        }
        days -= daysInYear;
        year++;
    }

    // Create a modifiable copy of the days in month array
    uint8_t monthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    uint8_t month = 1;
    
    // Adjust February for leap year
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        monthDays[1] = 29;
    }

    while (days >= monthDays[month-1]) {
        days -= monthDays[month-1];
        month++;
    }
    uint8_t day = days + 1;

    // Now we have UTC time, convert to local time
    int totalOffset = baseOffset;
    if (isDST(year, month, day, hour)) {
        totalOffset += dstOffset;
    }

    // Add the offset hours
    hour += totalOffset;

    // Handle day rollover
    if (hour >= 24) {
        hour -= 24;
        day++;
        if (day > monthDays[month-1]) {
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
    }
    else if (hour < 0) {
        hour += 24;
        day--;
        if (day < 1) {
            month--;
            if (month < 1) {
                month = 12;
                year--;
                // Adjust February for leap year if we went back to previous year
                if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                    monthDays[1] = 29;
                } else {
                    monthDays[1] = 28;
                }
            }
            day = monthDays[month-1];
        }
    }

    // Format the string
    char buffer[25];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d", 
             year, month, day, hour, minute, second);
    
    return String(buffer);
}

DSTRule TimeUtils::parseDSTRule(const String& rule) { 
    DSTRule dst = {0, 0, 0, 0}; 
    if (rule.length() < 6) return dst;
    int firstDot = rule.indexOf('.');
    int secondDot = rule.indexOf('.', firstDot + 1);
    int slash = rule.indexOf('/');
    if (slash < 0) slash = rule.length();
    dst.month = rule.substring(1, firstDot).toInt();
    dst.week = rule.substring(firstDot + 1, secondDot).toInt();
    dst.dayOfWeek = rule.substring(secondDot + 1, slash).toInt();
    dst.hour = (slash < rule.length()) ? rule.substring(slash + 1).toInt() : 2;
    return dst;
}

int TimeUtils::getDayOfWeek(int year, int month, int day) { 
    static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4}; 
    year -= month < 3; return (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7; 
}

int TimeUtils::getDSTChangeDay(int year, const DSTRule& rule) { 
    int firstDay = 1; int firstDOW = getDayOfWeek(year, rule.month, firstDay); int targetDay = firstDay + (rule.dayOfWeek - firstDOW + 7) % 7;

    targetDay += (rule.week - 1) * 7;
    if (rule.week == 5) { 
        while (targetDay + 7 <= getDaysInMonth(year, rule.month)) { 
            targetDay += 7; 
        } 
    }
    return targetDay;
}

int TimeUtils::getDaysInMonth(int year, int month) {
    const uint8_t daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    
    // February in leap year
    if (month == 2) {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            return 29;
        }
    }
    
    return daysInMonth[month - 1];
}

void TimeUtils::parseTimezone(const String& tz) { 
    int signPos = tz.indexOf('+'); 
    if (signPos < 0) signPos = tz.indexOf('-');

    if (signPos >= 0) {
        int commaPos = tz.indexOf(',');
        if (commaPos < 0) commaPos = tz.length();
        
        String offsetStr = tz.substring(signPos + 1, commaPos);
        baseOffset = offsetStr.toInt();
        if (tz.charAt(signPos) == '-') baseOffset = baseOffset;
        else baseOffset = -baseOffset;
    }
    int firstComma = tz.indexOf(','); int secondComma = tz.indexOf(',', firstComma + 1);
    if (firstComma > 0 && secondComma > 0) {
        String startRule = tz.substring(firstComma + 1, secondComma);
        String endRule = tz.substring(secondComma + 1);
        
        startDST = parseDSTRule(startRule);
        endDST = parseDSTRule(endRule);
        dstOffset = 1;
    } else {
        dstOffset = 0;
    }
}

bool TimeUtils::isDST(int year, int month, int day, int hour) {
    if (dstOffset == 0) return false;
    
    int startDay = getDSTChangeDay(year, startDST);
    int endDay = getDSTChangeDay(year, endDST);
    
    // Southern hemisphere (e.g., Australia where DST starts later in year)
    if (startDST.month > endDST.month) {
        // If we're between January and end date, or between start date and December
        if (month < endDST.month || month > startDST.month) {
            return true;
        }
        // If we're in the end month, check before the end day/hour
        if (month == endDST.month) {
            return day < endDay || (day == endDay && hour < endDST.hour);
        }
        // If we're in the start month, check after the start day/hour
        if (month == startDST.month) {
            return day > startDay || (day == startDay && hour >= startDST.hour);
        }
    }
    // Northern hemisphere
    else {
        // If we're between start and end month
        if (month > startDST.month && month < endDST.month) {
            return true;
        }
        // If we're in the start month, check after the start day/hour
        if (month == startDST.month) {
            return day > startDay || (day == startDay && hour >= startDST.hour);
        }
        // If we're in the end month, check before the end day/hour
        if (month == endDST.month) {
            return day < endDay || (day == endDay && hour < endDST.hour);
        }
    }
    
    return false;
}


unsigned long TimeUtils::getEpochTime(int year, int month, int day, int hour, int minute, int second) {
    bool isDSTActive = isDST(year, month, day, hour);
    int totalOffset = baseOffset;  // For AEST-10, this is 10
    if (isDSTActive) {
        totalOffset += dstOffset;  // During DST, offset becomes 11
    }

    // For Melbourne (UTC+10/+11), when converting to UTC we need to go backwards
    // Example: If it's 6:03 in Melbourne (UTC+11 during DST)
    // Then it's 19:03 the previous day in UTC (6:03 - 11 = -4:57, which means 19:03 previous day)
    
    int utcHour = hour - totalOffset;
    int utcDay = day;
    int utcMonth = month;
    int utcYear = year;

    // Handle negative hours (going back to previous day)
    while (utcHour < 0) {
        utcHour += 24;
        utcDay--;
        if (utcDay < 1) {
            utcMonth--;
            if (utcMonth < 1) {
                utcMonth = 12;
                utcYear--;
            }
            utcDay = getDaysInMonth(utcYear, utcMonth);
        }
    }

    // Calculate days since epoch
    long yearsSince1970 = utcYear - 1970;
    long leapYears = (yearsSince1970 + 2) / 4;
    if (utcYear % 4 == 0 && utcMonth <= 2) {
        leapYears--;
    }
    
    long daysInPreviousYears = yearsSince1970 * 365L + leapYears;
    
    const uint8_t daysInMonth[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    long daysInCurrentYear = 0;
    
    for (int i = 1; i < utcMonth; i++) {
        daysInCurrentYear += daysInMonth[i];
    }
    daysInCurrentYear += utcDay - 1;

    long totalDays = daysInPreviousYears + daysInCurrentYear;
    
    return (totalDays * 86400L) + 
           (utcHour * 3600L) + 
           (minute * 60L) + 
           second;
}