


#ifndef LIBRARIES_DIGITALSTABLES_DAFFODILWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_DAFFODILWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
// #include <DaffodilData.h>
//#include <RosieData.h>
#include <DigitalStablesData.h>
#include <atomic>
#include <functional>

#define WEB_PAGE_MAX_ACTIVE 2

class DaffodilWifiManager : public WifiManager{



public:
	DigitalStablesData& digitalStablesData;
    DigitalStablesConfigData& digitalStablesConfigData;
	
	DaffodilWifiManager(HardwareSerial& serial , FS &fs, PCF8563TimeManager& t, Esp32SecretManager& e, DigitalStablesData& tf, DigitalStablesConfigData& p) ;
	void start( );
	void setWifiActiveSwitchStatus(bool b);
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();
	// Power panel of the web page (2026-10-14): /DaffodilServlet?formName=GetPowerData. Filled by
	// the sketch, which holds the battery ledger and vital signs. Runs in the async_tcp task.
	void setPowerDataProvider(std::function<void(JsonDocument&)> provider) { powerDataProvider = provider; }
	// Viewer function (2026-11-06): GET formName=GetViewerConfig fills the JSON (config, field
	// list, what was heard); POST formName=SetViewerConfig, param "config" = the config JSON. The
	// setter returns "" when accepted, otherwise the reason. Both run in the async_tcp task.
	void setViewerDataProvider(std::function<void(JsonDocument&)> provider) { viewerDataProvider = provider; }
	void setViewerConfigSetter(std::function<String(const String&)> setter) { viewerConfigSetter = setter; }

    virtual ~DaffodilWifiManager();

private:
	String okString="Ok";
	std::function<void(JsonDocument&)> powerDataProvider;
	std::function<void(JsonDocument&)> viewerDataProvider;
	std::function<String(const String&)> viewerConfigSetter;
	// The one-file gzip page (build_web.py, 2026-11-06).
	String pageEtag;
	uint8_t pageActive = 0;
	void sendPage(AsyncWebServerRequest *request);
    std::atomic<bool> _isValid{true};

};
#endif /* LIBRARIES_DIGITALSTABLES_DAFFODILWIFIMANAGER_H_ */