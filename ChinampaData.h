
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
	uint8_t microtemperatureMaximum=75;
	
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

    // fishtank 
	float minimumFishTankLevel=60.0;
	float maximumFishTankLevel=80.0;
    float fishTankMeasuredHeight=0.0;
	float fishTankHeight=0.0;
	int secondsSinceLastFishTankData=0;
	int fishTankStaleDataSeconds=90;
	float previousFishTankMeasuredHeight=-99;
	
	// sumptrough 
	float minimumSumpTroughLevel=60.0;
	float maximumSumpTroughLevel=80.0;
	float sumpTroughMeasuredHeight=0.0;
	float sumpTroughHeight=0.0;
	long fishtankSecondsTime=0L;
	int sumpTroughStaleDataSeconds=90;
	long sumpTroughSecondsTime=0L;
	int secondsSinceLastSumpTroughData=0;
	float previousSumpTroughMeasuredHeight=-99;
	bool alertstatus;
	//
	// alert codes:
	// 0 initializing device
	// 1 Fish Tank Data Stale
	// 2 Sump Trough Stale
	// 3 Fish Tank and Sump Trough Data Stale
	// 4 Fish Solenoid is open and flow is less than 2
	// 5 Sump too low
	
	//  99 value when no alert

	uint8_t alertcode;
	//
	// positions
	// 0 = uTemp Too High
	// 1 = fish Tank Height suden change
	// 2 = Sump Trough suden change
	// 3 = Fish Tank Water Temperature suden Change
	// 4 = Sump Trough Water Temperature suden Change
	// 5 = TDS sudden change
	// 6 = EC sudden change
	// 7 = DO sudden change
	// 8 = PH sudden change
	// 9 = ORP sudden change

	bool sensorstatus[12];
    //
    // flow  sensor
    float fishtankoutflowflowRate=0.0;
	float fishtankoutPulsePerMinute=0.0;
	float fishtankoutQFactor=63;

	float pumpflowRate=0.0;

	int secondsSinceLastPulse=5;
	bool digitalStablesUpload=false;
	long dsLastUpload=0;
	float sleepPingMinutes=30;
	float latitude=-37.17139;
	float longitude=144.48861;
	long totpcode;
	float outdoortemperature=0.0;
	float outdoorhumidity=0.0;
	 uint8_t operatingStatus=0;

};
#endif
