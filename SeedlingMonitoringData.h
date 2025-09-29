#ifndef SEEDLINGMONITORDATA_H
#define SEEDLINGMONITORDATA_H
struct SeedlingMonitorData{
	char devicename[16];
	char deviceshortname[5];
	uint8_t serialnumberarray[8];
	char groupidentifier[16];
	char deviceTypeId[30];
	char ipAddress[16];
	long secondsTime=0L;
	int8_t checksum;
	uint8_t currentFunctionValue=0;
	uint8_t dataSamplingSec=3;
	float temperature=0;
	float reg33Voltage=0.0;
	float rtcBatVolt=0.0;
	float rssi=0;
	float snr=0;
    float heatIndex=0;
	float dewPoint=0;
	float outdoorTemperature=0.0;
    float greenhouseTemp=0.0;
    float greenhouseHum=0.0;
	float sleepPingMinutes=30;
	long secondsSinceLastPulse;
	bool digitalStablesUpload;
	long dsLastUpload;
	float operatingStatus=0;
	bool humidifierstatus=false;
	bool loraActive=false;
	bool internetAvailable=false;
	float latitude;
	float longitude;
	long totpcode;
};
#endif

#ifndef SEEDLINGMONITORCOMMANDDATA_H
#define SEEDLINGMONITORCOMMANDDATA_H
struct SeedlingMonitorCommandData{
	float fieldId=0;
	long commandcode=0;
	char command[64];
};
#endif

#ifndef SEEDLINGMONITORCONFIGDATA_H
#define SEEDLINGMONITORCONFIGDATA_H
struct SeedlingMonitorConfigData{
	float fieldId=0;
	long commandcode=0;
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif