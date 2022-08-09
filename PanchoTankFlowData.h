
#ifndef PANCHOCONFIGDATA_H
#define PANCHOCONFIGDATA_H
struct PanchoConfigData{
	float fieldId=0;
	long commandcode=0;
	float operatingStatus=0;
	float sleepPingMinutes=30;
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
	
	long secondsTime=0L;
	uint8_t dataSamplingSec=10;
	float temperature=0.0;
	float reg33Voltage=0.0;
	float rtcBatVolt=0.0;
	uint8_t opMode=0;
	float rssi=0;
	float snr=0;
	float flowRate=0.0;
	float totalMilliLitres=0.0;
	
	float flowRate2=0.0;
	float totalMilliLitres2=0.0;

	float tankPressurePsi=0.0;
	float tankPressureVolts=0.0;
	float tankWaterLevel=0.0;
	float tankHeightMeters=.3;
	
	float tank2PressurePsi=0.0;
	float tank2PressureVolts=0.0;
	float tank2WaterLevel=0.0;
	float tank2HeightMeters=.3;
	
	float qfactor=.82;
	// the status values are:
	// 0 - Comma
	// 1 - WPS normal
	// 2 - Active Normal
	float operatingStatus=0;
	float sleepPingMinutes=30;
	long secondsSinceLastPulse;
};
#endif
