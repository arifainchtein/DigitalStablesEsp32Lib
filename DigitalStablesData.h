
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
	char ipAddress[16];
	uint8_t wifiStatus;   // 0=off, 1=AP, 2=STA no internet, 3=STA+internet
	float flowRate=0.0;
	float totalMilliLitres=0.0;

	float flowRate2=0.0;
	float totalMilliLitres2=0.0;

	
	float tank1HeightMeters=.3;
	float tank1maxvollit;
	float tank1PressurePsi;
	float tank2PressurePsi;
	float tank2HeightMeters=.3;
	float tank2maxvollit;

	float troughlevelminimumcm=20.0;
	float troughlevelmaximumcm=30.0;
	float panelVoltage=-99;   // Wally USB/panel INA219 (0x45) bus voltage; -99 if sensor absent. Replaces the old scepticAvailablePercentage (was purely derived, never needed on the wire).
	float maximumScepticHeight=0.0;
	float measuredHeight=0.0;
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
	
	float lux=0;
	long sleepTime=0; // in seconds — set exclusively by goToSleep()
	uint8_t minimumEfficiencyForLed;
	uint8_t minimumEfficiencyForWifi;
	float batteryCurrent=-99;
	float estimatedRuntime=0.0;
	uint8_t asyncdata=0;
	uint8_t wakeTimeSec=0;
	float panelCurrent=-99;   // Wally USB/panel INA219 (0x45) current, mA; -99 if sensor absent. Freed by shrinking sensor1name/sensor2name from [8] to [6].
};
#endif


#ifndef DIAGNOSTICRECORD_H
#define DIAGNOSTICRECORD_H

// Generic on-demand diagnostics, triggered remotely via a RequestCommand
// ("EnableDiagnostics#<type>" / "DisableDiagnostics") — NOT part of the normal periodic
// uplink. diagnosticType selects which member of the payload union is populated; add new
// diagnostic kinds by adding a payload struct + union member + DIAGNOSTIC_TYPE_* constant,
// not a new top-level LoRa packet type/size.
#define DIAGNOSTIC_TYPE_NONE        0
#define DIAGNOSTIC_TYPE_TX_CURRENT  1
#define DIAGNOSTIC_TYPE_I2C_STATUS  2

#define DIAGNOSTIC_TX_MAX_SAMPLES 20

#pragma pack(push, 1)
struct TxCurrentSample {
    uint16_t offsetMs;    // ms since LoRa.beginPacket()
    uint16_t milliamps;   // INA219 current_mA at that offset
};

struct TxCurrentDiagnosticPayload {
    uint16_t v50i_mV;     // V50_I sampled just before keying up, millivolts
    uint16_t mAPre;       // battery current just before keying up
    uint16_t mAPost;      // battery current just after TX completed
    uint8_t  sampleCount; // valid entries in samples[]
    TxCurrentSample samples[DIAGNOSTIC_TX_MAX_SAMPLES];
};

// Snapshot of the boot-time I2C scan — which sensors were actually found. Unlike TX-current,
// this needs no sampling window: it's ready the instant EnableDiagnostics#2 is processed.
// Bit assignments (see buildI2CStatusMask() in Daffodil.ino). DS18B20 is OneWire, not I2C,
// and is deliberately excluded — bit7 is reserved.
//   bit0=lcd(0x03) bit1=temp(0x40) bit2=ADS1115(0x48) bit3=BH1750(0x23)
//   bit4=INA219 battery(0x41) bit5=PCF8563T(0x51) bit6=INA219 solar(0x45) bit7=reserved
struct I2CStatusDiagnosticPayload {
    uint8_t deviceFoundMask;
};

// Total size: 4 (totpcode) + 8 (serialnumberarray) + 1 (diagnosticType)
//           + 87 (payload union, sized to TxCurrentDiagnosticPayload) + 1 (checksum) = 101 bytes.
// Must stay unique relative to other LoRa packet structs (DigitalStablesData, RequestCommand,
// WeatherForecastUpdate, GraveyardShiftUpdate) since receivers dispatch on packet size.
struct DiagnosticRecord {
    long    totpcode = 0;
    uint8_t serialnumberarray[8];
    uint8_t diagnosticType = DIAGNOSTIC_TYPE_NONE;
    union DiagnosticPayload {
        TxCurrentDiagnosticPayload txCurrent;
        I2CStatusDiagnosticPayload i2cStatus;
        DiagnosticPayload() : txCurrent() {}
    } payload;
    uint8_t checksum = 0;
};
#pragma pack(pop)

#endif