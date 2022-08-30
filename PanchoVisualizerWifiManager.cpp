#include <PanchoVisualizerWifiManager.h>
#include <ArduinoJson.h>

 
PanchoVisualizerWifiManager::PanchoVisualizerWifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e,  PanchoTankFlowData& tf,PanchoConfigData& p) :
WifiManager(serial ,  t, e,  tf,  p) {}

 

void PanchoVisualizerWifiManager::start(){
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
       asyncWebServer.begin();
    }else{
         //  mark as not connected
    }
    

asyncWebServer.on("/GetWebData", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);
	json["flow1name"] = this->panchoTankFlowData.flow1name;
	json["flow2name"] = this->panchoTankFlowData.flow2name;
	json["tank1name"] = this->panchoTankFlowData.tank1name;
	json["tank2name"] = this->panchoTankFlowData.tank2name;
    json["secondsTime"] = this->panchoTankFlowData.secondsTime;
    json["dataSamplingSec"] = this->panchoTankFlowData.dataSamplingSec;
    json["currentFunctionValue"] = this->panchoTankFlowData.currentFunctionValue;
    json["temperature"] = this->panchoTankFlowData.temperature;
    json["reg33Voltage"] = this->panchoTankFlowData.reg33Voltage;
    json["rtcBatVolt"] = this->panchoTankFlowData.rtcBatVolt;
    json["opMode"] = this->panchoTankFlowData.opMode;
    json["rssi"] = this->panchoTankFlowData.rssi;
    json["snr"] = this->panchoTankFlowData.snr;
    json["flowRate"] = this->panchoTankFlowData.flowRate;
    json["totalMilliLitres"] = this->panchoTankFlowData.totalMilliLitres;
    json["flowRate2"] = this->panchoTankFlowData.flowRate2;
    json["totalMilliLitres2"] = this->panchoTankFlowData.totalMilliLitres2;
    json["tankPressurePsi"] = this->panchoTankFlowData.tankPressurePsi;
    json["tankPressureVolts"] = this->panchoTankFlowData.tankPressureVolts;
    json["tankWaterLevel"] = this->panchoTankFlowData.tankWaterLevel;
    json["tankHeightMeters"] = this->panchoTankFlowData.tankHeightMeters;
    json["tank2PressurePsi"] = this->panchoTankFlowData.tank2PressurePsi;
    json["tank2PressureVolts"] = this->panchoTankFlowData.tank2PressureVolts;
    json["tank2WaterLevel"] = this->panchoTankFlowData.tank2WaterLevel;
    json["tank2HeightMeters"] = this->panchoTankFlowData.tank2HeightMeters;
    json["qfactor1"] = this->panchoTankFlowData.qfactor1;
    json["qfactor2"] = this->panchoTankFlowData.qfactor2;
    json["operatingStatus"] = this->panchoTankFlowData.operatingStatus;
    json["sleepPingMinutes"] = this->panchoTankFlowData.sleepPingMinutes;
    json["secondsSinceLastPulse"] = this->panchoTankFlowData.secondsSinceLastPulse;
    json["soft_ap_ssid"] = this->soft_ap_ssid;
    json["serialNumber"] = this->serialNumber;
    json["apAddress"] = this->apAddress;
    json["hostname"] = this->hostname;
    json["stationmode"] = this->stationmode;
    json["ssid"] =this-> ssid;
    json["ipAddress"] = this->ipAddress;
    serializeJson(json, *response);
    request->send(response);
  });

 asyncWebServer.on("/GetSensorData", HTTP_GET, [this](AsyncWebServerRequest *request){
       this->_HardSerial.println("curl request returning");
        this->_HardSerial.println(sensorString);
        request->send_P(200, "text/plain", sensorString.c_str()); 
    });


    
/*
    asyncWebServer.on("/Command", HTTP_GET, [this](AsyncWebServerRequest *request){
			int numberOfParameters = request->params();

			if(numberOfParameters==0){
				String toReturn="Invalid Request";
				request->send_P(200, "text/plain", toReturn.c_str());
			}

			int switchState=0;
			AsyncWebParameter* p = request->getParam(0);
			String command = p->value();
			this->HardSerial.print("command:");
			this->HardSerial.println(command);
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

				this->HardSerial.println("curl request GetTime returning");
				this->HardSerial.println(toReturn);
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

						this->HardSerial.println("curl request SetTime returning");
						this->HardSerial.println(toReturn);
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
				this->HardSerial.print("GetCommandCode produces:");
				this->HardSerial.println(code);
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

					this->HardSerial.println(secret);
					this->HardSerial.println("Ok-GetSecret");
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
				this->HardSerial.print("switchState=");
				this->HardSerial.println(switchState);
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
						this->HardSerial.println(secret);
						p = request->getParam(2);
						int numberDigits = p->value().toInt();
						p = request->getParam(3);
						int periodSeconds= p->value().toInt();


						this->HardSerial.print("receive savesecret");
						this->HardSerial.println(secret);
						secretManager.saveSecret(secret, numberDigits, periodSeconds);
						this->HardSerial.println("stored secret, reading secret");
						String secretRead = secretManager.readSecret();
						this->HardSerial.println(secretRead);
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
*/
    asyncWebServer.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request){

         });

}

PanchoVisualizerWifiManager::~PanchoVisualizerWifiManager() {}