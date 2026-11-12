


#ifndef LIBRARIES_DIGITALSTABLES_ANNABELLEWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_ANNABELLEWIFIMANAGER_H_
#include "Arduino.h"
#include <functional>
#include <WifiManager.h>
#include <AnnabelleData.h>
#include <DataManager.h>
#include <ArduinoJson.h>

// Fixed memory for the page's JSON replies (2026-10-17). Building each reply in a growing buffer
// with a JsonDocument from the heap fragmented Annabelle's memory: her largest free block fell from
// ~51 KB at boot to ~38 KB within the hour (the step tracker put it all "elsewhere", i.e. the web
// server), replies came back with bytes missing, and a burst of page requests crashed her (out of
// memory in async_tcp, 2026-10-16 20:01). Now one arena (for the JsonDocument) and one reply buffer
// are allocated once in start() and reused: one reply at a time; a request arriving while one is
// still being sent gets 503 and the page simply polls again.
#define WEB_JSON_ARENA_BYTES 16384
#define WEB_REPLY_BUFFER_BYTES 8192

// Web request gate (2026-10-22). Every response going out holds ~10 KB until its connection closes
// (a 2.9 KB send buffer, up to 5.7 KB of TCP buffers, the request, an open file) and Annabelle has
// ~34 KB to spare. A browser loading the page opened 6 connections at once and ran her out of
// memory within a second: panics in async_tcp 2026-10-20 06:39, 2026-10-21 07:47, 2026-10-22 07:33
// and in loopTask 2026-10-21 13:41 (LittleFS open failing), each with ~34 KB free the second before.
// Now the page is one gzipped file (build_web.py), at most WEB_MAX_ACTIVE replies go out at once and
// anything more gets 503 (the page's polls ask again). A queue that held requests back and answered
// them later crashed her too and stalled replies (2026-10-22 09:32 and 14:35) - don't bring it back.
// Neither refusing new connections inside the web library before it allocates them (abort() +
// delete: events reached the freed client; close(): lwIP "tcp_receive: wrong state" assert,
// 2026-10-22 16:21-16:43). A flood of 10 page loads can still run her out of memory; a browser
// loading the one-file page can't.
#define WEB_MAX_ACTIVE 2
#define WEB_MIN_FREE_HEAP 16384
#define WEB_MIN_FREE_BLOCK 6144
class WebJsonArena : public ArduinoJson::Allocator {
public:
	uint8_t *base = nullptr;
	size_t capacity = 0, used = 0, last = 0;  // last = offset of the newest block, so it can grow in place
	size_t peak = 0;                           // most arena used by one reply
	void reset() { used = 0; last = 0; }
	void *allocate(size_t size) override;
	void deallocate(void *) override {}        // everything is dropped at once by reset()
	void *reallocate(void *ptr, size_t newSize) override;
};

class AnnabelleWifiManager : public WifiManager{



	FS _webFs;  // the page's file system - see setWebFs()
public:
    DataManager& dataManager;
	AnnabelleData& annabelleData;
	boolean debug=false;
	AnnabelleWifiManager(HardwareSerial& serial ,FS &fs,DataManager& d,PCF8563TimeManager& t, Esp32SecretManager& e, AnnabelleData& tf) ;
	// Where the page (index.html.gz) is served from (2026-11-11): the "www" partition once its image
	// has been flashed, else the data LittleFS it was uploaded to before. Call before start().
	void setWebFs(FS &fs) { _webFs = fs; }
	void start( );
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();	
	int uploadDigitalStablesDataToDigitalStables(DigitalStablesData& r);
	// Fills the /Telepathons JSON (last-heard table for data/index.html). Set by the sketch,
	// which owns the table, before start().
	void setTelepathonsProvider(std::function<void(JsonDocument&)> provider) { telepathonsProvider = provider; }
	// Fills /TelepathonHistory?row=&gen= - every card kept for one row of that table.
	void setHistoryProvider(std::function<void(JsonDocument&, int, int)> provider) { historyProvider = provider; }
	// /RadioHistory: Annabelle.ino fillRadioHistoryJson (10-minute radio/memory snapshots, 2026-10-16)
	void setRadioHistoryProvider(std::function<void(JsonDocument&)> provider) { radioHistoryProvider = provider; }
	// Adds the sketch's own fields (clock, board temperature, last settings outcome) to GetWebData.
	void setWebDataExtras(std::function<void(DynamicJsonDocument&)> extras) { webDataExtras = extras; }
	// Handles the settings forms other than WiFi (formName, first value, second value) and returns
	// the reply text, or "" for a form it doesn't know.
	void setSettingsAction(std::function<String(const String&, const String&, const String&)> action) { settingsAction = action; }
	// Reply-buffer statistics for the Diagnostics tab: largest reply sent, most arena used, and
	// requests turned away because a reply was still going out (or didn't fit).
	size_t getReplyPeakBytes() const { return replyPeakBytes; }
	size_t getArenaPeakBytes() const { return jsonArena.peak; }
	uint32_t getReplyBusyRejects() const { return replyBusyRejects; }
	uint32_t getReplyTooBig() const { return replyTooBig; }
	// Web gate statistics: requests answered 503 because 2 replies were already going out, or
	// because memory was low.
	uint32_t getWebBusy() const { return webBusy; }
	uint32_t getWebLowMemory() const { return webLowMemory; }

    virtual ~AnnabelleWifiManager();

private:
	String okString="Ok";
	std::function<void(JsonDocument&)> telepathonsProvider;
	std::function<void(JsonDocument&, int, int)> historyProvider;
	std::function<void(JsonDocument&)> radioHistoryProvider;
	std::function<void(DynamicJsonDocument&)> webDataExtras;
	std::function<String(const String&, const String&, const String&)> settingsAction;
	WebJsonArena jsonArena;
	char *replyBuffer = nullptr;
	volatile bool replyBusy = false;
	volatile uint32_t replyBusySinceMs = 0;
	size_t replyPeakBytes = 0;
	uint32_t replyBusyRejects = 0, replyTooBig = 0;
	AsyncWebServerRequest *replyOwner = nullptr;  // the request the reply buffer is going out on
	void sendJsonReply(AsyncWebServerRequest *request, std::function<void(JsonDocument&)> fill);

	// Web gate - handlers and onDisconnect all run in the async_tcp task.
	typedef std::function<void(AsyncWebServerRequest *)> WebHandler;
	uint8_t webActive = 0;
	uint32_t webBusy = 0, webLowMemory = 0;
	String pageEtag;
	static bool webMemoryOk();
	void gated(AsyncWebServerRequest *request, WebHandler handler);
	void webRequestDone(AsyncWebServerRequest *request);
	void sendPage(AsyncWebServerRequest *request);

};
#endif /* LIBRARIES_DIGITALSTABLES_ROSIEWIFIMANAGER_H_ */