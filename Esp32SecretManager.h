/*
 * Esp32SecretManager.h
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_ESP32SECRETMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_ESP32SECRETMANAGER_H_
#include <SecretManager.h>
#include <TimeManager.h>
#include "Arduino.h"

class Esp32SecretManager   : public SecretManager{


public:

	Esp32SecretManager(TimeManager & t) ;
	void saveSecret(String secret, int numberDigits, int periodSeconds );
	String readSecret();
	void getDeviceSensorConfig(char* devicename, char* deviceshortname, char* sensor1name, char* sensor2name, String& timezone,  double& latitude, double& longitude);
	void saveDeviceSensorConfig(String devicename,String deviceshortname, String sensor1name, String sensor2name, String tz, double latitude, double longitude);
	void saveWifiParameters(String ssid, String password, String softAPSSID, String softAPPASS,  String hostName, bool stationmode);

	void saveConfigData(float fieldId, String stationName );
	void saveOperatingStatus(float operatingStatus );
	float getOperatingStatus();
	float  getFieldId();
    String getStationName();  
	void saveSleepPingMinutes(float pingMinutes );
	float getSleepPingMinutes();
	void setTimeZone(String s);
	String readTimeZone( );
	String readDeviceName();
	String readDeviceShortName();
	String readFlow1Name();
	String readFlow2Name();
	String readTank1Name();
	String readTank2Name();

	String getSSID();
	String getWifiPassword();
	String getSoftAPSSID();
	String getSoftAPPASS();
	String getHostName();
	String getGroupIdentifier();
	void setGroupIdentifier(String groupIdentifier);
	bool getStationMode();
	virtual ~Esp32SecretManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_ESP32SECRETMANAGER_H_ */
