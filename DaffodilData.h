
#include "Arduino.h"
#ifndef DAFFODILCONFIGDATA_H
#define DAFFODILCONFIGDATA_H

struct DaffodilConfigData{
	float fieldId=0;
	long commandcode=0;
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif


#ifndef DAFFODILCOMMANDDATA_H
#define DAFFODILCOMMANDDATA_H
struct DaffodilCommandData{
	float fieldId=0;
	long commandcode=0;
	char command[64];
};
#endif

#ifndef DAFFODILWIFIDATA_H
#define DAFFODILWIFIDATA_H
struct DaffodilWifiData{
	float fieldId=0;
	long commandcode=0;
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif

#ifndef DAFFODILTANKFLOWDATA_H
#define DAFFODILTANKFLOWDATA_H
struct DaffodilData{
	char devicename[16];
	char deviceshortname[5];
	char flow1name[10];
	char flow2name[10];
	char tank1name[10];
	char tank2name[10];

	uint8_t serialnumberarray[8];
	uint8_t sentbyarray[8];
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
	float outdoortemperature=0.0;
	float outdoorhumidity=0.0;
	float minimumSepticHeight=0.0;
	float maximumScepticHeight=0.0;
    float measuredHeight=0.0;
	float scepticAvailablePercentage=0.0;
	// the status values are:
	// 0 - Active
	// 1 - WPS normal
	uint8_t operatingStatus=0;
	int secondsSinceLastPulse=5;
	bool digitalStablesUpload=false;
	long dsLastUpload=0;
	float sleepPingMinutes=30;
	float latitude=-37.17139;
	float longitude=144.48861;
    float capacitorVoltage=0.0;
	long totpcode;
};
#endif
