#ifndef LANGLEYDATA_H
#define LANGLEYDATA_H
struct LangleyData{

  char devicename[16];
 char deviceshortname[4];
  long secondsTime=0L;
  float capacitorVoltage=0.0;
  float externalBatteryVoltage=0.0;
  float temperature=0.0;
  float fenceVoltage=0.0;
  float rssi=0;
  float snr=0;
  // the status values are:
  // 0 - Comma
  // 1 - WPS normal
  // 2 - Active Normal
  float operatingStatus=0;
  float wpsFrequencySeconds=1800;
  float maxWPSVoltage=3.75;
  float minWPSVoltage=3.25;
  long secondsSinceLastPulse;
};
#endif