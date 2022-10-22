#ifndef SEEDLINGMONITORDATA_H
#define SEEDLINGMONITORDATA_H
struct SeedlingMonitorData{
	char devicename[16];
	char flow1name[16];
	char tank1name[16];
	char groupidentifier[16];
	char deviceTypeId[30];
	float qfactor1=.82;
	float flowRate=0.0;
	float totalMilliLitres=0.0;
	long secondsTime=0L;
	uint8_t currentFunctionValue=0;
	uint8_t dataSamplingSec=3;
	uint8_t temperature=0;
	float reg33Voltage=0.0;
	float rtcBatVolt=0.0;
	uint8_t opMode=0;
	float rssi=0;
	float snr=0;
    float heatIndex=0;
	float dewPoint=0;
    float greenhouseTemp=0.0;
    float greenhouseHum=0.0;
    int soilMoisture;
    float soilTemperature;
	float roomTemperature;
	float sleepPingMinutes=30;
	long secondsSinceLastPulse;
	bool digitalStablesUpload;
	long dsLastUpload;
	float operatingStatus=0;
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