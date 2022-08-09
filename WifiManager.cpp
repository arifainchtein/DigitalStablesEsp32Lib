/*
 * WifiManager.cpp
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#include <WifiManager.h>

#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
String ssid = "MainRouter24";
String password = "";
String soft_ap_ssid = "PanchoVisualizer_AP";
String soft_ap_password = "";
AsyncWebServer asyncWebServer(80);
//WiFiServer server(8080);
HTTPClient http;
String apAddress;
String ipAddress;
String sensorString;
uint8_t delayT=10;
float fieldId;
String hostname = "PanchoTankFlow";

//
// stationmode:
// 0 - AP and STA
// 1 Serial.print()
bool stationmode=false;

WifiManager::WifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e,  PanchoTankFlowData& tf,PanchoConfigData& p) :
 _HardSerial(serial),timeManager(t),secretManager(e), tankFlowData(tf) ,panchoConfigData(p)  {}


void WifiManager::setCurrentTimerRecord(const RTCInfoRecord c){
    currentTimerRecord=c;   
}
    
bool WifiManager::getStationMode( )
{
    return stationmode;
}

String WifiManager::getMacAddress( )
{
    return WiFi.macAddress();
}

void WifiManager::setCurrentSSID(String s)
{
    ssid = s;
}
void WifiManager::setSensorString(String s)
{
    sensorString=s;
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
     soft_ap_password=s;
}

String WifiManager::getSoft_ap_ssid()
{
    return soft_ap_ssid;
}

void WifiManager::setSoft_ap_ssid(String s)
{
     soft_ap_ssid=s;
}

String WifiManager::getIpAddress()
{
    return ipAddress;
}

String WifiManager::getHostName()
{
    return hostname;
}

void  WifiManager::setHostName(String c)
{
     hostname=c;
}

String WifiManager::getSSID()
{
    return ssid;
}

void  WifiManager::setSSID(String c)
{
     ssid=c;
}

String WifiManager::getTeleonomeData(String url, bool debug){
    
    
    int endPos = url.indexOf(".local");
    if(endPos>0){
        // http://Sento.local
        // http://Ra.local
        int startPos = 7;
        String hostname = url.substring(startPos, endPos);// + ".local";
         if(debug){
            Serial.print("hostname: ");
            Serial.print(hostname);
        }
        IPAddress ipAddress;
            
        int err = WiFi.hostByName(hostname.c_str(), ipAddress) ;
        String ipA = String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3])  ; 
        if(debug){
            Serial.print(" Ip address: ");
            Serial.print(ipA);
            Serial.print(" err: ");
            Serial.print(err);
        }
        if(err == 1){
            String h = (String)hostname;
            h.concat(".local");
                url.replace(h,ipA);
                if(debug){
                    Serial.print(" new url: ");
                    Serial.println(url);
                }
                
        } 
    }
    
    String toReturn="Error";
   
   // _HardSerial.println(WiFi.localIP());

 // _HardSerial.print("Fetching " + url );

   // if(WiFi.status()== WL_CONNECTED){
        http.begin(url);    
        int httpCode = http.GET();
        if (httpCode == 200) { //Check for the returning code
            toReturn = http.getString();
        }else{
           toReturn="Error:" + httpCode; 
        }
        http.end();
   // }


    if(debug){
        _HardSerial.print("teleonome data httpCode: ");
        _HardSerial.println(httpCode);
    }
 
    return toReturn;
}

void WifiManager::scanNetworks() {
  _HardSerial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  _HardSerial.println("scan done");
  if (n == 0) {
      _HardSerial.println("no networks found");
  } else {
    _HardSerial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      _HardSerial.print(i + 1);
      _HardSerial.print(": ");
      _HardSerial.print(WiFi.SSID(i));
      _HardSerial.print(" (");
      _HardSerial.print(WiFi.RSSI(i));
      _HardSerial.print(")");
      _HardSerial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);
}

uint8_t WifiManager::getWifiStatus(){
    return WiFi.status();
}

bool WifiManager::getAPStatus(){
    return apConnected;
}
/*
void WifiManager::configWifi(String s, String p, String sas, String sap, String h ){
    ssid = s;
    password = p;
    soft_ap_ssid = sas;
    soft_ap_password = sap;
    hostname=h;
    secretManager.saveWifiParameters( ssid,  password, soft_ap_ssid, soft_ap_password,hostname);
   // secretManager.saveConfigData(float fieldId, stationName );
    WiFi.disconnect();
    connect();
}
*/
bool WifiManager::configWifiAP( String sas, String sap, String h ){
    ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();;
    soft_ap_ssid = sas;
    soft_ap_password = sap;
    hostname=h;
    stationmode=false;
    secretManager.saveWifiParameters( ssid,  password, soft_ap_ssid, soft_ap_password,hostname, stationmode);
   // secretManager.saveConfigData(float fieldId, stationName );
    WiFi.disconnect();
    return connectAP();
}

bool WifiManager::configWifiSTA(String s, String p, String h ){
    ssid = s;
    password = p;
    hostname=h;
    soft_ap_ssid = secretManager.getSoftAPSSID();
    if(soft_ap_ssid=="")soft_ap_ssid="192.168.4.1";
    soft_ap_password = secretManager.getSoftAPPASS();
    //hostname=secretManager.getHostName();
    stationmode=true;
    secretManager.saveWifiParameters( ssid,  password, soft_ap_ssid, soft_ap_password,hostname, stationmode);
    WiFi.disconnect();
    return connectSTA();
}

void WifiManager::restartWifi(){
    WiFi.disconnect();
     if(stationmode){
       connectSTA();
    }else{
         connectAP();
    }
}


bool WifiManager::connectSTA(){
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
     WiFi.setHostname(hostname.c_str()); //define hostname
  
   WiFi.begin(const_cast<char*>(ssid.c_str()), const_cast<char*>(password.c_str()));
    bool gotConnection=true;
    uint8_t counter=0;
    bool keepGoing=true;
    while (keepGoing){
        keepGoing=WiFi.status() != WL_CONNECTED;
        delay(1000);
        _HardSerial.print(".");
        counter++;
        if(counter>30){
            keepGoing=false;
            gotConnection=false;
        }
    }

_HardSerial.print("in connectSTA after settmg wifi, ip=");
  _HardSerial.println(WiFi.localIP());

    if(gotConnection){
       
        ipAddress = WiFi.localIP().toString();
        _HardSerial.print(" Connected with ip=");
        _HardSerial.println(ipAddress); 
    }else{
        _HardSerial.print("Error connecting to wifi router ssid=");
         _HardSerial.println(ssid);
    }
    
    return gotConnection;

    
    
}

bool WifiManager::connectAP(){
    WiFi.mode(WIFI_MODE_AP);
    
    //  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
 //   WiFi.onEvent(OnWiFiEvent);
    apConnected=false;
    WiFi.setHostname(hostname.c_str());
    WiFi.softAP(soft_ap_ssid.c_str(), soft_ap_password.c_str());
    apAddress = WiFi.softAPIP().toString();
    if(apAddress=="192.168.4.1")apConnected=true;
    /*
    bool gotConnection=true;
    uint8_t counter=0;
    bool keepGoing=true;
    while (keepGoing){
        keepGoing=WiFi.status() != WL_CONNECTED;
        delay(500);
        _HardSerial.print(".");
        counter++;
        if(counter>10){
            keepGoing=false;
            gotConnection=false;
        }
    }


    if(gotConnection){
       
        apAddress = WiFi.softAPIP().toString();
        _HardSerial.print(" after got connectin apConnected=");
        _HardSerial.println(this->apConnected);
        if(apAddress=="192.168.4.1")apConnected=true;
        _HardSerial.println("Access Point Enabled connected.");
        _HardSerial.println("Access Point IP address: ");
        _HardSerial.println(apAddress); 
    }else{
        _HardSerial.print("Error connecting to wifi router ssid=");
         _HardSerial.println(ssid);
    }
    */
    return apConnected;
}



void WifiManager::start(){
   //
   // get the parameters
   //
   ssid = secretManager.getSSID();
    password = secretManager.getWifiPassword();;
    soft_ap_ssid = secretManager.getSoftAPSSID();
    soft_ap_password = secretManager.getSoftAPPASS();
    hostname=secretManager.getHostName();
    stationmode = secretManager.getStationMode();
    _HardSerial.println("ssid=");
    _HardSerial.println(ssid);
    _HardSerial.println("stationmode=");
    _HardSerial.println(stationmode);
    
    if(stationmode){
       connectSTA();
    }else{
         connectAP();
    }
    asyncWebServer.begin();
    asyncWebServer.on("/GetSensorData", HTTP_GET, [this](AsyncWebServerRequest *request){
       this->_HardSerial.println("curl request returning");
        this->_HardSerial.println(sensorString);
        request->send_P(200, "text/plain", sensorString.c_str()); 
    });

    asyncWebServer.on("/Command", HTTP_GET, [this](AsyncWebServerRequest *request){
			int numberOfParameters = request->params();

			if(numberOfParameters==0){
				String toReturn="Invalid Request";
				request->send_P(200, "text/plain", toReturn.c_str());
			}

			int switchState=0;
			AsyncWebParameter* p = request->getParam(0);
			String command = p->value();
			this->_HardSerial.print("command:");
			this->_HardSerial.println(command);
			RTCInfoRecord newTime;
			if(command=="GetTime"){

				String toReturn="";
				toReturn+=this->currentTimerRecord.date;
				toReturn+="/";
				toReturn+=this->currentTimerRecord.month;
				toReturn+="/";
				toReturn+=this->currentTimerRecord.year;
				toReturn+=" ";
				toReturn+=this->currentTimerRecord.hour;
				toReturn+=":";
				if(this->currentTimerRecord.minute<10){
					toReturn+="0";
				}
				toReturn+=this->currentTimerRecord.minute;
				toReturn+=":";
				if(this->currentTimerRecord.second<10){
					toReturn+="0";
				}
				toReturn+=this->currentTimerRecord.second;
				toReturn+="\r\nOk-GetTime";

				this->_HardSerial.println("curl request GetTime returning");
				this->_HardSerial.println(toReturn);
				request->send_P(200, "text/plain", toReturn.c_str());
			}else if(command=="SetTime"){
				//switchState = digitalRead( OP_MODE );
				if (tankFlowData.opMode == LOW ){
					//  AsyncWebParameter* p = request->getParam(1);
					//  long userCode = p->value().toInt();
					//   bool validCode = secretManager.checkCode( userCode);
					//   _HardSerial.print("Set Time, aut code:");
					//   _HardSerial.print(userCode);
					//  _HardSerial.print("  valid=");
					//  _HardSerial.println(validCode);
					// if(validCode){

					if(numberOfParameters!=8){
						String toReturn="Invalid Request";
						request->send_P(200, "text/plain", toReturn.c_str());
					} else{
						AsyncWebParameter* p = request->getParam(1);
						newTime.date = p->value().toInt();
						p = request->getParam(2);
						newTime.month = p->value().toInt();
						p = request->getParam(3);
						newTime.year = p->value().toInt();
						p = request->getParam(4);
						newTime.dayOfWeek = p->value().toInt();
						p = request->getParam(5);
						newTime.hour = p->value().toInt();
						p = request->getParam(6);
						newTime.minute = p->value().toInt();
						p = request->getParam(7);
						newTime.second = p->value().toInt();

						timeManager.printTimeToSerial(newTime);
						timeManager.setTime(newTime);
						//SetTime#17#5#20#7#11#06#00
						RTCInfoRecord r =  timeManager.now();
						String toReturn="";
						toReturn+=r.date;
						toReturn+="/";
						toReturn+=r.month;
						toReturn+="/";
						toReturn+=r.year;
						toReturn+=" ";
						toReturn+=r.hour;
						toReturn+=":";
						if(r.minute<10){
							toReturn+="0";
						}
						toReturn+=r.minute;
						toReturn+=":";
						if(r.second<10){
							toReturn+="0";
						}
						toReturn+=r.second;
						toReturn+="\r\nOk-SetTime";

						this->_HardSerial.println("curl request SetTime returning");
						this->_HardSerial.println(toReturn);
						request->send_P(200, "text/plain", toReturn.c_str());
					}
					//  }else{
					//    String toReturn="Failure-SetTime:Authentication Failed";
					//   request->send_P(200, "text/plain", toReturn.c_str());
					//  }

				}else{
					String toReturn="Failure-SetTime:Command Not Allowed";
					request->send_P(200, "text/plain", toReturn.c_str());
				}
			}else if(command.startsWith("GetCommandCode")){
				long code =secretManager.generateCode();
				this->_HardSerial.print("GetCommandCode produces:");
				this->_HardSerial.println(code);
				//
				// patch a bug in the totp library
				// if the first digit is a zero, it
				// returns a 5 digit number
				String toReturn="";

				if(code<100000){
					toReturn+="0";
				}
				toReturn+=code;
				request->send_P(200, "text/plain", toReturn.c_str());
			}else if(command.startsWith("VerifyUserCode")){
				if(numberOfParameters!=2){
					String toReturn="Invalid Request";
					request->send_P(200, "text/plain", toReturn.c_str());
				}else{
					AsyncWebParameter* p = request->getParam(1);
					String codeInString = p->value();
					long userCode = codeInString.toInt();
					boolean validCode = secretManager.checkCode( userCode);
					String toReturn="Failure-Invalid Code";
					if(validCode)toReturn="Ok-Valid Code";
					request->send_P(200, "text/plain", toReturn.c_str());
					delay(delayT);
				}
			}else if(command.startsWith("SetRosieConfigParams")){
				if(numberOfParameters!=5){
					String toReturn="Invalid Request";
					request->send_P(200, "text/plain", toReturn.c_str());
				}else{
					AsyncWebParameter* p = request->getParam(1);
					String codeInString = p->value();
					long userCode = codeInString.toInt();
					bool validCode = secretManager.checkCode( userCode);
					String toReturn="";
					if(validCode){
						p = request->getParam(2);
						float fieldId = (float)(p->value().toInt());

						p = request->getParam(3);
						String stationNameS = p->value();

						p = request->getParam(4);
						float sleepPingMinutes = (float)(p->value().toInt());
						secretManager.saveSleepPingMinutes(sleepPingMinutes);

						// Set values to send
						//stationNameS.toCharArray(tankFlowData.stationName, 20);
						//panchoConfigData.fieldId=fieldId;
						//tankFlowData.fieldId = fieldId;
						//secretManager.saveConfigData(fieldId,  tankFlowData.stationName );
						toReturn="Ok-SetRosieConfigParams";
					}else{
						toReturn="Failure-SetRosieConfigParams-Invalid Code";
					}
					request->send_P(200, "text/plain", toReturn.c_str());
					delay(delayT);
				}
			}else if(command.startsWith("GetSecret")){
				//switchState = digitalRead(OP_MODE);
				//if (switchState == LOW ){
                if (tankFlowData.opMode == LOW ){
					//
					// the switch is in the PGM position so set the secret
					//
					// char secretCode[SHARED_SECRET_LENGTH];
					String secret = secretManager.readSecret();

					this->_HardSerial.println(secret);
					this->_HardSerial.println("Ok-GetSecret");
					Serial.flush();
					delay(delayT);
					String toReturn=secret;
					toReturn+="\r\nOk-GetSecret";
					request->send_P(200, "text/plain", toReturn.c_str());
				}else{
					String toReturn="Failure-GetSecret:Command Not Allowed";
					request->send_P(200, "text/plain", toReturn.c_str());
				}

			} else if(command.startsWith("SetSecret")){
			//	switchState = digitalRead(OP_MODE);
				this->_HardSerial.print("switchState=");
				this->_HardSerial.println(switchState);
				if (tankFlowData.opMode == LOW ){
					//   //
					//   // the switch is in the PGM position so return the secret
					//   //
					//    ///c=SetSecret&a=IZQWS3TDNB2GK2LO&b=6&c=30
					if(numberOfParameters!=4){
						String toReturn="Invalid Request";
						request->send_P(200, "text/plain", toReturn.c_str());
					}else{
						AsyncWebParameter* p = request->getParam(1);
						String secret = p->value();
						this->_HardSerial.println(secret);
						p = request->getParam(2);
						int numberDigits = p->value().toInt();
						p = request->getParam(3);
						int periodSeconds= p->value().toInt();


						this->_HardSerial.print("receive savesecret");
						this->_HardSerial.println(secret);
						secretManager.saveSecret(secret, numberDigits, periodSeconds);
						this->_HardSerial.println("stored secret, reading secret");
						String secretRead = secretManager.readSecret();
						this->_HardSerial.println(secretRead);
						String toReturn="Ok-SetSecret";
						request->send_P(200, "text/plain", toReturn.c_str());
					}
				}else{
					String toReturn="Failure-Set Secret:Command Not Allowed";
					request->send_P(200, "text/plain", toReturn.c_str());
				}
			} else if(command.startsWith("UpdateWifiParameters")){
				if(numberOfParameters!=4){
					String toReturn="Invalid Request";
					request->send_P(200, "text/plain", toReturn.c_str());
				}else{
					AsyncWebParameter* p = request->getParam(1);
					String codeInString = p->value();
					long userCode = codeInString.toInt();
					bool validCode = secretManager.checkCode( userCode);
					String toReturn="";
					if(validCode){
						p = request->getParam(2);
						ssid = p->value();
						p = request->getParam(3);
						password = p->value();
						//secretManager.saveWifiParameters(ssid, password);
                         secretManager.saveWifiParameters( ssid,  password, soft_ap_ssid, soft_ap_password,hostname, stationmode);
						ipAddress = WiFi.localIP().toString();
						WiFi.disconnect();
						WiFi.begin(const_cast<char*>(ssid.c_str()), const_cast<char*>(password.c_str()));
						toReturn="Ok-UpdateWifiParameters";
					}else{
						toReturn="Failure-SetRosieConfigParams-Invalid Code";
					}
					request->send_P(200, "text/plain", toReturn.c_str());
					delay(delayT);
				}
			}else{
				String toReturn="Failure-Invalid Command:" + command;
				request->send_P(200, "text/plain", toReturn.c_str());
	} });

    asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
        String indexPage = getIndexPage();

        request->send_P(200, "text/html", indexPage.c_str());

        if (ON_STA_FILTER(request))
        {
            this->_HardSerial.println("asking for index.html from station");
            request->send(200, "text/html", indexPage.c_str());
            return;
        }
        else if (ON_AP_FILTER(request))
        {
            this->_HardSerial.println("asking for index.html from access point");
            request->send(200, "text/html", indexPage.c_str());
            return;
        } });

    asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
                      {
        String indexPage = getIndexPage();
        if (ON_STA_FILTER(request)){
            request->send(200, "text/html", indexPage.c_str());
            return;
        }else if (ON_AP_FILTER(request)){
            request->send(200, "text/html", indexPage.c_str());
            return;
        } });

  //  server.begin();

}

String WifiManager::getIndexPage(){
        String toReturn = "<html>";
        toReturn += "<head>";
        toReturn += "<meta http-equiv=\"refresh\" content=\"3\">";
        toReturn += "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3\" crossorigin=\"anonymous\">";
        toReturn += "<title>";
        toReturn += hostname;
        toReturn += "</title>";
        toReturn += "</head>";
        toReturn += "<body><div class=\"container\">";
        toReturn += "<div class=\"row\">";
        toReturn += "<div class=\"col-9\"><h4 style=\"margin-left:50px;margin-right:100px;\">";
        toReturn += hostname;
        toReturn += "</h4></div><div class=\"col-3\">";

        toReturn += currentTimerRecord.hour;
        toReturn += ":";
        if (currentTimerRecord.minute < 10)
        {
            toReturn += "0";
        }
        toReturn += currentTimerRecord.minute;
        toReturn += ":";
        if (currentTimerRecord.second < 10)
        {
            toReturn += "0";
        }
        toReturn += currentTimerRecord.second;
        toReturn += "</div></div>";
        //
        // Sensors
        //
        toReturn += "<br><br>";
        toReturn += "<div class=\"row\"><div class=\"col-12\"><h5 style=\"margin-left:0px;margin-right:100px;\">Sensors</h5></div></div>";
        toReturn += "<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";
        toReturn += "<tr><td>Flow Rate (l/min)</td><td>";
        toReturn += tankFlowData.flowRate;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Total Volume Flow (liters)</td><td>";
        toReturn += tankFlowData.totalMilliLitres / 1000;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Tank Pressure (psi)</td><td>";
        toReturn += tankFlowData.tankPressurePsi;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Tank Pressure (volts)</td><td>";
        toReturn += tankFlowData.tankPressureVolts;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Tank Water Height (m)</td><td>";
        toReturn += tankFlowData.tankWaterLevel;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Tank Height (m)</td><td>";
        toReturn += tankFlowData.tankHeightMeters;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Temperature</td><td>";
        toReturn += tankFlowData.temperature;
        toReturn += "</td></tr>";

        toReturn += "</table>";
        toReturn += "<br><br>";
        toReturn += "<div class=\"row\"><div class=\"col-12\"><h5 style=\"margin-left:0px;margin-right:100px;\">Power</h5></div></div>";
        toReturn += "<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";

        toReturn += "<tr><td>RTC Battery Voltage</td><td>";
        toReturn += tankFlowData.rtcBatVolt;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>3.3V Regulator Voltage</td><td>";
        toReturn += tankFlowData.reg33Voltage;
        toReturn += "</td></tr>";
        toReturn += "</td></tr>";
        toReturn += "</table>";
        toReturn += "<br><br>";
        toReturn += "<div class=\"row\"><div class=\"col-12\"><h5 style=\"margin-left:0px;margin-right:100px;\">Operational Data</h5></div></div>";
        toReturn += "<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";
        
        toReturn += "<tr><td>Serial Number</td><td>";
        toReturn += getMacAddress();
        toReturn += "</td></tr>";

        long now = timeManager.getCurrentTimeInSeconds(currentTimerRecord);

        toReturn += "<tr><td>Seconds since Last Pulse</td><td>";
        long l1 = now - tankFlowData.secondsTime;
        toReturn += l1;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Sampling Frequency(sec)</td><td>";
        toReturn += tankFlowData.dataSamplingSec;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Operational Mode Switch  </td><td>";
        if (tankFlowData.opMode == LOW)
            toReturn += "PROGRAMMING";
        else
            toReturn += "RUN";
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Operating Status</td><td>";
        if (tankFlowData.operatingStatus == 2)
        {
            toReturn += "Normal";
        }
        else if (tankFlowData.operatingStatus == 1)
        {
            toReturn += "WPS";
        }
        else if (tankFlowData.operatingStatus == 0)
        {
            toReturn += "Sleep";
        }
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Sleep Ping Minutes  </td><td>";
        toReturn += tankFlowData.sleepPingMinutes;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Last Message Received At</td><td>";
        toReturn += lastReceptionRTCInfoRecord.date;
        toReturn += "/";
        toReturn += lastReceptionRTCInfoRecord.month;
        toReturn += "/";
        toReturn += lastReceptionRTCInfoRecord.year;
        toReturn += " ";
        toReturn += lastReceptionRTCInfoRecord.hour;
        toReturn += ":";
        if (lastReceptionRTCInfoRecord.minute < 10)
        {
            toReturn += "0";
        }
        toReturn += lastReceptionRTCInfoRecord.minute;
        toReturn += ":";
        if (lastReceptionRTCInfoRecord.second < 10)
        {
            toReturn += "0";
        }
        toReturn += lastReceptionRTCInfoRecord.second;
        toReturn += "</td></tr>";

        toReturn += "</table>";

        toReturn += "<br><br>";

        toReturn += "<div class=\"row\">";
        toReturn += "<div class=\"col-9\"><h4 style=\"margin-left:0px;margin-right:100px;\">Pancho Radio Info</h4></div><div class=\"col-3\"></div></div>";

        toReturn += "<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";

        toReturn += "<tr><td>Wifi Strength</td><td>";
        toReturn += WiFi.RSSI();
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Mac Address</td><td>";
        toReturn += getMacAddress();
        toReturn += "</td></tr>";


        toReturn += "<tr><td>Ip Address</td><td>";
        toReturn += ipAddress;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>SSID</td><td>";
        toReturn += ssid;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Access Point Name</td><td>";
        toReturn += soft_ap_ssid;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Access Point IP Address</td><td>";
        toReturn += apAddress;
        toReturn += "</td></tr>";

        


        toReturn += "<tr><td>Lora RSSI</td><td>";
        toReturn += tankFlowData.rssi;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Lora SNR</td><td>";
        toReturn += tankFlowData.snr;
        toReturn += "</td></tr>";

        toReturn += "</table>";
        toReturn += "</div></body>";
        toReturn += "</html>";
        return toReturn;
    }

/*
void WifiManager::checkClient(){
    WiFiClient client = server.available(); // listen for incoming clients
    if (client)
    {                                  // if you get a client,
        Serial.println("New Client."); // print a message out the serial port
        String currentLine = "";       // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.write(c);        // print it out the serial monitor
                if (c == '\n')
                { // if the byte is a newline character

                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
                        client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }

                // Check to see if the client request was "GET /H" or "GET /L":
                if (currentLine.endsWith("GET /GetSensorData"))
                {
                    
                    client.println(sensorString);
                }
                if (currentLine.endsWith("GET /L"))
                {
                    digitalWrite(5, LOW); // GET /L turns the LED off
                }
            }
        }

        // close the connection:
        client.stop();
        Serial.println("Client Disconnected.");
    }
}
*/
WifiManager::~WifiManager() {}
