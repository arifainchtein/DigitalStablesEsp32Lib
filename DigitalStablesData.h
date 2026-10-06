
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
#define VOLTAGE_MONITOR 10
#define DAFFODIL_WATER_TROUGH_TANK1 11
#define DAFFODIL_WATER_TROUGH_WATER_TEMP 13  // trough UART ultrasonic on pin18 + waterproof DS18B20 on pin33; water temp (°C) carried in measuredHeight2 (no struct change) - added 2026-10-02, switch position 0001
#define DAFFODIL_2_WATER_TROUGH 12  // 2 independent troughs, UART ultrasonic sensors on Serial1/Serial2 - added 2026-09-01, reclaims switch position 0011 (was a duplicate DAFFODIL_WATER_TROUGH slot). Sensor-reading/display code not yet implemented - pending hardware.

	// const uint8_t tank[] = {
	//   TSEG_F | TSEG_G | TSEG_D | TSEG_E,                  // t
	//   TSEG_C | TSEG_D | TSEG_E | TSEG_B | TSEG_A | TSEG_G,  // a
	//   TSEG_C | TSEG_E | TSEG_G,                          // n
	//   TSEG_G | TSEG_D | TSEG_E                           // c
	// };

	// const uint8_t templabel[] = {

	//   TSEG_F | TSEG_G | TSEG_D | TSEG_E,                  // t
	//   TSEG_A | TSEG_D | TSEG_E | TSEG_F | TSEG_G,
	//    0x00, 0x00  // e
	// };

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
	char devicename[12];
	char deviceshortname[5];
	char groupidentifier[5];
	char sensor1name[6];
	char sensor2name[6];
	uint8_t serialnumberarray[8];
	uint8_t sentbyarray[8];
	uint8_t checksum;
	char deviceTypeId[12];
	long secondsTime=0L;
	uint8_t dataSamplingSec=2;
	int8_t currentFunctionValue=0;
	uint8_t temperature=0;
	float rtcBatVolt=0.0;
	uint8_t opMode=0;
	float rssi=0;
	float snr=0;
	uint8_t operatingStatus=0;
	uint8_t loraActive=0;
	uint8_t ledBrightness=0;
	char ipAddress[12];  // Shrunk from 16 2026-09-01 to fund measuredHeight2/maximumScepticHeight2 (avoids growing sizeof(DigitalStablesData) past 244, which would collide with ChinampaData's 248). Confirmed dead across every project using this struct — none of them ever write to it (each has its own local ipAddress String for its own WiFi/display logic instead) — so there's nothing to truncate. Must shrink by a multiple of 4 to actually reduce sizeof: this field sits right before a 4-byte-aligned member, so a 2-byte trim is silently absorbed as alignment padding instead of freeing anything (verified by compiling the struct — deviceTypeId has the same issue, left at its original size since one 4-byte cut here is enough).
	uint8_t wifiStatus;   // 0=off, 1=AP, 2=STA no internet, 3=STA+internet
	float flowRate=0.0;
	float totalMilliLitres=0.0;

	float flowRate2=0.0;
	float totalMilliLitres2=0.0;

	
	float tank1HeightMeters=.3;
	float tank1maxvollit=0.0;
	float tank1PressurePsi;
	float tank2PressurePsi;
	float tank2HeightMeters=.3;
	float tank2maxvollit=0.0;

	float troughlevelminimumcm=20.0;
	float troughlevelmaximumcm=30.0;
	float panelVoltage=-99;   // Wally USB/panel INA219 (0x45) bus voltage; -99 if sensor absent. Replaces the old scepticAvailablePercentage (was purely derived, never needed on the wire).
	float maximumScepticHeight=0.0;
	float measuredHeight=0.0;
	float maximumScepticHeight2=0.0;  // 2nd independent trough (UART ultrasonic on Serial2) — added 2026-09-01, funded by removing lux and shrinking ipAddress below (estimatedRuntime kept — parsed by AnnabelleDeserializer.java)
	float measuredHeight2=0.0;
	//
	// from aliexpress
	//  25mm flow meter qfactor =1.08   https://www.aliexpress.com/item/32792886446.html
	//  32mm flow meter qfactor = .45   https://www.aliexpress.com/item/4000795880974.html
	//  40mm flow meter qfactor = .45   https://www.aliexpress.com/item/32795067364.html
	//  50mm flow meter qfactor = .2   https://www.alibaba.com/product-detail/YF-DN50-hall-sensor-small-inductive_1600196933068.html

 	float qfactor1=.35;
	float qfactor2=.82;
	long dsLastUpload;
	float latitude;
	float longitude;
	float altitude;
	float batteryVoltage=0.0;
	float v50Voltage=0.0;
	long totpcode;
	float outdoortemperature=0.0;
	float outdoorhumidity=0.0;
	
    bool digitalStablesUpload;
	
	long sleepTime=0; // in seconds — set exclusively by goToSleep()
	uint8_t minimumEfficiencyForLed;
	uint8_t minimumEfficiencyForWifi;
	float batteryCurrent=-99;
	float estimatedRuntime=0.0;  // kept 2026-09-01: parsed by AnnabelleDeserializer (Teleonome), unlike lux which was display-only there
	uint8_t asyncdata=0;
	uint8_t wakeTimeSec=0;
	float panelCurrent=-99;   // Wally USB/panel INA219 (0x45) current, mA; -99 if sensor absent. Freed by shrinking sensor1name/sensor2name from [8] to [6].
};
#endif

// DiagnosticRecord (on-demand TX_CURRENT / I2C_STATUS diagnostics) was retired 2026-10-05 -
// superseded by VitalSignsRecord.h, which is sent after every pulse.
