#ifndef SEEDLINGMONITORDATA_H
#define SEEDLINGMONITORDATA_H
struct SeedlingMonitorData{
	
	long secondsTime=0L;
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
};
#endif