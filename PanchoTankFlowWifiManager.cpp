#include <PanchoTankFlowWifiManager.h>


 String okString="Ok";

PanchoTankFlowWifiManager::PanchoTankFlowWifiManager(HardwareSerial &serial, PCF8563TimeManager &t, Esp32SecretManager &e,  PanchoTankFlowData& tf,PanchoConfigData& p) :
WifiManager(serial ,  t, e,  tf,  p) {}

 

void PanchoTankFlowWifiManager::start(){
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
/*
	"Value": "Soon",
	"Name": "Page Title",
	"Value Type": "String"
}
*/

asyncWebServer.on("/GetDene", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");


	const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(1) + 20;
    DynamicJsonDocument doc(8192);//capacity);
    JsonObject dene = doc.to<JsonObject>();
    JsonArray denewords = dene.createNestedArray("DeneWords");

 	dene["Name"] = this->hostname;
	dene["Dene Type"] = "PanchoTankFlow";

	JsonObject deneWordFlow1Name = denewords.createNestedObject();
	createDeneWord(deneWordFlow1Name,"Flow1Name", this->panchoTankFlowData.flow1name,  "String");
	
	
	JsonObject deneWordFlow2Name = denewords.createNestedObject();
	createDeneWord(deneWordFlow2Name,"Flow2Name", this->panchoTankFlowData.flow2name,  "String");
	
	JsonObject deneWordTank1Name = denewords.createNestedObject();
	createDeneWord(deneWordTank1Name,"Tank1Name", this->panchoTankFlowData.tank1name,  "String");
	
	JsonObject deneWordTank2Name = denewords.createNestedObject();
	createDeneWord(deneWordTank2Name,"Tank2Name", this->panchoTankFlowData.tank2name,  "String");
	
	JsonObject deneWordSecondsTime = denewords.createNestedObject();
	createDeneWord(deneWordSecondsTime,"secondsTime", this->panchoTankFlowData.secondsTime,  "long");
	
	JsonObject deneWordDataSamplingSec = denewords.createNestedObject();
	createDeneWord(deneWordDataSamplingSec,"dataSamplingSec", this->panchoTankFlowData.dataSamplingSec,  "int");

	JsonObject deneWordCurrentFunctionValue = denewords.createNestedObject();
	createDeneWord(deneWordCurrentFunctionValue,"currentFunctionValue", this->panchoTankFlowData.currentFunctionValue,  "String");
	
	JsonObject deneWordTemperature = denewords.createNestedObject();
	createDeneWord(deneWordTemperature,"temperature", this->panchoTankFlowData.temperature,  "String");
	
	JsonObject deneWordReg33Voltage = denewords.createNestedObject();
	createDeneWord(deneWordReg33Voltage,"reg33Voltage", this->panchoTankFlowData.reg33Voltage,  "double");
	
	JsonObject deneWordRtcBatVolt = denewords.createNestedObject();
	createDeneWord(deneWordRtcBatVolt,"rtcBatVolt", this->panchoTankFlowData.rtcBatVolt,  "double");

	JsonObject deneWordOpMode = denewords.createNestedObject();
	createDeneWord(deneWordOpMode,"opMode", this->panchoTankFlowData.opMode,  "int");

	JsonObject deneWordRSSI = denewords.createNestedObject();
	createDeneWord(deneWordRSSI,"rssi", this->panchoTankFlowData.rssi,  "double");   
   
   	JsonObject deneWordsnr = denewords.createNestedObject();
	createDeneWord(deneWordsnr,"snr", this->panchoTankFlowData.snr,  "double");   
   
   JsonObject deneWordflowrate = denewords.createNestedObject();
	createDeneWord(deneWordflowrate,"flowRate", this->panchoTankFlowData.flowRate,  "double"); 

	JsonObject deneWordtotalMilliLitres = denewords.createNestedObject();
	createDeneWord(deneWordtotalMilliLitres,"totalMilliLitres", this->panchoTankFlowData.totalMilliLitres,  "double"); 

	JsonObject deneWordflowrate2 = denewords.createNestedObject();
	createDeneWord(deneWordflowrate2,"flowRate2", this->panchoTankFlowData.flowRate2,  "double"); 

	JsonObject deneWordtotalMilliLitres2 = denewords.createNestedObject();
	createDeneWord(deneWordtotalMilliLitres2,"totalMilliLitres2", this->panchoTankFlowData.totalMilliLitres2,  "double"); 

	JsonObject deneWordtankPressurePsi = denewords.createNestedObject();
	createDeneWord(deneWordtankPressurePsi,"tankPressurePsi", this->panchoTankFlowData.tankPressurePsi,  "double"); 

	JsonObject deneWordtankPressureVolts = denewords.createNestedObject();
	createDeneWord(deneWordtankPressureVolts,"tankPressureVolts", this->panchoTankFlowData.tankPressureVolts,  "double"); 

	JsonObject deneWordtankWaterLevel = denewords.createNestedObject();
	createDeneWord(deneWordtankWaterLevel,"tankWaterLevel", this->panchoTankFlowData.tankWaterLevel,  "double"); 

	JsonObject deneWordtankHeightMeters = denewords.createNestedObject();
	createDeneWord(deneWordtankHeightMeters,"tankHeightMeters", this->panchoTankFlowData.tankHeightMeters,  "double"); 

	JsonObject deneWordtank2PressurePsi = denewords.createNestedObject();
	createDeneWord(deneWordtank2PressurePsi,"tank2PressurePsi", this->panchoTankFlowData.tank2PressurePsi,  "double"); 

	JsonObject deneWordtank2PressureVolts = denewords.createNestedObject();
	createDeneWord(deneWordtank2PressureVolts,"tank2PressureVolts", this->panchoTankFlowData.tank2PressureVolts,  "double"); 

	JsonObject deneWordtank2WaterLevel = denewords.createNestedObject();
	createDeneWord(deneWordtank2WaterLevel,"tank2WaterLevel", this->panchoTankFlowData.tank2WaterLevel,  "double"); 

	JsonObject deneWordtank2HeightMeters = denewords.createNestedObject();
	createDeneWord(deneWordtank2HeightMeters,"tank2HeightMeters", this->panchoTankFlowData.tank2HeightMeters,  "double"); 

	JsonObject deneWordqfactor1 = denewords.createNestedObject();
	createDeneWord(deneWordqfactor1,"qfactor1", this->panchoTankFlowData.qfactor1,  "double"); 

	JsonObject deneWordqfactor2 = denewords.createNestedObject();
	createDeneWord(deneWordqfactor2,"qfactor2", this->panchoTankFlowData.qfactor2,  "double"); 
	
	JsonObject deneWordoperatingStatus = denewords.createNestedObject();
	createDeneWord(deneWordoperatingStatus,"operatingStatus", this->panchoTankFlowData.operatingStatus,  "double"); 

	JsonObject deneWordsecondsSinceLastPulse = denewords.createNestedObject();
	createDeneWord(deneWordsecondsSinceLastPulse,"secondsSinceLastPulse", this->panchoTankFlowData.secondsSinceLastPulse,  "long"); 

	JsonObject deneWordsoft_ap_ssid = denewords.createNestedObject();
	createDeneWord(deneWordsoft_ap_ssid,"soft_ap_ssid", this-> soft_ap_ssid,  "String");

	JsonObject deneWordserialNumber = denewords.createNestedObject();
	createDeneWord(deneWordserialNumber,"serialNumber", this->serialNumber,  "String"); 

	JsonObject deneWordapAddress = denewords.createNestedObject();
	createDeneWord(deneWordapAddress,"apAddress", this->apAddress,  "String"); 
	
	JsonObject deneWordstationmode = denewords.createNestedObject();
	createDeneWord(deneWordstationmode,"stationmode", this->stationmode,  "String"); 

	JsonObject deneWordssid = denewords.createNestedObject();
	createDeneWord(deneWordssid,"ssid", this->ssid,  "String"); 

	JsonObject deneWordipAddress = denewords.createNestedObject();
	createDeneWord(deneWordipAddress,"ipAddress", this->ipAddress,  "String"); 

    serializeJson(dene, *response);
    request->send(response);
  });


asyncWebServer.on("/GetWebData", HTTP_GET, [this](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(2056);


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


    asyncWebServer.on("/SetQFactor1", HTTP_GET, [this](AsyncWebServerRequest *request){
        int numberOfParameters = request->params();
        AsyncWebParameter* p = request->getParam(0);
        panchoTankFlowData.qfactor1 = p->value().toFloat();
       
        request->send_P(200, "text/plain", okString.c_str()); 
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

PanchoTankFlowWifiManager::~PanchoTankFlowWifiManager() {}