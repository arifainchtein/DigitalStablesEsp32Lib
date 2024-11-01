
/*
 * WifiManager.cpp
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#include <WifiManager.h>
#define WATCHDOG_WDI 18

const char *ntpServer = "oceania.pool.ntp.org";
const long gmtOffset_sec = 10 * 3600;
const int daylightOffset_sec = 3600;
bool timeConfigured = false;
bool pingComplete = false;

static const int TASK_STACK_SIZE = 8192;

WifiManager::WifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e) : _HardSerial(serial), timeManager(t), secretManager(e), asyncWebServer(80) {}

bool WifiManager::getInternetAvailable()
{
    return internetAvailable;
}
void WifiManager::setLora(boolean b)
{
    lora = b;
    _HardSerial.print("setting lora to ");
    _HardSerial.println(lora);
}

void WifiManager::setTimezone(String t)
{
    timezone = t;
}

void WifiManager::setCurrentToTpCode(long c)
{
    totpcode = c;
}

void WifiManager::setSerialNumber(String s)
{
    serialNumber = s;
}
void WifiManager::setCurrentTimerRecord(const RTCInfoRecord c)
{
    currentTimerRecord = c;
}

bool WifiManager::getStationMode()
{
    return stationmode;
}

String WifiManager::getMacAddress()
{
    return WiFi.macAddress();
}

void WifiManager::setCurrentSSID(String s)
{
    ssid = s;
}
void WifiManager::setSensorString(String s)
{
    sensorString = s;
}

String WifiManager::getApAddress()
{
    apAddress = WiFi.softAPIP().toString();

    return apAddress;
}

double WifiManager::getRSSI()
{
    return WiFi.RSSI();
}

String WifiManager::getSoft_ap_password()
{
    return soft_ap_password;
}

void WifiManager::setSoft_ap_password(String s)
{
    soft_ap_password = s;
}

String WifiManager::getSoft_ap_ssid()
{
    return soft_ap_ssid;
}

void WifiManager::setSoft_ap_ssid(String s)
{
    soft_ap_ssid = s;
}

String WifiManager::getIpAddress()
{
    return ipAddress;
}

String WifiManager::getHostName()
{
    return hostname;
}

void WifiManager::setHostName(String c)
{
    hostname = c;
}

String WifiManager::getSSID()
{
    return ssid;
}

void WifiManager::setSSID(String c)
{
    ssid = c;
}

void WifiManager::createDeneWord(JsonObject &deneWord, String name, String value, String valueType)
{
    deneWord["Name"] = name;
    deneWord["Value"] = value;
    deneWord["Dene Type"] = valueType;
}

void WifiManager::createDeneWord(JsonObject &deneWord, String name, float value, String valueType)
{
    deneWord["Name"] = name;
    deneWord["Value"] = value;
    deneWord["Value Type"] = valueType;
}

void WifiManager::createDeneWord(JsonObject &deneWord, String name, int value, String valueType)
{
    deneWord["Name"] = name;
    deneWord["Value"] = value;
    deneWord["Value Type"] = valueType;
}

void WifiManager::createDeneWord(JsonObject &deneWord, String name, long value, String valueType)
{
    deneWord["Name"] = name;
    deneWord["Value"] = value;
    deneWord["Value Type"] = valueType;
}

String WifiManager::getTeleonomeData(String url, bool debug)
{

    int endPos = url.indexOf(".local");
    if (endPos > 0)
    {
        // http://Sento.local
        // http://Ra.local
        int startPos = 7;
        String hostname = url.substring(startPos, endPos); // + ".local";
        if (debug)
        {
            Serial.print("hostname: ");
            Serial.print(hostname);
        }
        IPAddress ipAddress;

        int err = WiFi.hostByName(hostname.c_str(), ipAddress);
        String ipA = String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]);
        if (debug)
        {
            Serial.print(" Ip address: ");
            Serial.print(ipA);
            Serial.print(" err: ");
            Serial.print(err);
        }
        if (err == 1)
        {
            String h = (String)hostname;
            h.concat(".local");
            url.replace(h, ipA);
            if (debug)
            {
                Serial.print(" new url: ");
                Serial.println(url);
            }
        }
    }

    String toReturn = "Error";

    // _HardSerial.println(WiFi.localIP());

    // _HardSerial.print("Fetching " + url );

    // if(WiFi.status()== WL_CONNECTED){
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == 200)
    { // Check for the returning code
        toReturn = http.getString();
    }
    else
    {
        toReturn = "Error:" + httpCode;
    }
    http.end();
    // }

    if (debug)
    {
        _HardSerial.print("teleonome data httpCode: ");
        _HardSerial.println(httpCode);
    }

    return toReturn;
}

String WifiManager::scanNetworks()
{

    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);

    this->_HardSerial.println("startscan poiomnt 1");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    this->_HardSerial.println("startscan poiomnt 2");
    delay(5);
    digitalWrite(WATCHDOG_WDI, HIGH);
    // delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    int n = WiFi.scanNetworks(true);

    uint8_t counter = 0;
    bool keepGoing = true;

    while (keepGoing)
    {
        digitalWrite(WATCHDOG_WDI, HIGH);
        // delay(2);
        digitalWrite(WATCHDOG_WDI, LOW);
        int16_t WiFiScanStatus = WiFi.scanComplete();
        if (WiFiScanStatus < 0)
        {
            digitalWrite(WATCHDOG_WDI, HIGH);
            // delay(2);
            digitalWrite(WATCHDOG_WDI, LOW);
            delay(500);
            counter++;
            if (counter > 60)
            {
                keepGoing = false;
                this->_HardSerial.println("failed to find");
            }
        }
        else
        {
            keepGoing = false;
            this->_HardSerial.println("FOUND NETWORKAS");
        }
    }

    // delay(5);
    //  digitalWrite(WATCHDOG_WDI, HIGH);
    // delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    this->_HardSerial.println("startscan poiomnt 3");
    String json = "[";
    n = WiFi.scanComplete();
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    this->_HardSerial.println("scan 4 complete");
    if (n)
    {
        for (int i = 0; i < n; ++i)
        {
            if (i)
                json += ",";
            json += "{";
            json += "\"rssi\":" + String(WiFi.RSSI(i));
            json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
            json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
            json += ",\"channel\":" + String(WiFi.channel(i));
            json += ",\"secure\":" + String(WiFi.encryptionType(i));
            // json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
            this->_HardSerial.print(",");
            this->_HardSerial.print(WiFi.SSID(i));
            json += "}";
        }
    }
    json += "]";
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    WiFi.scanDelete();
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    return json;
}

// uint8_t WifiManager::getWifiStatus(){
//     return WiFi.status();
// }

bool WifiManager::getAPStatus()
{
    return apConnected;
}

bool WifiManager::configWifiAP(String sas, String sap, String h)
{
    ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();
    ;
    soft_ap_ssid = sas;
    soft_ap_password = sap;
    hostname = h;
    stationmode = false;
    secretManager.saveWifiParameters(ssid, password, soft_ap_ssid, soft_ap_password, hostname, stationmode);
    // secretManager.saveConfigData(float fieldId, stationName );
    WiFi.disconnect();
    return connectAP();
}

bool WifiManager::configWifiSTA(String s, String p, String h)
{
    ssid = s;
    password = p;
    hostname = h;
    soft_ap_ssid = secretManager.getSoftAPSSID();
    if (soft_ap_ssid == "")
    {
        soft_ap_ssid = "192.168.4.1";
    }
    else
    {
    }
    soft_ap_password = secretManager.getSoftAPPASS();
    // hostname=secretManager.getHostName();
    stationmode = true;
    secretManager.saveWifiParameters(ssid, password, soft_ap_ssid, soft_ap_password, hostname, stationmode);
    WiFi.disconnect();
    return connectSTA();
}
bool WifiManager::getWifiStatus()
{

    if (getAPStatus())
    {
        return true;
    }
    else
    {
        return (WiFi.status() == WL_CONNECTED);
    }
}

void WifiManager::stop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        _HardSerial.println("WifiManager stop, Voltage low, turning off Wi-Fi");
        WiFi.mode(WIFI_OFF);
        apConnected = false;
        WiFi.disconnect();
    }
}

void WifiManager::restartWifi()
{
    WiFi.disconnect();
    if (stationmode)
    {
        bool gotConnection = connectSTA();
        _HardSerial.println("restartwifi got connection station mode");
        if (!gotConnection)
        {
            _HardSerial.println("restartwifi could not connect ");
            connectAP();
        }
    }
    else
    {
        _HardSerial.println("restartwifi c connectap ");
        connectAP();
    }
}

bool WifiManager::connectSTA()
{
    ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();
    ;
    ssid = secretManager.getSSID();
    hostname = secretManager.getHostName();
    _HardSerial.print("Setting hostname=");
    _HardSerial.println(hostname);
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    //
    // the host name needs to be set before setting
    // the mode
    //
    const char *hnchar = hostname.c_str();
    int hnl = hostname.length();
    int hnlt = hnl + 1;
    char hname[hnlt];
    // hname[hnlt] = '\0';

    snprintf(hname, hnlt, hnchar, 32);
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hname);
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);

    _HardSerial.print("in connectSTA ssid=");
    _HardSerial.println(ssid);

    _HardSerial.println(WiFi.localIP());
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    WiFi.begin(const_cast<char *>(ssid.c_str()), const_cast<char *>(password.c_str()));
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    bool gotConnection = true;
    uint8_t counter = 0;
    bool keepGoing = true;

    while (keepGoing)
    {
        keepGoing = WiFi.status() != WL_CONNECTED;
        //   delay(1000);
        _HardSerial.print(".");
        WiFi.disconnect();
        WiFi.begin(const_cast<char *>(ssid.c_str()), const_cast<char *>(password.c_str()));
        digitalWrite(WATCHDOG_WDI, HIGH);
        delay(2);
        digitalWrite(WATCHDOG_WDI, LOW);
        delay(500);
        digitalWrite(WATCHDOG_WDI, HIGH);
        delay(2);
        digitalWrite(WATCHDOG_WDI, LOW);
        counter++;
        if (counter > 10)
        {
            keepGoing = false;
            gotConnection = false;
        }
    }
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    _HardSerial.print("in connectSTA after settmg wifi, ip=");
    _HardSerial.println(WiFi.localIP());

    if (gotConnection)
    {

        ipAddress = WiFi.localIP().toString();
        _HardSerial.print(" Connected with ip=");
        _HardSerial.println(ipAddress);

        digitalWrite(WATCHDOG_WDI, HIGH);
        delay(2);
        digitalWrite(WATCHDOG_WDI, LOW);

        xTaskCreate(
            [](void *parameter)
            {
                WifiManager *instance = (WifiManager *)parameter;
                instance->setupTime();
                vTaskDelete(NULL);
            },
            "TimeConfig",
            TASK_STACK_SIZE,
            NULL,
            1,
            NULL);

        keepGoing = true;
        timeConfigured = false;
        while (keepGoing)
        {

            digitalWrite(WATCHDOG_WDI, HIGH);
            delay(2);
            digitalWrite(WATCHDOG_WDI, LOW);
            delay(500);
            if (timeConfigured)
                keepGoing = false;
            digitalWrite(WATCHDOG_WDI, HIGH);
            delay(2);
            digitalWrite(WATCHDOG_WDI, LOW);
            counter++;
            if (counter > 10)
            {
                keepGoing = false;
            }
        }
        digitalWrite(WATCHDOG_WDI, HIGH);
        delay(2);
        digitalWrite(WATCHDOG_WDI, LOW);

        // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

        _HardSerial.print(" after config time");
        _HardSerial.println(ipAddress);
        digitalWrite(WATCHDOG_WDI, HIGH);
        delay(2);
        digitalWrite(WATCHDOG_WDI, LOW);
        internetConnectionAvailable();
        digitalWrite(WATCHDOG_WDI, HIGH);
        delay(2);
        digitalWrite(WATCHDOG_WDI, LOW);
        _HardSerial.print(" after internetConnectionAvailable");
    }
    else
    {
        _HardSerial.print("Error connecting to wifi router ssid=");
        _HardSerial.println(ssid);
    }
    if (!MDNS.begin(hostname.c_str()))
    {
        _HardSerial.println("Error setting up MDNS responder!");
    }
    else
    {
        _HardSerial.println("MDNS setup ok");
    }

    return gotConnection;
}

bool WifiManager::connectAP()
{
    // WiFi.mode(WIFI_MODE_AP);
    WiFi.mode(WIFI_MODE_AP);

    soft_ap_ssid = secretManager.getSoftAPSSID();
    if (soft_ap_ssid == "" || soft_ap_ssid == "192.168.4.1")
    {
        soft_ap_ssid = "Daffodil";
    }
    soft_ap_password = secretManager.getSoftAPPASS();
    hostname = secretManager.getHostName();
    ;

    _HardSerial.print("Starting connectAP 2 soft_ap_ssid=");
    _HardSerial.println(soft_ap_ssid);

    //  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    //   WiFi.onEvent(OnWiFiEvent);
    apConnected = false;
    // WiFi.setHostname(hostname.c_str());
    // soft_ap_ssid="PanchoPool";
    WiFi.softAP(soft_ap_ssid.c_str(), soft_ap_password.c_str());
    delay(500);
    apAddress = WiFi.softAPIP().toString();
    _HardSerial.print("Ap Mode using Ip Address of ");

    _HardSerial.println(apAddress);
    if (apAddress == "192.168.4.1")
        apConnected = true;

    return apConnected;
}

void WifiManager::internetConnectionAvailable()
{
    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    // Start ping task
    //   xTaskCreate(
    //     [](void* parameter) {
    //         WifiManager* instance = (WifiManager*)parameter;
    //       instance->doPing();
    //       vTaskDelete(NULL);
    //     },
    //     "PingTask",
    //     TASK_STACK_SIZE,
    //     NULL,
    //     1,
    //     NULL
    //   );

    // bool keepGoing=true;
    // int counter=0;
    //     while (keepGoing){

    //        digitalWrite(WATCHDOG_WDI, HIGH);
    //     delay(2);
    //     digitalWrite(WATCHDOG_WDI, LOW);
    //       delay(500);
    //        if (pingComplete) keepGoing=false;
    //        digitalWrite(WATCHDOG_WDI, HIGH);
    //     delay(2);
    //     digitalWrite(WATCHDOG_WDI, LOW);
    //         counter++;
    //         if(counter>10){
    //             keepGoing=false;

    //         }
    //     }
    //  digitalWrite(WATCHDOG_WDI, HIGH);
    //     delay(2);
    //     digitalWrite(WATCHDOG_WDI, LOW);

    // internetAvailable =  Ping.ping("www.google.com",2,300);
    IPAddress target(8, 8, 8, 8); // Google's DNS server
    struct ping_option ping;
    ping.count = 2;
    ping.ip = target;
    ping.coarse_time = 300; // timeout

    //  ping.recv_function = &pingResults;

    bool internetAvailable = ping_start(&ping);

    // internetAvailable = ping_start(target, 2, 300, 0, 32);
    // internetAvailable= ping_start(&ping);

    digitalWrite(WATCHDOG_WDI, HIGH);
    delay(2);
    digitalWrite(WATCHDOG_WDI, LOW);
    internetPingTime = 0;
    if (internetAvailable)
    {
        internetPingTime = 300;
    }
    _HardSerial.print("called ping, speed=");
    _HardSerial.println(internetPingTime);
}

//  static void WifiManager::pingResults(void* opt, void* resp) {
//      WifiManager* instance = (WifiManager*)parameter;
//         if (instance != nullptr) {
//             instance->handlePingResponse(opt, resp);
//         }
//     }

void WifiManager::setupTime()
{
    //   configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    timeConfigured = true;
}

// void WifiManager::doPing() {
//      digitalWrite(WATCHDOG_WDI, HIGH);
//     delay(2);
//     digitalWrite(WATCHDOG_WDI, LOW);

//   internetAvailable = Ping.ping("google.com", 3, 300);
//    digitalWrite(WATCHDOG_WDI, HIGH);
//     delay(2);
//     digitalWrite(WATCHDOG_WDI, LOW);
//    internetPingTime=0;
//   if(internetAvailable) {
//     internetPingTime = Ping.averageTime();
//   }
//   pingComplete = true;
// }

bool WifiManager::setTimeFromInternet()
{
    bool toReturn = false;
    configTime(0, 0, "pool.ntp.org");
    // String timezone="AEST-10AEDT,M10.1.0,M4.1.0/3";
    setenv("TZ", timezone.c_str(), 1); //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
    tzset();
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        uint8_t date = timeinfo.tm_mday;
        uint8_t month = timeinfo.tm_mon + 1;
        uint8_t year = timeinfo.tm_year - 100;
        uint8_t hour = timeinfo.tm_hour;
        uint8_t minute = timeinfo.tm_min;
        uint8_t second = timeinfo.tm_sec;
        uint8_t dw = timeinfo.tm_wday;
        String commandTime = "SetTime#";
        commandTime.concat(date);
        commandTime.concat("#");
        commandTime.concat(month);
        commandTime.concat("#");
        commandTime.concat(year);
        commandTime.concat("#");
        commandTime.concat(dw);
        commandTime.concat("#");
        commandTime.concat(hour);
        commandTime.concat("#");
        commandTime.concat(minute);
        commandTime.concat("#");
        commandTime.concat(second);
        commandTime.concat("#");

        toReturn = timeManager.setTime(commandTime);
        _HardSerial.print("Setting internetTime:");
        _HardSerial.print(commandTime);
        _HardSerial.print(" returned ");
        _HardSerial.println(toReturn);
        currentTimerRecord = timeManager.now();
    }
    return toReturn;
}

WifiManager::~WifiManager() {}
