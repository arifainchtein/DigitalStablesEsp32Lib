#ifndef LIBRARIES_DIGITALSTABLES_LANGLEYWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_LANGLEYWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
#include <LangleyData.h>

// Langley's web page - same WiFi behaviour as DaffodilWifiManager (station or access-point mode,
// configurable from the page, mDNS hostname, static files served from the firmware's own "www"
// partition), but reading LangleyData and with none of Daffodil's flow/tank/trough forms.
class LangleyWifiManager : public WifiManager{

public:
	LangleyData& langleyData;

	// Called (from the web server task) when the page saves new GPS coordinates, so the sketch can
	// persist them and update its own copies - only the sketch owns the full device-config record.
	typedef void (*GpsSavedCallback)(float latitude, float longitude);

	LangleyWifiManager(HardwareSerial& serial, FS &fs, PCF8563TimeManager& t, Esp32SecretManager& e, LangleyData& d);
	void start();
	void setGpsSavedCallback(GpsSavedCallback cb);
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();

	virtual ~LangleyWifiManager();

private:
	GpsSavedCallback gpsSavedCallback = nullptr;
};
#endif /* LIBRARIES_DIGITALSTABLES_LANGLEYWIFIMANAGER_H_ */
