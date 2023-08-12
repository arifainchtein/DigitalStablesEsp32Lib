

#include "Arduino.h"


#ifndef CAJALDATA_H
#define CAJALDATA_H
struct CajalData{
	char devicename[16];
	char deviceshortname[4];
	uint8_t serialnumberarray[8];
	char groupidentifier[12];
	char deviceTypeId[12];
	long secondsTime=0L;
	uint8_t dataSamplingSec=3;
	uint8_t currentFunctionValue=0;
	uint8_t temperature=0;
	float rtcBatVolt=0.0;
	uint8_t opMode=0;
	float rssi=0;
	float snr=0;
	
	// the status values are:
	// 0 - Comma
	// 1 - WPS normal
	// 2 - Active Normal
	int operatingStatus=0;
	int secondsSinceLastPulse=5;
	bool digitalStablesUpload=false;
	long dsLastUpload=0;
	
	float latitude=-37.17139;
	float longitude=144.48861;

};
#endif
