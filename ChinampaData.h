
#include "Arduino.h"
#ifndef CHINAMPACONFIGDATA_H
#define CHINAMPACONFIGDATA_H

struct ChinampaConfigData{
	float fieldId=0;
	long commandcode=0;
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif


#ifndef CHINAMPACOMMANDDATA_H
#define CHINAMPACOMMANDDATA_H
struct ChinampaCommandData{
	float fieldId=0;
	long commandcode=0;
	char command[64];
};
#endif

#ifndef CHINAMPAWIFIDATA_H
#define CHINAMPAWIFIDATA_H
struct ChinampaWifiData{
	float fieldId=0;
	long commandcode=0;
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif

#ifndef CHINAMPADATA_H
#define CHINAMPADATA_H
struct ChinampaData{
	char devicename[16];
	char deviceshortname[5];
	
	uint8_t serialnumberarray[8];
	uint8_t sentbyarray[8];
	uint8_t checksum;
	char groupidentifier[12];
	char deviceTypeId[12];
	long secondsTime=0L;
	uint8_t dataSamplingSec=5;
	uint8_t microtemperature=0;
	float rtcBatVolt=0.0;
	uint8_t opMode=0;
	float rssi=0.0;
	float snr=0.0;
	uint8_t loraActive=0;
	char ipAddress[16];
	bool internetAvailable;

    //
    // relays
    //
	bool pumprelaystatus;
    bool fishtankoutflowsolenoidrelaystatus;

    // fishtank height
	float minimumFishTankLevel=60.0;
	float maximumFishTankLevel=80.0;
    float fishTankMeasuredHeight=0.0;
	float fishTankHeight=0.0;

	// sumptrough height
		float minimumSumpTroughLevel=60.0;
		float maximumSumpTroughLevel=80.0;
		float sumpTroughMeasuredHeight=0.0;
		float sumpTroughHeight=0.0;

	int sumpTroughStaleDataSeconds=90;
	int fishTankStaleDataSeconds=90;
	
	bool alertstatus;
	//
	// alert codes:
	// 1 Fish Tank Data Stale
	// 2 Sump Trough Stale
	// 
	uint8_t alertcode;
    //
    // flow  sensor
    float fishtankoutflowflowRate=0.0;
	float pumpflowRate=0.0;

	uint8_t operatingStatus=0;
	int secondsSinceLastPulse=5;
	bool digitalStablesUpload=false;
	long dsLastUpload=0;
	float sleepPingMinutes=30;
	float latitude=-37.17139;
	float longitude=144.48861;
	long totpcode;
};
#endif
