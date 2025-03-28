
#include "Arduino.h"

#ifndef GLORIACONFIGDATA_H
#define GLORIACONFIGDATA_H
struct GloriaConfigData{
	float fieldId=0;
	long commandcode=0;
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif


#ifndef GLORIACOMMANDDATA_H
#define GLORIACOMMANDDATA_H
struct GloriaCommandData{
	float fieldId=0;
	long commandcode=0;
	char command[64];
};
#endif

#ifndef GLORIAWIFIDATA_H
#define GLORIAWIFIDATA_H
struct GloriaWifiData{
	float fieldId=0;
	long commandcode=0;
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif

#ifndef GLORIATANKFLOWPUMPDATA_H
#define GLORIATANKFLOWPUMPDATA_H
struct GloriaTankFlowPumpData{
	char devicename[16];
	char deviceshortname[5];
	char flow1name[12];
	char flow2name[12];
	char tank1name[12];
	char tank2name[12];
	uint8_t serialnumberarray[8];
	uint8_t checksum;
	//char groupidentifier[12];
	char deviceTypeId[12];
	long secondsTime=0L;
	uint8_t dataSamplingSec=3;
	uint8_t currentFunctionValue=0;
	uint8_t temperature=0;
	float rtcBatVolt=0.0;
	uint8_t opMode=0;
	float rssi=0.0;
	float snr=0.0;
	uint8_t loraActive=0;
	char ipAddress[16];
	bool internetAvailable;

	float flowRate=0.0;
	float totalMilliLitres=0.0;
	float flowRate2=0.0;
	float totalMilliLitres2=0.0;

	float tank1PressurePsi=0.0;
	//float tank1PressureVolts=0.0;
	//float tank1WaterLevel=0.0;
	float tank1HeightMeters=.3;
	float tank1maxvollit=0;

	float tank2PressurePsi=0.0;
	//float tank2PressureVolts=0.0;
	//float tank2WaterLevel=0.0;
	float tank2HeightMeters=.3;
	float tank2maxvollit=0.0;
	//
	// from aliexpress
	//  25mm flow meter qfactor =1.08   https://www.aliexpress.com/item/32792886446.html
	//  32mm flow meter qfactor = .45   https://www.aliexpress.com/item/4000795880974.html
	//  40mm flow meter qfactor = .45   https://www.aliexpress.com/item/32795067364.html
	//  50mm flow meter qfactor = .2   https://www.alibaba.com/product-detail/YF-DN50-hall-sensor-small-inductive_1600196933068.html

 	float qfactor1=.82;
	float qfactor2=.82;
	// the status values are:
	// 0 - Comma
	// 1 - WPS normal
	// 2 - Active Normal
	int operatingStatus=0;
	int sleepPingMinutes=30;
	int secondsSinceLastPulse=5;
	bool digitalStablesUpload=0;
	long dsLastUpload=0L;
	float latitude=-37.17139;
	float longitude=144.48861;
	float solarvoltage=0.0;
	float capacitorvoltage=0.0;
	bool relaystatus=false;
	long relaysecondson=0;
};
#endif
