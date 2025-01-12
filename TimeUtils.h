#ifndef TimeUtils_h
#define TimeUtils_h
#include <Arduino.h>
struct DSTRule { 
    int month; // Month (1-12) 
    int week; // Week (1-5, 5=last week) 
    int dayOfWeek; // Day of week (0=Sun, 6=Sat) 
    int hour; // Hour (0-23) 
    };

class TimeUtils { 
    private: 
    static int baseOffset; // Base timezone offset in hours 
    static int dstOffset; // Additional DST offset (usually 1 hour) 
    static DSTRule startDST; // When DST starts 
    static DSTRule endDST; // When DST ends

    static DSTRule parseDSTRule(const String& rule);
    static int getDayOfWeek(int year, int month, int day);
    static int getDSTChangeDay(int year, const DSTRule& rule);
    static int getDaysInMonth(int year, int month);

public: 
    static void parseTimezone(const String& tz); 
    static String epochToString(unsigned long epoch);

    static bool isDST(int year, int month, int day, int hour); 
    static unsigned long getEpochTime(int year, int month, int day, int hour, int minute, int second); 
};

#endif