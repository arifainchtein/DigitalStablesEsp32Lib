
#ifndef PANCHOCONFIGDATA_H
#define PANCHOCONFIGDATA_H
struct PanchoConfigData{
	float fieldId=0;
	long commandcode=0;
	char stationName[20];
	float operatingStatus=0;
	float sleepPingMinutes=30;
};
#endif

#ifndef TANKFLOWDATA_H
#define TANKFLOWDATA_H
struct TankFlowData{
	char stationName[20];
	uint8_t rosieId[8];
	float fieldId=0;
	long secondsTime=0L;
	float capacitorVoltage=0.0;
	uint8_t dataSamplingSec=10;
	float temperature=0.0;
	float solarVoltage=0.0;
	float reg33Voltage=0.0;
	float rtcBatVolt=0.0;
	uint8_t opMode=0;
	uint8_t lowVoltageAlert=0;
	float rssi=0;
	float snr=0;
	float flowRate=0.0;
	float totalMilliLitres=0.0;
	float flowRate2=0.0;
	float totalMilliLitres2=0.0;
	float tankPressurePsi=0.0;
	float tankPressureVolts=0.0;
	float tankWaterLevel=0.0;
	float qfactor=.82;
	float tankHeightMeters=.3;
	// the status values are:
	// 0 - Comma
	// 1 - WPS normal
	// 2 - Active Normal
	float operatingStatus=0;
	float sleepPingMinutes=30;
	long secondsSinceLastPulse;
};
#endif
