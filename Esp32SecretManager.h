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
	void getChinampaParameters( double& fishq) ;
	void saveChinampaParameters( double& fishq) ;

	void getDeviceSensorConfig(char* devicename, char* deviceshortname, char* sensor1name, char* sensor2name, String& timezone,  double& latitude, double& longitude, double& altitude,uint8_t& minimumEfficiencyForLed, uint8_t& minimumEfficiencyForWifi);
	void saveDeviceSensorConfig(String devicename,String deviceshortname, String sensor1name, String sensor2name, String tz, double latitude, double longitude, double altitude,uint8_t minimumEfficiencyForLed, uint8_t minimumEfficiencyForWifi);
	void saveWifiParameters(String ssid, String password, String softAPSSID, String softAPPASS,  String hostName, bool stationmode);

	void getDeviceConfig(char* devicename, char* deviceshortname, String& timezone,  double& latitude, double& longitude);
	void saveDeviceConfig(String devicename,String deviceshortname, String tz, double latitude, double longitude);
	void saveTroughParameters(double troughcolumnheight, double throughlevelminimumcm, double throughlevelmaximumcm);
	void getTroughParameters( double& troughcolumnheight, double& throughlevelminimumcm, double& throughlevelmaximumcm) ;
 
	void saveConfigData(float fieldId, String stationName );
	void saveOperatingStatus(float operatingStatus );
	float getOperatingStatus();
	float  getFieldId();
    String getStationName();  
	void saveSleepPingMinutes(float pingMinutes );
	float getSleepPingMinutes();
	void setTimeZone(String s);
	String readTimeZone( );
	void saveDeviceName(String devicename);
	void saveDeviceShortName(String deviceshortname);
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

	// Fence topology mapping (Langley) - own namespace, deliberately separate from
	// {save,get}DeviceSensorConfig so that adding this never touches that shared signature
	// (also used by Daffodil/Cajal/Rosie/ChinampaMonitor/gloria-tank-flow-pump/pancho-tank-flow/
	// Annabelle) - see conversation 2026-07-26.
	void saveTopologyConfig(String parentShortname, String branchLabel);
	void getTopologyConfig(char* parentShortname, char* branchLabel);

	// Product definition snapshot injected by Factory at manufacture time (Get Serial Number step).
	// Own namespace ("productdef"), same namespace/keys serialnumberextractor.ino writes directly via
	// raw Preferences - kept here too so Daffodil/Langley can both receive it directly (SetProductDefinition)
	// and read it back for the Inspect flow (GetProductDefinition), without depending on flashing order.
	void saveProductDefinition(String name, String powerSource, String battery, String pcbs, String firmware);
	void getProductDefinition(String& name, String& powerSource, String& battery, String& pcbs, String& firmware);
	unsigned long getCommissionDate();
	void setCommissionDate(unsigned long epochSeconds);

	// Per-device CSW (config switch) calibration reference (Daffodil) - own namespace. The raw
	// ADC value read at the 00000 switch position (all off, no solar) scales proportionally with
	// whatever this specific unit's actual battery/boost-converter output happens to be, so a
	// single shared threshold table drifts between units. Storing one reference reading per
	// device (captured via a one-time install-time calibration step) lets firmware scale the
	// whole table to match, instead of resweeping all 32 positions per battery. 0 = not yet
	// calibrated (caller should fall back to using the table unscaled).
	void saveCSWReference(int32_t referenceRawValue);
	int32_t getCSWReference();
	// The serial command that requests calibration can't safely read the ladder itself - it runs
	// mid-runtime (WiFi/LoRa already up), a different electrical load on V50/DEVICE_POWER than
	// the actual early-boot decode ever sees (confirmed 2026-09-01: a mid-runtime read landed
	// right back on the ORIGINAL unscaled calibration point while the same boot's early-setup
	// reading matched the properly-scaled value). So the command only arms a flag here; setup()
	// checks it and captures the reference at the exact same point in boot as the real decode.
	void armCSWCalibration();
	bool isCSWCalibrationArmed();
	void clearCSWCalibrationArm();

	virtual ~Esp32SecretManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_ESP32SECRETMANAGER_H_ */
