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

void Esp32SecretManager::saveDeviceSensorConfig(String devicename,String deviceshortname, String flow1name, String flow2name, String tank1name, String tank2name, String timezone){

	preferences.begin("DeviceSenInf", false);
	preferences.putString("devicename", devicename);
	preferences.putString("deviceshortname", deviceshortname);
	preferences.putString("flow1name", flow1name);
	preferences.putString("flow2name", flow2name);
	preferences.putString("tank1name", tank1name);
	preferences.putString("tank2name", tank2name);
	preferences.putString("timezone", timezone);
	preferences.end();
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
