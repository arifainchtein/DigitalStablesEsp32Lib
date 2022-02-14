#include <Esp32WifiManager.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

 char* ssid     = "MainRouter24";
 char* password = "";
 char *soft_ap_ssid = "Rosie1";
 char *soft_ap_password = "";


    AsyncWebServer asyncWebServer(80);
    WiFiServer server(8080);

Esp32WifiManager::Esp32WifiManager( Hardware_HardSerial& _HardSerial): _Hard_HardSerial(_HardSerial){}

String Esp32WifiManager::getSSID(){
    return ssid;
}
String Esp32WifiManager::getIPAddress(){
    return ipAddress;
}

String Esp32WifiManager::getAPAddress(){
    return apAddress;
}

String Esp32WifiManager::getRSSI(){
    return WiFi.RSSI();
}
String Esp32WifiManager::getSoftAPSSID(){
    return soft_ap_ssid;
}


void Esp32WifiManager::start( ){
    WiFi.mode(WIFI_MODE_APSTA);
    String hostname = "Rosie";
    WiFi.setHostname(hostname.c_str());
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
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
    asyncWebServer.on("/GetSensorData", HTTP_GET, [](AsyncWebServerRequest *request){
      String sensorString =getSensorData();
      _HardSerial.println("curl request returning");
      _HardSerial.println(sensorString);
      
        request->send_P(200, "text/plain", sensorString.c_str());
    });

  asyncWebServer.on("/Command", HTTP_GET, [](AsyncWebServerRequest *request){
      int numberOfParameters = request->params();
      
      if(numberOfParameters==0){
        String toReturn="Invalid Request";
        request->send_P(200, "text/plain", toReturn.c_str());
      }
      
     int switchState=0;
      AsyncWebParameter* p = request->getParam(0);
      String command = p->value();
      _HardSerial.print("command:");
      _HardSerial.println(command);
        RTCInfoRecord newTime;  
        if(command=="GetTime"){
           
           String toReturn="";
           toReturn+=currentTimerRecord.date;
           toReturn+="/";
           toReturn+=currentTimerRecord.month;
           toReturn+="/";
           toReturn+=currentTimerRecord.year;
           toReturn+=" ";
           toReturn+=currentTimerRecord.hour;
           toReturn+=":";
           if(currentTimerRecord.minute<10){
            toReturn+="0";
           }
           toReturn+=currentTimerRecord.minute;
           toReturn+=":";
           if(currentTimerRecord.second<10){
            toReturn+="0";
           }
          toReturn+=currentTimerRecord.second;
          toReturn+="\r\nOk-GetTime";
    
          _HardSerial.println("curl request GetTime returning");
          _HardSerial.println(toReturn);
          request->send_P(200, "text/plain", toReturn.c_str());
        }else if(command=="SetTime"){
            switchState = digitalRead(OP_MODE);
            if (switchState == LOW ){
             
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
                   
                   timeManager.printTimeTo_HardSerial(newTime);
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
            
                  _HardSerial.println("curl request SetTime returning");
                  _HardSerial.println(toReturn);
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
           _HardSerial.print("GetCommandCode produces:");
          _HardSerial.println(code);
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
            delay(delayTime);
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
              String stationName = p->value();

              p = request->getParam(4);
              float sleepPingMinutes = (float)(p->value().toInt());
              secretManager.saveSleepPingMinutes(sleepPingMinutes);

              // Set values to send
              stationName.toCharArray(rosieConfigData.stationName, 20); 
              rosieConfigData.fieldId=fieldId;
              rosieData.fieldId = fieldId;
              stationName.toCharArray(rosieData.stationName, 20); 
              secretManager.saveConfigData(fieldId,  stationName );
              
             

               toReturn="Ok-SetRosieConfigParams";
            }else{
             toReturn="Failure-SetRosieConfigParams-Invalid Code";
            }
            request->send_P(200, "text/plain", toReturn.c_str());
            delay(delayTime);
          }
        }else if(command.startsWith("GetSecret")){
            switchState = digitalRead(OP_MODE);
            if (switchState == LOW ){
              //
              // the switch is in the PGM position so set the secret
              //
              // char secretCode[SHARED_SECRET_LENGTH];
              String secret = secretManager.readSecret();
              
              _HardSerial.println(secret);
              _HardSerial.println("Ok-GetSecret");
              _HardSerial.flush();
              delay(delayTime);
              String toReturn=secret;
              toReturn+="\r\nOk-GetSecret";
              request->send_P(200, "text/plain", toReturn.c_str());
            }else{
              String toReturn="Failure-GetSecret:Command Not Allowed";
              request->send_P(200, "text/plain", toReturn.c_str());
            }
          
        } else if(command.startsWith("SetSecret")){    
            switchState = digitalRead(OP_MODE);
            _HardSerial.print("switchState=");
              _HardSerial.println(switchState);            
             if (switchState == LOW ){
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
                    _HardSerial.println(secret);
                   p = request->getParam(2);
                    int numberDigits = p->value().toInt();
                   p = request->getParam(3);
                   int periodSeconds= p->value().toInt();
              
                  
                  _HardSerial.print("receive savesecret");
                  _HardSerial.println(secret);
                  secretManager.saveSecret(secret, numberDigits, periodSeconds);
                  _HardSerial.println("stored secret, reading secret");
                  String secretRead = secretManager.readSecret();
                  _HardSerial.println(secretRead);
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
            delay(delayTime);
          }
        }else{
            String toReturn="Failure-Invalid Command:" + command;
            request->send_P(200, "text/plain", toReturn.c_str());
        }
    });



    asyncWebServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
     String indexPage =getIndexPage();
    
        request->send_P(200, "text/html", indexPage.c_str());

        if (ON_STA_FILTER(request)) {
           _HardSerial.println("asking for index.html from station");
          request->send(200, "text/html", indexPage.c_str());
          return;
 
        } else if (ON_AP_FILTER(request)) {
            _HardSerial.println("asking for index.html from access point");
            request->send(200, "text/html", indexPage.c_str());
            return;
        }
     
    });
    asyncWebServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
     String indexPage =getIndexPage();
      if (ON_STA_FILTER(request)) {
           
          request->send(200, "text/html", indexPage.c_str());
          return;
 
        } else if (ON_AP_FILTER(request)) {
            
            request->send(200, "text/html", indexPage.c_str());
            return;
        }
     
    });

    
    server.begin();
}

void Esp32WifiManager::checkForClient(){
     WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    _HardSerial.println("New Client.");           // print a message out the _HardSerial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        _HardSerial.write(c);                    // print it out the _HardSerial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
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
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /GetSensorData")) {
           String sensorString =getSensorData();
           client.println(sensorString);
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);                // GET /L turns the LED off
        }
      }
    }
   
    // close the connection:
    client.stop();
    _HardSerial.println("Client Disconnected.");
  }
}

String Esp32WifiManager::getIndexPage(){
  String toReturn="<html>";
   toReturn+="<head>";
   toReturn+="<meta http-equiv=\"refresh\" content=\"3\">";
   toReturn+="<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3\" crossorigin=\"anonymous\">";
  toReturn+="<title>";
  toReturn+=rosieConfigData.stationName;
  toReturn+="</title>";
   toReturn+="</head>";
  toReturn+="<body><div class=\"container\">";
   toReturn+="<div class=\"row\">";
    toReturn+="<div class=\"col-9\"><h4 style=\"margin-left:50px;margin-right:100px;\">";
     toReturn+=rosieConfigData.stationName;
    toReturn+="</h4></div><div class=\"col-3\">";
   
  toReturn+=currentTimerRecord.hour;
  toReturn+=":";
   if(currentTimerRecord.minute<10){
    toReturn+="0";
   }
   toReturn+=currentTimerRecord.minute;
   toReturn+=":";
   if(currentTimerRecord.second<10){
    toReturn+="0";
   }
   toReturn+=currentTimerRecord.second;
    toReturn+="</div></div>";  
    //
    // Sensors
    //
     toReturn+="<br><br>";
  toReturn+="<div class=\"row\"><div class=\"col-12\"><h5 style=\"margin-left:0px;margin-right:100px;\">Sensors</h5></div></div>";
  toReturn+="<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";
 toReturn+="<tr><td>Flow Rate (l/min)</td><td>";
  toReturn+=rosieData.flowRate;
  toReturn+="</td></tr>";
  
  toReturn+="<tr><td>Total Volume Flow (liters)</td><td>";
  toReturn+=rosieData.totalMilliLitres/1000;
  toReturn+="</td></tr>";

  toReturn+="<tr><td>Tank Pressure (psi)</td><td>";
  toReturn+=rosieData.tankPressurePsi;
  toReturn+="</td></tr>";
  
  toReturn+="<tr><td>Tank Pressure (volts)</td><td>";
  toReturn+=rosieData.tankPressureVolts;
  toReturn+="</td></tr>";

  toReturn+="<tr><td>Tank Water Height (m)</td><td>";
  toReturn+=rosieData.tankWaterLevel;
  toReturn+="</td></tr>";

  toReturn+="<tr><td>Tank Height (m)</td><td>";
  toReturn+=rosieData.tankHeightMeters;
  toReturn+="</td></tr>";


  toReturn+="<tr><td>Temperature</td><td>";
  toReturn+=rosieData.temperature;
  toReturn+="</td></tr>";

  toReturn+="</table>";
 toReturn+="<br><br>";
toReturn+="<div class=\"row\"><div class=\"col-12\"><h5 style=\"margin-left:0px;margin-right:100px;\">Power</h5></div></div>";
  toReturn+="<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";

    toReturn+="<tr><td>Low Voltage Alert</td>";
  if(rosieData.lowVoltageAlert)toReturn+="<td style=\"color:red\">ON";
  else toReturn+="<td>OFF";
  toReturn+="</td></tr>";


  toReturn+="<tr><td>Solar Voltage</td><td>";
  toReturn+=rosieData.solarVoltage;
  toReturn+="</td></tr>";
  
  toReturn+="<tr><td>Capacitor Voltage</td><td>";
  toReturn+=rosieData.capacitorVoltage;
  toReturn+="</td></tr>";

  toReturn+="<tr><td>RTC Battery Voltage</td><td>";
  toReturn+=rosieData.rtcBatVolt;
  toReturn+="</td></tr>";
  
  toReturn+="<tr><td>3.3V Regulator Voltage</td><td>";
  toReturn+=rosieData.reg33Voltage;
  toReturn+="</td></tr>";
   toReturn+="</td></tr>";
toReturn+="</table>";
 toReturn+="<br><br>";
toReturn+="<div class=\"row\"><div class=\"col-12\"><h5 style=\"margin-left:0px;margin-right:100px;\">Operational Data</h5></div></div>";
  toReturn+="<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";
  String rosieId="";
  for(int i=0;i<8;i++){
        rosieId+=(rosieData.rosieId[i],HEX);
      }
  toReturn+="<tr><td>Id</td><td>"+rosieId+"</td></tr>";
  toReturn+="<tr><td>Field Id</td><td>";
  toReturn+=(uint8_t)rosieData.fieldId;
  toReturn+="</td></tr>";

 long now = timeManager.getCurrentTimeInSeconds(currentTimerRecord);
  
  toReturn+="<tr><td>Seconds since Last Pulse</td><td>";
  long l1=now-rosieData.secondsTime;
  toReturn+=l1;
  toReturn+="</td></tr>";

  toReturn+="<tr><td>Sampling Frequency(sec)</td><td>";
  toReturn+=rosieData.dataSamplingSec;
  toReturn+="</td></tr>";

  toReturn+="<tr><td>Operational Mode Switch  </td><td>";
  if(rosieData.opMode==LOW)toReturn+="PROGRAMMING";
  else toReturn+="RUN";
  toReturn+="</td></tr>";


toReturn+="<tr><td>Operating Status</td><td>";
  if(rosieData.operatingStatus==2){
    toReturn+="Normal";
  }else if(rosieData.operatingStatus==1){
     toReturn+="WPS";
  }else if(rosieData.operatingStatus==0){
     toReturn+="Sleep";
  }
  toReturn+="</td></tr>";

  toReturn+="<tr><td>Sleep Ping Minutes  </td><td>";
   toReturn+=rosieData.sleepPingMinutes;
  toReturn+="</td></tr>";

  toReturn+="<tr><td>Last Message Received At</td><td>";
  toReturn+=lastReceptionRTCInfoRecord.date;
  toReturn+="/";
  toReturn+=lastReceptionRTCInfoRecord.month;
  toReturn+="/";
  toReturn+=lastReceptionRTCInfoRecord.year;
  toReturn+=" ";
  toReturn+=lastReceptionRTCInfoRecord.hour;
  toReturn+=":";
  if(lastReceptionRTCInfoRecord.minute<10){
    toReturn+="0";
   }
  toReturn+=lastReceptionRTCInfoRecord.minute;
  toReturn+=":";
  if(lastReceptionRTCInfoRecord.second<10){
    toReturn+="0";
   }
  toReturn+=lastReceptionRTCInfoRecord.second;
  toReturn+="</td></tr>";

 toReturn+="</table>";
  
    toReturn+="<br><br>";
  
toReturn+="<div class=\"row\">";
    toReturn+="<div class=\"col-9\"><h4 style=\"margin-left:0px;margin-right:100px;\">Rosie Radio Info</h4></div><div class=\"col-3\"></div></div>";

     
  toReturn+="<table style=\"table-layout: fixed;\" class=\"table table-striped\"><tr><th>Data</th><th>Value</th></tr>";
  
  
  toReturn+="<tr><td>Wifi Strength</td><td>";
  toReturn+=WiFi.RSSI();
  toReturn+="</td></tr>";

 toReturn+="<tr><td>Ip Address</td><td>";
  toReturn+=ipAddress;
  toReturn+="</td></tr>";

toReturn+="<tr><td>SSID</td><td>";
  toReturn+=ssid;
  toReturn+="</td></tr>";


  toReturn+="<tr><td>Access Point Name</td><td>";
  toReturn+=soft_ap_ssid;
  toReturn+="</td></tr>";
  
  toReturn+="<tr><td>Access Point IP Address</td><td>";
  toReturn+=apAddress;
  toReturn+="</td></tr>";

  toReturn+="<tr><td>Lora RSSI</td><td>";
  toReturn+=rosieData.rssi;
  toReturn+="</td></tr>";

  
  toReturn+="<tr><td>Lora SNR</td><td>";
  toReturn+=rosieData.snr;
  toReturn+="</td></tr>";

  toReturn+="</table>";
  toReturn+="</div></body>";
  toReturn+="</html>";
  return toReturn;
}

Esp32WifiManager::~Esp32WifiManager() {}