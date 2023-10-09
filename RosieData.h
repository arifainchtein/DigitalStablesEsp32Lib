#ifndef ROSIECONFIGDATA_H
#define ROSIECONFIGDATA_H
struct RosieConfigData{
	float fieldId=0;
	long commandcode=0;
	char stationName[20];
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif

#ifndef ROSIEDATA_H
#define ROSIEDATA_H
struct RosieData{
	char devicename[16];
	char deviceshortname[4];
	char flow1name[12];
	char flow2name[12];
	char tank1name[12];
	char tank2name[12];
	uint8_t serialnumberarray[8];
	char groupidentifier[12];
	char deviceTypeId[12];
	long secondsTime=0L;
	uint8_t dataSamplingSec=20;
	uint8_t currentFunctionValue=0;
	uint8_t temperature=0;
	float rtcBatVolt=0.0;
	uint8_t opMode=0;
	float rssi=0;
	float snr=0;
	float flowRate=0.0;
	float totalMilliLitres=0.0;

	float flowRate2=0.0;
	float totalMilliLitres2=0.0;

	float tank1PressurePsi=0.0;
	float tank1PressureVolts=0.0;
	float tank1WaterLevel=0.0;
	float tank1HeightMeters=.3;
	float tank1maxvollit;
	float tank1heightmeters;

	float tank2PressurePsi=0.0;
	float tank2PressureVolts=0.0;
	float tank2WaterLevel=0.0;
	float tank2HeightMeters=.3;
	float tank2maxvollit;
	float tank2heightmeters;
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
	float operatingStatus=0;
	float sleepPingMinutes=30;
	long secondsSinceLastPulse;
	bool digitalStablesUpload;
	long dsLastUpload;
	float latitude;
	float longitude;
	float solarVoltage=0.0;
	float capacitorVoltage=0.0;
};
#endif