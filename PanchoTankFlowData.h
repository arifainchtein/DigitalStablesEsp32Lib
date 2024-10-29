
#include "Arduino.h"

#ifndef PANCHOCONFIGDATA_H
#define PANCHOCONFIGDATA_H
struct PanchoConfigData{
	float fieldId=0;
	long commandcode=0;
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif


#ifndef PANCHOCOMMANDDATA_H
#define PANCHOCOMMANDDATA_H
struct PanchoCommandData{
	float fieldId=0;
	long commandcode=0;
	char command[64];
};
#endif

#ifndef PANCHOWIFIDATA_H
#define PANCHOWIFIDATA_H
struct PanchoWifiData{
	float fieldId=0;
	long commandcode=0;
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif

#ifndef PANCHOTANKFLOWDATA_H
#define PANCHOTANKFLOWDATA_H
struct PanchoTankFlowData{
	char devicename[16];
	char deviceshortname[5];
	char flow1name[10];
	char flow2name[10];
	char tank1name[10];
	char tank2name[10];
	uint8_t serialnumberarray[8];
	uint8_t checksum;
	char groupidentifier[12];
	char deviceTypeId[12];
	long secondsTime=0L;
	uint8_t dataSamplingSec=20;
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
	float tank1PressureVolts=0.0;
	float tank1WaterLevel=0.0;
	float tank1HeightMeters=.31;
	float tank1maxvollit=0.0;
	float tank1heightmeters=0.0;

	float tank2PressurePsi=0.0;
	float tank2PressureVolts=0.0;
	float tank2WaterLevel=0.0;
	float tank2HeightMeters=.32;
	float tank2maxvollit=0.0;
	float tank2heightmeters=0.0;
	//
	// from aliexpress
	//  25mm flow meter qfactor =1.08   https://www.aliexpress.com/item/32792886446.html
	//  32mm flow meter qfactor = .45   https://www.aliexpress.com/item/4000795880974.html
	//  40mm flow meter qfactor = .45   https://www.aliexpress.com/item/32795067364.html
	//  50mm flow meter qfactor = .2   https://www.alibaba.com/product-detail/YF-DN50-hall-sensor-small-inductive_1600196933068.html

 	float qfactor1=.82;
	float qfactor2=.82;
	// the status values are:
	// 0 - Active
	// 1 - WPS normal
	uint8_t operatingStatus=0;
	int secondsSinceLastPulse=5;
	bool digitalStablesUpload=false;
	long dsLastUpload=0;
	
	float latitude=-37.17139;
	float longitude=144.48861;

};
#endif
