#ifndef LIBRARIES_DIGITALSTABLES_LANGLEYWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_LANGLEYWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
#include <LangleyData.h>
#include <functional>

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
	// Fills the Power panel (the bolt under the cog) - same hook as DaffodilWifiManager. Runs on the
	// web server task, so the provider must only copy values the sketch has already read.
	void setPowerDataProvider(std::function<void(JsonDocument&)> provider) { powerDataProvider = provider; }
	// Relay card (2026-11-11): status fills GetWebData's "relay" object; set is called by the page's
	// SetRelay (on, strong-signal limit in dBm or 1 = keep it, minimum battery volts to stay awake for
	// the relay or NAN = keep it). Both run on the web server task.
	void setRelayHandlers(std::function<void(JsonObject)> status, std::function<void(bool, int, float)> set) {
		relayStatus = status;
		relaySet = set;
	}
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();

	virtual ~LangleyWifiManager();

private:
	GpsSavedCallback gpsSavedCallback = nullptr;
	std::function<void(JsonDocument&)> powerDataProvider;
	std::function<void(JsonObject)> relayStatus;
	std::function<void(bool, int, float)> relaySet;
};
#endif /* LIBRARIES_DIGITALSTABLES_LANGLEYWIFIMANAGER_H_ */
