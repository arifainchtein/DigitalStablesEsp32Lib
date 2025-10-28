/*
 * Esp32SecretManager.cpp
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#include <Esp32SecretManager.h>

#include <Preferences.h>

Preferences preferences;

Esp32SecretManager::Esp32SecretManager(TimeManager& t):SecretManager(t){}


// void Esp32SecretManager::storeDigitalStablesDataSleepSample( DigitalStablesData& dsd ){
// 	preferences.begin("DeviceSenInf", false);

// 	unsigned int dataCount = preferences.getUInt("count", 0);  
// 	char key[15];
//   	sprintf(key, "data_%d", dataCount);
//   	preferences.putBytes(key, &dsd, sizeof(dsd));
// 	preferences.putUInt("count", dataCount + 1);
// 	preferences.end();
// }

// void Esp32SecretManager::clearDigitalStablesDataSleepSample( DigitalStablesData& dsd ){
// 	preferences.begin("DeviceSenInf", false);
// 	preferences.clear(); 
// 	preferences.end();
// }

// void Esp32SecretManager::readDigitalStablesDataSleepSample( DigitalStablesData& dsd ){
// 	preferences.begin("DeviceSenInf", false);
// 	preferences.clear(); 
// 	preferences.end();
// }
void Esp32SecretManager::saveTroughParameters(double troughcolumnheight, double troughlevelminimumcm, double troughlevelmaximumcm){
	preferences.begin("TroughParams", false);
	preferences.putDouble("columnheight", troughcolumnheight);
	preferences.putDouble("minimumcm", troughlevelminimumcm);
	preferences.putDouble("maximumcm", troughlevelmaximumcm);
	//serial.println("line 44, troughcolumnheight=" + String(troughcolumnheight));
	preferences.end();
}
	
void Esp32SecretManager::getTroughParameters( double& troughcolumnheight, double& troughlevelminimumcm, double& troughlevelmaximumcm) {
    preferences.begin("TroughParams", true);
   
    
    // Check if latitude and longitude are available, if not set default values  
    if (!preferences.isKey("columnheight")) {
        troughcolumnheight = 65;
	//	serial.println("line 55, columnheight");
    } else {
        troughcolumnheight = preferences.getDouble("columnheight", 68);
    }

	 if (!preferences.isKey("minimumcm")) {
        troughlevelminimumcm = 25;
    } else {
        troughlevelminimumcm = preferences.getDouble("minimumcm", 28);
    }
    if (!preferences.isKey("maximumcm")) {
        troughlevelmaximumcm = 35;
    } else {
        troughlevelmaximumcm = preferences.getDouble("maximumcm", 38);
    }
   

    preferences.end();
}

void Esp32SecretManager::saveSecret(String secret, int numberDigits, int periodSeconds ){

	preferences.begin("SecretManager", false);
	preferences.putString("Secret", secret);
	preferences.putUInt("NumberDigits", numberDigits);
	preferences.putUInt("PeriodSeconds", periodSeconds);
	preferences.end();
}
String Esp32SecretManager::readSecret(){
	preferences.begin("SecretManager",false);
	String ret = preferences.getString("Secret");
	preferences.end();
	return ret;
}

void Esp32SecretManager::setTimeZone( String timezone ){
	preferences.begin("DeviceSenInf", false);
	preferences.putString("timezone", timezone);
	preferences.end();
}


String Esp32SecretManager::readTimeZone(){
	preferences.begin("DeviceSenInf", false);
	String ret = preferences.getString("timezone");
	preferences.end();
	return ret;
}

void Esp32SecretManager::saveDeviceConfig(String devicename,String deviceshortname, String timezone, double latitude, double longitude){
	preferences.begin("DeviceSenInf", false);
	preferences.putString("devicename", devicename);
	preferences.putString("deviceshortname", deviceshortname);
	preferences.putString("timezone", timezone);
	preferences.putDouble("latitude", latitude);
    preferences.putDouble("longitude", longitude);
	preferences.end();
}	

void Esp32SecretManager::getDeviceConfig(char* devicename, char* deviceshortname, String& timezone, double& latitude, double& longitude) {
    preferences.begin("DeviceSenInf", true);
    String tempString = preferences.getString("devicename", "NoData");
    strcpy(devicename, tempString.c_str());
    tempString = preferences.getString("deviceshortname", "NoData");
    strcpy(deviceshortname, tempString.c_str());
    timezone = preferences.getString("timezone", "NoData");
    
    // Check if latitude and longitude are available, if not set default values  
    if (!preferences.isKey("latitude")) {
        latitude = -37.13305556;
    } else {
        latitude = preferences.getDouble("latitude", -37.13305556);
    }
    
    if (!preferences.isKey("longitude")) {
        longitude = 144.47472222;
    } else {
        longitude = preferences.getDouble("longitude", 144.47472222);
    }
    

    preferences.end();
}


void Esp32SecretManager::saveDeviceSensorConfig(String devicename,String deviceshortname, String sensor1name, String sensor2name, String timezone, double latitude, double longitude, double altitude,uint8_t minimumEfficiencyForLed, uint8_t minimumEfficiencyForWifi){
	preferences.begin("DeviceSenInf", false);
	preferences.putString("devicename", devicename);
	preferences.putString("deviceshortname", deviceshortname);
	preferences.putString("sensor1name", sensor1name);
	preferences.putString("sensor2name", sensor2name);
	preferences.putString("timezone", timezone);
	preferences.putDouble("latitude", latitude);
    preferences.putDouble("longitude", longitude);
    preferences.putDouble("altitude", altitude);
    preferences.putInt("mineffled",minimumEfficiencyForLed);
	preferences.putInt("mineffeifi", minimumEfficiencyForWifi);
	preferences.end();
}	

void Esp32SecretManager::getDeviceSensorConfig(char* devicename, char* deviceshortname, char* sensor1name, char* sensor2name, String& timezone, double& latitude, double& longitude, double& altitude,uint8_t& minimumEfficiencyForLed, uint8_t& minimumEfficiencyForWifi) {
    preferences.begin("DeviceSenInf", true);
    String tempString = preferences.getString("devicename", "NoData");
    strcpy(devicename, tempString.c_str());
    tempString = preferences.getString("deviceshortname", "NoData");
    strcpy(deviceshortname, tempString.c_str());
    tempString = preferences.getString("sensor1name", "NoData");
    strcpy(sensor1name, tempString.c_str());
    tempString = preferences.getString("sensor2name", "NoData");
    strcpy(sensor2name, tempString.c_str());
    timezone = preferences.getString("timezone", "NoData");
    
    // Check if latitude and longitude are available, if not set default values  
    if (!preferences.isKey("latitude")) {
        latitude = -37.13305556;
    } else {
        latitude = preferences.getDouble("latitude", -37.13305556);
    }
    
    if (!preferences.isKey("longitude")) {
        longitude = 144.47472222;
    } else {
        longitude = preferences.getDouble("longitude", 144.47472222);
    }
    
	if (!preferences.isKey("altitude")) {
        altitude = 410;
    } else {
        altitude = preferences.getDouble("altitude", 410);
    }

	if (!preferences.isKey("mineffled")) {
        minimumEfficiencyForLed = 40;
    } else {
        minimumEfficiencyForLed = preferences.getInt("mineffled", 30);
    }
	if (!preferences.isKey("mineffeifi")) {
        minimumEfficiencyForWifi = 50;
    } else {
        minimumEfficiencyForWifi = preferences.getInt("mineffeifi", 50);
    }

    preferences.end();
}

String Esp32SecretManager::readDeviceName(){
	preferences.begin("DeviceSenInf", false);
	String ret = preferences.getString("devicename");
	preferences.end();
	return ret;
}
String Esp32SecretManager::readDeviceShortName(){
	preferences.begin("DeviceSenInf", false);
	String ret = preferences.getString("deviceshortname");
	preferences.end();
	return ret;
}

String Esp32SecretManager::readFlow1Name(){
	preferences.begin("DeviceSenInf", false);
	String ret = preferences.getString("flow1name");
	preferences.end();
	return ret;
}

String Esp32SecretManager::readFlow2Name(){
	preferences.begin("DeviceSenInf", false);
	String ret = preferences.getString("flow2name");
	preferences.end();
	return ret;
}

String Esp32SecretManager::readTank1Name(){
	preferences.begin("DeviceSenInf", false);
	String ret = preferences.getString("tank1name");
	preferences.end();
	return ret;
}

String Esp32SecretManager::readTank2Name(){
	preferences.begin("DeviceSenInf", false);
	String ret = preferences.getString("tank2name");
	preferences.end();
	return ret;
}
void Esp32SecretManager::saveConfigData(float fieldId, String stationName ){

	preferences.begin("ConfigData", false);
	preferences.putString("stationName", stationName);
	preferences.putFloat("fieldId", fieldId);	
	preferences.end();
}

void Esp32SecretManager::setGroupIdentifier( String groupIdentifier ){
	preferences.begin("ConfigData", false);
	preferences.putString("groupIdentifier", groupIdentifier);
	preferences.end();
}

String Esp32SecretManager::getGroupIdentifier(){
	preferences.begin("ConfigData",false);
	String groupIdentifier = preferences.getString("groupIdentifier");
	preferences.end();
	return groupIdentifier;
}

String Esp32SecretManager::getStationName(){
	preferences.begin("ConfigData",false);
	String stationName = preferences.getString("stationName");
	preferences.end();
	return stationName;
}

float Esp32SecretManager::getFieldId(){
	preferences.begin("ConfigData",false);
	int fieldId = preferences.getFloat("fieldId");
	preferences.end();
	return fieldId;
}

void Esp32SecretManager::saveOperatingStatus(float operatingStatus ){
	preferences.begin("OperationalData", false);
	preferences.putFloat("operatingStatus", operatingStatus);
	preferences.end();
}

float Esp32SecretManager::getOperatingStatus(){
	preferences.begin("OperationalData",false);
	float operatingStatus = preferences.getFloat("operatingStatus");
	if(isnan(operatingStatus)){
		operatingStatus=3;
	}
	preferences.end();
	return operatingStatus;
}

void  Esp32SecretManager::saveSleepPingMinutes(float pingMinutes ){
	preferences.begin("OperationalData", false);
	preferences.putFloat("pingMinutes", pingMinutes);
	preferences.end();
}

float Esp32SecretManager::getSleepPingMinutes(){
	preferences.begin("OperationalData",false);
	float pingMinutes = preferences.getFloat("pingMinutes");
	preferences.end();
	return pingMinutes;
}


void Esp32SecretManager::saveWifiParameters(String ssid, String password, String softAPSSID, String softAPPASS,  String hostName, bool stationmode){
	preferences.begin("WIFIData",false);
	preferences.putString("ssid", ssid);
	preferences.putString("password", password);
	preferences.putString("softAPSSID", softAPSSID);
	preferences.putString("softAPPASS", softAPPASS);
	preferences.putString("hostName", hostName);
	preferences.putBool("stationmode", stationmode);
	preferences.end();
}

bool Esp32SecretManager::getStationMode(){
	preferences.begin("WIFIData",false);
	bool stationmode = false;
	if(preferences.isKey("stationmode"))stationmode=preferences.getBool("stationmode","");
	preferences.end();
	return stationmode;
}

String Esp32SecretManager::getSSID(){
	preferences.begin("WIFIData",false);
	String ssid = "";
	if(preferences.isKey("ssid"))ssid=preferences.getString("ssid","");
	preferences.end();
	return ssid;
}

String Esp32SecretManager::getWifiPassword(){
	preferences.begin("WIFIData",false);
	String password = "";
	if(preferences.isKey("password"))password=preferences.getString("password","");
	preferences.end();
	return password;
}

String Esp32SecretManager::getSoftAPSSID(){
	preferences.begin("WIFIData",false);
	String softAPSSID = "";
	if(preferences.isKey("softAPSSID"))softAPSSID=preferences.getString("softAPSSID","");
	preferences.end();
	return softAPSSID;
}

String Esp32SecretManager::getSoftAPPASS(){
	preferences.begin("WIFIData",false);
	String softAPPASS = "";
	if(preferences.isKey("softAPPASS"))softAPPASS=preferences.getString("softAPPASS","");
	preferences.end();
	return softAPPASS;
}

String Esp32SecretManager::getHostName(){
	preferences.begin("WIFIData",false);
	String hostName = "";
	if(preferences.isKey("hostName"))hostName=preferences.getString("hostName","");
	preferences.end();
	return hostName;
}

Esp32SecretManager::~Esp32SecretManager() {}
