#include <AnnabelleWifiManager.h>
#include "esp_heap_caps.h"
#include <ArduinoJson.h>
#include <ArduinoJson.h>
//#include <LittleFS.h>
//#define _fs LittleFS


AnnabelleWifiManager::AnnabelleWifiManager(HardwareSerial &serial, FS &fs,DataManager &d, PCF8563TimeManager &t, Esp32SecretManager &e, AnnabelleData &tf) : WifiManager(serial, fs,t, e), dataManager(d), annabelleData(tf) {}



void AnnabelleWifiManager::start()
{

   
    ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();
    ;
    soft_ap_ssid = secretManager.getSoftAPSSID();
    soft_ap_password = secretManager.getSoftAPPASS();
    hostname = secretManager.getHostName();
    stationmode = secretManager.getStationMode();
    if(debug) _HardSerial.println("ssid=");
    if(debug) _HardSerial.println(ssid);
    if(debug) _HardSerial.println("stationmode=");
    if(debug) _HardSerial.println(stationmode);

    if(debug) _HardSerial.print("in  AnnabelleWifiManager ssid=");
    if(debug) _HardSerial.print(ssid);
    if(debug) _HardSerial.print(" stationmode=");
    if(debug) _HardSerial.println(stationmode);
    if(debug) _HardSerial.println("about to do start scan");
    ssids = scanNetworks();
    //
    // set the mode to null so that
    // the hostname is applied
    //  https://github.com/espressif/arduino-esp32/issues/6700
    WiFi.mode(WIFI_MODE_NULL);
    // A station that can't connect (wrong password saved from the web page, router gone) leaves
    // Annabelle unreachable except over the Pi's serial line - come up as an access point instead.
    if (!(stationmode && ssid != "" && connectSTA()))
    {
        if(debug) _HardSerial.print("ssids=");
        if(debug) _HardSerial.println(ssids);

        if(debug) _HardSerial.println("about to do connectAP");
        connectAP();
    }

    // Serial is the Pi's command line on Annabelle - nothing here prints unless debug is set.
    // The whole page - Bootstrap, scripts, images - is one file, data/index.html.gz built from web/
    // by build_web.py (2026-10-22): the browser used to open 6 connections for it at once, which
    // ran Annabelle out of memory. Every handler goes through gated() (see WEB_MAX_ACTIVE).
    asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        gated(request, [this](AsyncWebServerRequest *r) { sendPage(r); });
    });

    asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request){
        gated(request, [this](AsyncWebServerRequest *r) { sendPage(r); });
    });

    asyncWebServer.on("/GetHostName", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/plain", hostname);
    });

    // Settings page (data/settings.js). Parameters are read by name - the old handler took
    // formName from position 1, which is the SSID, so no form ever matched.
    asyncWebServer.on("/AnnabelleServlet", HTTP_POST, [this](AsyncWebServerRequest *request){
      gated(request, [this](AsyncWebServerRequest *request) {
        auto arg = [request](const char *name) -> String {
            return request->hasParam(name, true) ? request->getParam(name, true)->value() : String();
        };
        String formName = arg("formName");
        if(debug) _HardSerial.println("AnnabelleServlet POST " + formName);

        if (formName == "ConfigSTA" || formName == "ConfigAP") {
            String host = arg("host");
            if (host == "") {
                request->send(400, "text/plain", "Host name missing");
                return;
            }
            // Saved, then a restart - start() connects with the saved settings and falls back to
            // the access point if the station can't connect. Connecting from here would drop
            // the connection this reply goes out on.
            if (formName == "ConfigSTA") {
                String newSsid = arg("ssid");
                if (newSsid == "") {
                    request->send(400, "text/plain", "SSID missing");
                    return;
                }
                String apSsid = secretManager.getSoftAPSSID();
                secretManager.saveWifiParameters(newSsid, arg("pass"), apSsid, secretManager.getSoftAPPASS(), host, true);
            } else {
                String apSsid = arg("apaddress");
                if (apSsid == "") {
                    request->send(400, "text/plain", "Access point name missing");
                    return;
                }
                secretManager.saveWifiParameters(secretManager.getSSID(), secretManager.getWifiPassword(), apSsid, arg("pass"), host, false);
            }
            request->send(200, "text/plain", "Saved - Annabelle is restarting");
            xTaskCreate([](void *) {
                vTaskDelay(pdMS_TO_TICKS(1500));
                ESP.restart();
            }, "webRestart", 2048, nullptr, 1, nullptr);
            return;
        }

        // Clock and GPS change the RTC and preferences, which the sketch only touches from
        // loop() - it queues them and the page reads the outcome from GetWebData.
        if (settingsAction) {
            String reply = formName == "DeviceWifi"
                ? settingsAction(formName, arg("serial"), arg("state"))  // telepathons.js WiFi button
                : settingsAction(formName, arg(formName == "SetGPS" ? "lat" : "time"), arg("long"));
            if (reply != "") {
                request->send(200, "text/plain", reply);
                return;
            }
        }
        request->send(400, "text/plain", "Unknown form " + formName);
      });
    });

    asyncWebServer.on("/AnnabelleServlet", HTTP_GET, [this](AsyncWebServerRequest *request){
      gated(request, [this](AsyncWebServerRequest *request) {
        String formName = request->hasParam("formName") ? request->getParam("formName")->value() : String();
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        if (formName == "GetWebData"){
            DynamicJsonDocument json(2600);
            this->generateWebData(json, serialNumber);
            serializeJson(json, *response);
            request->send(response);
        } else if (formName == "GetDevicesData"){
            serializeJson(dataManager.completeObject, *response);
            request->send(response);
        } else {
            request->send(400, "text/plain", "Unknown form " + formName);
        }
      });
    });

    asyncWebServer.on("/GetSensorData", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/plain", sensorString);
    });

    // The page's ETag (size and time of index.html.gz): a reload the browser already has gets 304.
    File page = _fs.open("/index.html.gz", "r");
    if (page) {
        char etag[24];
        snprintf(etag, sizeof(etag), "\"%x-%lx\"", (unsigned)page.size(), (unsigned long)page.getLastWrite());
        pageEtag = etag;
        page.close();
    }

    // The fixed reply memory - allocated once, never freed (see WebJsonArena in the header).
    if (!jsonArena.base) {
        jsonArena.base = (uint8_t *)heap_caps_malloc(WEB_JSON_ARENA_BYTES, MALLOC_CAP_8BIT);
        jsonArena.capacity = jsonArena.base ? WEB_JSON_ARENA_BYTES : 0;
        replyBuffer = (char *)heap_caps_malloc(WEB_REPLY_BUFFER_BYTES, MALLOC_CAP_8BIT);
    }

    // Last-heard table of every telepathon, newest card per row, polled by data/telepathons.js.
    asyncWebServer.on("/Telepathons", HTTP_GET, [this](AsyncWebServerRequest *request){
        gated(request, [this](AsyncWebServerRequest *r) {
            sendJsonReply(r, [this](JsonDocument &json) { if (telepathonsProvider) telepathonsProvider(json); });
        });
    });

    // All cards kept for one row: /TelepathonHistory?row=3&gen=7 (gen guards against the row
    // having been reused for another device since the page last polled).
    asyncWebServer.on("/TelepathonHistory", HTTP_GET, [this](AsyncWebServerRequest *request){
        int row = request->hasParam("row") ? request->getParam("row")->value().toInt() : -1;
        int gen = request->hasParam("gen") ? request->getParam("gen")->value().toInt() : -1;
        gated(request, [this, row, gen](AsyncWebServerRequest *r) {
            sendJsonReply(r, [this, row, gen](JsonDocument &json) { if (historyProvider) historyProvider(json, row, gen); });
        });
    });

    // Radio and memory history, one row per 10 minutes - fetched by the Diagnostics tab.
    asyncWebServer.on("/RadioHistory", HTTP_GET, [this](AsyncWebServerRequest *request){
        gated(request, [this](AsyncWebServerRequest *r) {
            sendJsonReply(r, [this](JsonDocument &json) { if (radioHistoryProvider) radioHistoryProvider(json); });
        });
    });

  asyncWebServer.begin();

}




void AnnabelleWifiManager::generateWebData(DynamicJsonDocument& json, String sentBy){
    json["currentFunctionValue"]= annabelleData.currentFunctionValue;
    json["groupidentifier"]=annabelleData.groupidentifier;
    json["secondsTime"] = annabelleData.secondsTime;
    json["dataSamplingSec"] = annabelleData.dataSamplingSec;
    json["temperature"] = annabelleData.temperature;
    json["rtcBatVolt"] = annabelleData.rtcBatVolt;
    json["opMode"] = annabelleData.opMode;
    json["rssi"] = annabelleData.rssi;
    json["snr"] = annabelleData.snr;
    json["rtcBatVolt"] = annabelleData.rtcBatVolt;        
    json["operatingStatus"] = annabelleData.operatingStatus;
    json["digitalStablesUpload"] = annabelleData.digitalStablesUpload;
    json["secondsSinceLastPulse"] = annabelleData.secondsSinceLastPulse;
    json["soft_ap_ssid"] = soft_ap_ssid;
    json["serialnumber"] = serialNumber;
   
   

    json["sentBy"] = serialNumber;
    json["apAddress"] = apAddress;
    json["hostname"] = hostname;
    json["stationmode"] = stationmode;
    json["ssid"] = ssid;
    json["ssids"] = ssids;
    json["lora"] = lora;
    json["internetAvailable"] = internetAvailable;
    json["internetPingTime"] = internetPingTime;
    json["ipAddress"] = ipAddress;
    json["totp"] = totpcode;
    json["deviceTypeId"]=annabelleData.deviceTypeId;
    json["dsLastUpload"]=annabelleData.dsLastUpload;
    json["latitude"]=annabelleData.latitude;
     json["longitude"]=annabelleData.longitude;
    // Clock, board temperature, settings outcome - kept by the sketch.
    if (webDataExtras) webDataExtras(json);
  }

int AnnabelleWifiManager::uploadDataToDigitalStables()
{

    DynamicJsonDocument json(1800);
    generateWebData(json,"");
    String output;
    serializeJson(json, output);
    
    const char *serverName = "http://devices.digitalstables.com/DeviceUploadServlet";
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    boolean toReturn = false;
    int httpResponseCode = http.POST(output);
   if(debug) _HardSerial.print("upload digitalstables return ");
    if(debug) _HardSerial.println(httpResponseCode);
    annabelleData.digitalStablesUpload = false;
    if (httpResponseCode == 200)
    {                                       // Check for the returning code
        String response = http.getString(); // Get the response to the request
        if(debug) _HardSerial.print("reponse= ");
        if(debug) _HardSerial.println(response);
        if (response == "Ok")
        {
            toReturn = true;
            annabelleData.digitalStablesUpload = true;
        }
        else
        {
            httpResponseCode = 500;
        }
    }
    http.end();
    if(debug) _HardSerial.print("upload digitalstables return ");
    if(debug) _HardSerial.println(httpResponseCode);

    return httpResponseCode; // toReturn;
}



int AnnabelleWifiManager::uploadDigitalStablesDataToDigitalStables(DigitalStablesData& digitalStablesData){

  DynamicJsonDocument json(1800);
  // JSONObjectTransformer::generateDaffodilWebData(daffodilData,json, serialNumber);
  memcpy(digitalStablesData.sentbyarray, annabelleData.serialnumberarray, sizeof(annabelleData.serialnumberarray));
 
  dataManager.generateDigitalStablesData(digitalStablesData, json);
  String output;
  serializeJson(json, output);
  const char* serverName = "http://devices.digitalstables.com/DeviceUploadServlet";
  http.begin(serverName);    
  http.addHeader("Content-Type", "application/json");
  boolean toReturn=false;
  int httpResponseCode = http.POST(output);
  if(debug) _HardSerial.print("upload  digitalStablesData ");
 if(debug) _HardSerial.print(digitalStablesData.devicename);
  if(debug) _HardSerial.print(" to digitalstables return ");
if(debug) _HardSerial.println(httpResponseCode);
   annabelleData.digitalStablesUpload=false;
  if (httpResponseCode == 200) { //Check for the returning code
      String response = http.getString();  //Get the response to the request
      if(response=="Ok"){
        toReturn =true;
         annabelleData.digitalStablesUpload=true;
      }else{
        httpResponseCode=500;
      }
  }
  http.end();
  return httpResponseCode;//toReturn;
}


AnnabelleWifiManager::~AnnabelleWifiManager() {}

// --- Fixed-memory JSON replies (2026-10-17, see WebJsonArena in the header) ---------------------

// Each block carries its size in front, so reallocate() knows how much to copy.
void *WebJsonArena::allocate(size_t size) {
	size_t need = ((size + 3) & ~(size_t)3) + 4;
	if (!base || used + need > capacity) return nullptr;  // ArduinoJson then marks the document overflowed
	uint8_t *block = base + used;
	*(uint32_t *)block = (uint32_t)size;
	last = used;
	used += need;
	if (used > peak) peak = used;
	return block + 4;
}

void *WebJsonArena::reallocate(void *ptr, size_t newSize) {
	if (!ptr) return allocate(newSize);
	uint8_t *block = (uint8_t *)ptr - 4;
	size_t oldSize = *(uint32_t *)block;
	if (block == base + last) {  // the newest block: grow or shrink it where it is
		size_t need = ((newSize + 3) & ~(size_t)3) + 4;
		if (last + need > capacity) return nullptr;
		*(uint32_t *)block = (uint32_t)newSize;
		used = last + need;
		if (used > peak) peak = used;
		return ptr;
	}
	void *moved = allocate(newSize);
	if (moved) memcpy(moved, ptr, oldSize < newSize ? oldSize : newSize);
	return moved;
}

// Builds a reply in the arena, writes it into the reply buffer and sends it from there. Only one
// reply is in flight at a time: the buffer is busy until the client has the reply and the
// connection closes (onDisconnect), or 10 s at most.
void AnnabelleWifiManager::sendJsonReply(AsyncWebServerRequest *request, std::function<void(JsonDocument&)> fill) {
	if (replyBusy && millis() - replyBusySinceMs > 10000) replyBusy = false;  // a client that never closed
	if (!jsonArena.base || !replyBuffer || replyBusy) {
		replyBusyRejects++;
		request->send(503, "text/plain", "busy");
		return;
	}
	replyBusy = true;
	replyBusySinceMs = millis();
	replyOwner = request;  // webRequestDone() frees the buffer when this connection closes
	jsonArena.reset();
	size_t len = 0;
	bool overflowed = false;
	{
		JsonDocument json(&jsonArena);
		fill(json);
		overflowed = json.overflowed();
		if (!overflowed) len = serializeJson(json, replyBuffer, WEB_REPLY_BUFFER_BYTES);
	}
	if (overflowed || len == 0 || len >= WEB_REPLY_BUFFER_BYTES - 1) {
		replyTooBig++;
		replyBusy = false;
		replyOwner = nullptr;
		request->send(507, "text/plain", "reply too big");
		return;
	}
	if (len > replyPeakBytes) replyPeakBytes = len;
	AsyncWebServerResponse *response = request->beginResponse(200, "application/json", (const uint8_t *)replyBuffer, len);
	if (!response) {
		replyBusy = false;
		replyOwner = nullptr;
		webLowMemory++;
		request->send(503, "text/plain", "busy");
		return;
	}
	request->send(response);
}

// --- Web gate (2026-10-22, see WEB_MAX_ACTIVE in the header) ------------------------------------

bool AnnabelleWifiManager::webMemoryOk() {
	return ESP.getFreeHeap() >= WEB_MIN_FREE_HEAP
		&& heap_caps_get_largest_free_block(MALLOC_CAP_8BIT) >= WEB_MIN_FREE_BLOCK;
}

// Runs handler now if there's room and memory, otherwise answers 503 - the page polls again.
void AnnabelleWifiManager::gated(AsyncWebServerRequest *request, WebHandler handler) {
	if (webActive >= WEB_MAX_ACTIVE) {
		webBusy++;
		request->send(503, "text/plain", "busy");
		return;
	}
	if (!webMemoryOk()) {
		webLowMemory++;
		request->send(503, "text/plain", "busy");
		return;
	}
	webActive++;
	request->onDisconnect([this, request]() { webRequestDone(request); });
	handler(request);
}

// A gated request's connection closed (reply delivered, or the client went away).
void AnnabelleWifiManager::webRequestDone(AsyncWebServerRequest *request) {
	if (webActive) webActive--;
	if (request == replyOwner) {
		replyOwner = nullptr;
		replyBusy = false;
	}
}

// data/index.html.gz - the library finds the .gz and sends it with Content-Encoding: gzip.
void AnnabelleWifiManager::sendPage(AsyncWebServerRequest *request) {
	if (pageEtag.length() && request->hasHeader("If-None-Match")
		&& request->getHeader("If-None-Match")->value() == pageEtag) {
		request->send(304);
		return;
	}
	AsyncWebServerResponse *response = request->beginResponse(_fs, "/index.html", "text/html");
	if (!response) {  // NULL when the file can't be opened - including LittleFS out of memory
		request->send(503);
		return;
	}
	if (pageEtag.length()) response->addHeader("ETag", pageEtag);
	response->addHeader("Cache-Control", "no-cache");  // keep it, but ask (304) before using it
	request->send(response);
}

