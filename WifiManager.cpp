/*
 * WifiManager.cpp
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#include <WifiManager.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

char *ssid = "MainRouter24";
char *password = "";
char *soft_ap_ssid = "Rosie1";
char *soft_ap_password = "";
AsyncWebServer asyncWebServer(80);
WiFiServer server(8080);

String apAddress;
String ipAddress;
String sensorString;
uint8_t delayT=10;
float fieldId;

WifiManager::WifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e) : _HardSerial(serial),timeManager(t),secretManager(e)  {}

void WifiManager::setCurrentSSID(char* s)
{
    ssid = s;
}
void WifiManager::setSensorString(String s)
{
    sensorString=s;
}


String WifiManager::getApAddress()
{
    return apAddress;
}
void WifiManager::setCurrentStatusData(RTCInfoRecord &c, TankFlowData* t,PanchoConfigData* p,RTCInfoRecord &l)
{
    currentTimerRecord = c;
    lastReceptionRTCInfoRecord=l;
    tankFlowData=t;
    panchoConfigData=p;
}

double WifiManager::getRSSI()
{
    return WiFi.RSSI();
}

String WifiManager::getSoft_ap_ssid()
{
    return soft_ap_ssid;
}

String WifiManager::getIpAddress()
{
    return ipAddress;
}

void WifiManager::start(){
    WiFi.mode(WIFI_MODE_APSTA);
    //  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    String hostname = "Rosie";
    WiFi.setHostname(hostname.c_str());
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        _HardSerial.print(".");
    }
    _HardSerial.println("");
    ipAddress = WiFi.localIP().toString();
    _HardSerial.println("WiFi connected.");
    _HardSerial.println("IP address: ");
    _HardSerial.println(ipAddress);

    delay(2000);
    apAddress = WiFi.softAPIP().toString();
    _HardSerial.println("Access Point Enabled connected.");
    _HardSerial.println("Access Point IP address: ");
    _HardSerial.println(apAddress);

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
				if (tankFlowData->opMode == LOW ){
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
						stationNameS.toCharArray(tankFlowData->stationName, 20);
						panchoConfigData->fieldId=fieldId;
						tankFlowData->fieldId = fieldId;
						secretManager.saveConfigData(fieldId,  tankFlowData->stationName );
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
                if (tankFlowData->opMode == LOW ){
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
				if (tankFlowData->opMode == LOW ){
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
						String ssids = p->value();

						ssids.toCharArray(ssid, 20);
						p = request->getParam(3);
						String pass = p->value();
						pass.toCharArray(password, 20);
						secretManager.saveWifiParameters(ssid, password);
						ipAddress = WiFi.localIP().toString();
						WiFi.disconnect();
						WiFi.begin(ssid, password);
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

    server.begin();

}

String WifiManager::getIndexPage(){
        String toReturn = "<html>";
        toReturn += "<head>";
        toReturn += "<meta http-equiv=\"refresh\" content=\"3\">";
        toReturn += "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3\" crossorigin=\"anonymous\">";
        toReturn += "<title>";
        toReturn += tankFlowData->stationName;
        toReturn += "</title>";
        toReturn += "</head>";
        toReturn += "<body><div class=\"container\">";
        toReturn += "<div class=\"row\">";
        toReturn += "<div class=\"col-9\"><h4 style=\"margin-left:50px;margin-right:100px;\">";
        toReturn += tankFlowData->stationName;
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
        toReturn += tankFlowData->flowRate;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Total Volume Flow (liters)</td><td>";
        toReturn += tankFlowData->totalMilliLitres / 1000;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Tank Pressure (psi)</td><td>";
        toReturn += tankFlowData->tankPressurePsi;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Tank Pressure (volts)</td><td>";
        toReturn += tankFlowData->tankPressureVolts;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Tank Water Height (m)</td><td>";
        toReturn += tankFlowData->tankWaterLevel;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Tank Height (m)</td><td>";
        toReturn += tankFlowData->tankHeightMeters;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Temperature</td><td>";
        toReturn += tankFlowData->temperature;
        toReturn += "</td></tr>";

        toReturn += "</table>";
        toReturn += "<br><br>";
        toReturn += "<div class=\"row\"><div class=\"col-12\"><h5 style=\"margin-left:0px;margin-right:100px;\">Power</h5></div></div>";
        toReturn += "<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";

        toReturn += "<tr><td>Low Voltage Alert</td>";
        if (tankFlowData->lowVoltageAlert)
            toReturn += "<td style=\"color:red\">ON";
        else
            toReturn += "<td>OFF";
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Solar Voltage</td><td>";
        toReturn += tankFlowData->solarVoltage;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Capacitor Voltage</td><td>";
        toReturn += tankFlowData->capacitorVoltage;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>RTC Battery Voltage</td><td>";
        toReturn += tankFlowData->rtcBatVolt;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>3.3V Regulator Voltage</td><td>";
        toReturn += tankFlowData->reg33Voltage;
        toReturn += "</td></tr>";
        toReturn += "</td></tr>";
        toReturn += "</table>";
        toReturn += "<br><br>";
        toReturn += "<div class=\"row\"><div class=\"col-12\"><h5 style=\"margin-left:0px;margin-right:100px;\">Operational Data</h5></div></div>";
        toReturn += "<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";
        String rosieId = "";
        for (int i = 0; i < 8; i++)
        {
            rosieId += (tankFlowData->rosieId[i], HEX);
        }
        toReturn += "<tr><td>Id</td><td>" + rosieId + "</td></tr>";
        toReturn += "<tr><td>Field Id</td><td>";
        toReturn += (uint8_t)tankFlowData->fieldId;
        toReturn += "</td></tr>";

        long now = timeManager.getCurrentTimeInSeconds(currentTimerRecord);

        toReturn += "<tr><td>Seconds since Last Pulse</td><td>";
        long l1 = now - tankFlowData->secondsTime;
        toReturn += l1;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Sampling Frequency(sec)</td><td>";
        toReturn += tankFlowData->dataSamplingSec;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Operational Mode Switch  </td><td>";
        if (tankFlowData->opMode == LOW)
            toReturn += "PROGRAMMING";
        else
            toReturn += "RUN";
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Operating Status</td><td>";
        if (tankFlowData->operatingStatus == 2)
        {
            toReturn += "Normal";
        }
        else if (tankFlowData->operatingStatus == 1)
        {
            toReturn += "WPS";
        }
        else if (tankFlowData->operatingStatus == 0)
        {
            toReturn += "Sleep";
        }
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Sleep Ping Minutes  </td><td>";
        toReturn += tankFlowData->sleepPingMinutes;
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
        toReturn += "<div class=\"col-9\"><h4 style=\"margin-left:0px;margin-right:100px;\">Rosie Radio Info</h4></div><div class=\"col-3\"></div></div>";

        toReturn += "<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";

        toReturn += "<tr><td>Wifi Strength</td><td>";
        toReturn += WiFi.RSSI();
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
        toReturn += tankFlowData->rssi;
        toReturn += "</td></tr>";

        toReturn += "<tr><td>Lora SNR</td><td>";
        toReturn += tankFlowData->snr;
        toReturn += "</td></tr>";

        toReturn += "</table>";
        toReturn += "</div></body>";
        toReturn += "</html>";
        return toReturn;
    }


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
WifiManager::~WifiManager() {}
