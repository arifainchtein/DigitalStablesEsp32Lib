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




void Esp32SecretManager::saveConfigData(float fieldId, String stationName ){

	preferences.begin("ConfigData", false);
	preferences.putString("stationName", stationName);
	preferences.putFloat("fieldId", fieldId);
	preferences.end();
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

Esp32SecretManager::~Esp32SecretManager() {}
