#include <LangleyWifiManager.h>
#include <ArduinoJson.h>

LangleyWifiManager::LangleyWifiManager(HardwareSerial &serial, FS &fs, PCF8563TimeManager &t, Esp32SecretManager &e, LangleyData& d) :
WifiManager(serial, fs, t, e), langleyData(d){}

void LangleyWifiManager::setGpsSavedCallback(GpsSavedCallback cb){
	gpsSavedCallback = cb;
}

// "" if the client didn't send the parameter - request->getParam() returns null then, and every
// handler below is reachable by anything on the network.
static String postParam(AsyncWebServerRequest *request, const char *name){
	const AsyncWebParameter *p = request->getParam(name, true);
	return p ? p->value() : String();
}

// Serves one static file from the www partition at the URL path the page's HTML asks for it by.
#define SERVE_FILE(urlPath, filePath) \
	asyncWebServer.on(urlPath, HTTP_GET, [this](AsyncWebServerRequest *request){ \
		request->send(_fs, filePath, String(), false); \
	})

void LangleyWifiManager::start(){
	ssid = secretManager.getSSID();
	password = secretManager.getWifiPassword();
	soft_ap_ssid = secretManager.getSoftAPSSID();
	soft_ap_password = secretManager.getSoftAPPASS();
	hostname = secretManager.getHostName();
	stationmode = secretManager.getStationMode();
	_HardSerial.print("in LangleyWifiManager ssid=");
	_HardSerial.print(ssid);
	_HardSerial.print(" stationmode=");
	_HardSerial.println(stationmode);

	ssids = scanNetworks();
	// mode NULL first so the hostname is applied - https://github.com/espressif/arduino-esp32/issues/6700
	WiFi.mode(WIFI_MODE_NULL);
	if(stationmode && ssid){
		connectSTA();
	}else{
		connectAP();
	}

	SERVE_FILE("/assets/bootstrap/css/bootstrap.min.css", "/bootstrap.min.css");
	SERVE_FILE("/assets/bootstrap/js/bootstrap.min.js", "/bootstrap.min.js");
	SERVE_FILE("/assets/img/Langley.svg", "/Langley.svg");
	SERVE_FILE("/assets/js/jquery.min.js", "/jquery.min.js");
	SERVE_FILE("/assets/css/slideswitch.css", "/slideswitch.css");
	SERVE_FILE("/assets/css/styles.css", "/styles.css");
	SERVE_FILE("/assets/css/Roboto.css", "/Roboto.css");
	SERVE_FILE("/assets/fonts/fa-solid-900.woff2", "/fa-solid-900.woff2");
	SERVE_FILE("/assets/fonts/fontawesome-all.min.css", "/fontawesome-all.min.css");
	SERVE_FILE("/assets/fonts/Roboto-Regular.woff2", "/Roboto-Regular.woff2");
	SERVE_FILE("/js/index.js", "/index.js");
	SERVE_FILE("/", "/index.html");
	SERVE_FILE("/index.html", "/index.html");

	asyncWebServer.on("/LangleyServlet", HTTP_POST, [this](AsyncWebServerRequest *request){
		if(!request->hasParam("formName", true)){
			request->send(400, "text/plain", "Missing formName in POST data");
			return;
		}
		String formName = postParam(request, "formName");
		this->_HardSerial.println("POST formName: " + formName);
		AsyncResponseStream *response = request->beginResponseStream("text/plain");

		if(formName=="ConfigSTA"){
			String newSsid = postParam(request, "ssid");
			String newPassword = postParam(request, "pass");
			String newHost = postParam(request, "host");
			if(this->configWifiSTA(newSsid, newPassword, newHost)){
				this->_HardSerial.println("rebooting after configuring station mode");
				ESP.restart();
			}else{
				this->_HardSerial.println("failed change to station");
			}
		}else if(formName=="ConfigAP"){
			String newApSsid = postParam(request, "apaddress");
			String newPassword = postParam(request, "pass");
			String newHost = postParam(request, "host");
			if(this->configWifiAP(newApSsid, newPassword, newHost)){
				this->_HardSerial.println("rebooting after configuring access point");
				ESP.restart();
			}else{
				this->_HardSerial.println("failed change to ap");
			}
		}else if(formName=="SetTimeViaInternet"){
			setTimeFromInternet();
			DynamicJsonDocument json(3000);
			this->generateWebData(json, serialNumber);
			serializeJson(json, *response);
			request->send(response);
		}else if(formName=="ManualSetTime"){
			timeManager.setTime(postParam(request, "time"));
			DynamicJsonDocument json(3000);
			this->generateWebData(json, serialNumber);
			serializeJson(json, *response);
			request->send(response);
		}else if(formName=="SetGPS"){
			float lat = postParam(request, "lat").toFloat();
			float lon = postParam(request, "long").toFloat();
			langleyData.latitude = lat;
			langleyData.longitude = lon;
			if(gpsSavedCallback != nullptr) gpsSavedCallback(lat, lon);
			DynamicJsonDocument json(3000);
			this->generateWebData(json, serialNumber);
			serializeJson(json, *response);
			request->send(response);
		}else{
			request->send(400, "text/plain", "Unknown formName");
		}
	});

	asyncWebServer.on("/LangleyServlet", HTTP_GET, [this](AsyncWebServerRequest *request){
		if(!request->hasParam("formName")){
			request->send(400, "text/plain", "Missing formName parameter");
			return;
		}
		String formName = request->getParam("formName")->value();
		if(formName=="GetWebData"){
			AsyncResponseStream *response = request->beginResponseStream("text/plain");
			DynamicJsonDocument json(3000);
			this->generateWebData(json, serialNumber);
			serializeJson(json, *response);
			request->send(response);
		}else{
			request->send(400, "text/plain", "Unknown formName");
		}
	});

	asyncWebServer.onNotFound([this](AsyncWebServerRequest *request){
		this->_HardSerial.println("File Not Found: " + request->url());
		request->send(404, "text/plain", "Not found");
	});
	asyncWebServer.begin();
	_HardSerial.println("HTTP server started");
}

void LangleyWifiManager::generateWebData(DynamicJsonDocument& json, String sentBy){
	json["devicename"] = langleyData.devicename;
	json["deviceshortname"] = langleyData.deviceshortname;
	json["secondsTime"] = langleyData.secondsTime;
	json["temperature"] = langleyData.temperature;
	json["rtcBatVolt"] = langleyData.rtcBatVolt;
	json["operatingStatus"] = langleyData.operatingStatus;

	json["fenceVoltage"] = langleyData.fenceVoltage;
	json["fenceVoltageMin"] = langleyData.fenceVoltageMin;
	json["fenceVoltageMax"] = langleyData.fenceVoltageMax;
	json["fenceVoltageAvg"] = langleyData.fenceVoltageAvg;
	json["pulseCount"] = langleyData.pulseCount;
	json["secondsSinceLastPulse"] = langleyData.secondsSinceLastPulse;

	json["batteryVoltage"] = langleyData.batteryVoltage;
	json["batteryCurrentMa"] = langleyData.batteryCurrentMa;
	json["batteryChemistry"] = langleyData.batteryChemistry;
	json["estimatedRuntime"] = langleyData.estimatedRuntime;
	json["overnightMah"] = langleyData.overnightMah;
	json["solarVoltage"] = langleyData.solarVoltage;
	json["solarCurrentMa"] = langleyData.solarCurrentMa;
	json["energizerBatteryVoltage"] = langleyData.energizerBatteryVoltage;
	json["energizerBatteryCurrent"] = langleyData.energizerBatteryCurrent;

	json["rssi"] = langleyData.rssi;
	json["snr"] = langleyData.snr;
	json["parentShortname"] = langleyData.parentShortname;
	json["branchLabel"] = langleyData.branchLabel;
	json["latitude"] = langleyData.latitude;
	json["longitude"] = langleyData.longitude;
	json["altitude"] = langleyData.altitude;

	json["soft_ap_ssid"] = soft_ap_ssid;
	json["serialnumber"] = serialNumber;
	json["sentBy"] = sentBy;
	json["apAddress"] = apAddress;
	json["hostname"] = hostname;
	json["stationmode"] = stationmode;
	json["ssid"] = ssid;
	json["ssids"] = ssids;
	json["lora"] = lora;
	json["internetAvailable"] = internetAvailable;
	json["internetPingTime"] = internetPingTime;
	json["ipAddress"] = ipAddress;
}

// Langley reports upstream over LoRa, not to devices.digitalstables.com - kept only because
// WifiManager declares it pure virtual.
int LangleyWifiManager::uploadDataToDigitalStables(){
	return -1;
}

LangleyWifiManager::~LangleyWifiManager(){
	asyncWebServer.end();
}
