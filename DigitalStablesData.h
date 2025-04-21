
#include "Arduino.h"
#include <TM1637Display.h>

#ifndef DIGITALSTABLESCONFIGDATA_H
#define DIGITALSTABLESCONFIGDATA_H

#define FUN_1_FLOW 1
#define FUN_2_FLOW 2
#define FUN_1_FLOW_1_TANK 3
#define FUN_1_TANK 4
#define FUN_2_TANK 5
#define DAFFODIL_SCEPTIC_TANK 6
#define DAFFODIL_WATER_TROUGH 7
#define DAFFODIL_TEMP_SOILMOISTURE 8
#define DAFFODIL_LIGHT_DETECTOR 9


const uint8_t tank[] = {
  SEG_F | SEG_G | SEG_D | SEG_E,                  // t
  SEG_C | SEG_D | SEG_E | SEG_B | SEG_A | SEG_G,  // a
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_G | SEG_D | SEG_E                           // c
};

const uint8_t templabel[] = {

  SEG_F | SEG_G | SEG_D | SEG_E,                  // t
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G  // e
};

// #define SEND_ASYNC_DATA 1
// #define RECEIVED_OK 2
// #define CLEARED_OK 3
// #define NO_DATA 4

struct RequestCommand {
    long totpcode = 0;
    char commandString[32]; // 
	uint8_t checksum;
    RequestCommand() {
        commandString[0] = '\0'; 
    }
    void setCommand(const String& command) {
        // Ensure we don't exceed the buffer size (leaving room for null terminator)
        size_t maxLen = sizeof(commandString) - 1;
        size_t copyLen = command.length() < maxLen ? command.length() : maxLen;
        
        // Copy the string data
        memcpy(commandString, command.c_str(), copyLen);
        commandString[copyLen] = '\0'; // Ensure null termination
    }
};


struct DigitalStablesConfigData{
	float fieldId=0;
	long commandcode=0;
	char stationName[20];
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif





#ifndef DIGITALSTABLESDATA_H
#define DIGITALSTABLESDATA_H
struct DigitalStablesData{
	char devicename[16];
	char deviceshortname[5];
	char groupidentifier[8];
	char sensor1name[10];
	char sensor2name[10];
	uint8_t serialnumberarray[8];
	uint8_t sentbyarray[8];
	uint8_t checksum;
	char deviceTypeId[12];
	long secondsTime=0L;
	uint8_t dataSamplingSec=2;
	uint8_t currentFunctionValue=0;
	uint8_t temperature=0;
	float rtcBatVolt=0.0;
	uint8_t opMode=0;
	float rssi=0;
	float snr=0;
	uint8_t operatingStatus=0;
	uint8_t loraActive=0;
	uint8_t ledBrightness=0;
	char ipAddress[16];
	bool internetAvailable;
	float flowRate=0.0;
	float totalMilliLitres=0.0;

	float flowRate2=0.0;
	float totalMilliLitres2=0.0;

	float tank1PressurePsi=0.0;
	float tank1HeightMeters=.3;
	float tank1maxvollit;
	
	float tank2PressurePsi=0.0;
	float tank2HeightMeters=.3;
	float tank2maxvollit;
	//
	// from aliexpress
	//  25mm flow meter qfactor =1.08   https://www.aliexpress.com/item/32792886446.html
	//  32mm flow meter qfactor = .45   https://www.aliexpress.com/item/4000795880974.html
	//  40mm flow meter qfactor = .45   https://www.aliexpress.com/item/32795067364.html
	//  50mm flow meter qfactor = .2   https://www.alibaba.com/product-detail/YF-DN50-hall-sensor-small-inductive_1600196933068.html

 	float qfactor1=.35;
	float qfactor2=.82;
	long secondsSinceLastPulse;
	bool digitalStablesUpload;
	long dsLastUpload;
	float latitude;
	float longitude;
	float altitude;
	float solarVoltage=0.0;
	float capacitorVoltage=0.0;
	long totpcode;
	float outdoortemperature=0.0;
	float outdoorhumidity=0.0;
	float maximumScepticHeight=0.0;
    float measuredHeight=0.0;
	float scepticAvailablePercentage=0.0;
	float lux=0;
	long sleepTime=600; // in seconds
	uint8_t minimumEfficiencyForLed;
	uint8_t minimumEfficiencyForWifi;
	float capacitorCurrent=-99;
	uint8_t asyncdata=0;
};
#endif