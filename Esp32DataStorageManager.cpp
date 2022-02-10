/*
 * Esp32DataStorageManager.cpp
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#include <Esp32DataStorageManager.h>

#include <Preferences.h>

Preferences preferences;


Esp32DataStorageManager::Esp32DataStorageManager(TimeManager& t):SecretManager(t){}


void Esp32SecreEsp32DataStorageManagertManager::saveConfigData(int fieldId, String stationName ){

	preferences.begin("ConfigData", false);
	preferences.putString("stationName", stationName);
	preferences.putUInt("fieldId", fieldId);
	preferences.end();
}


String Esp32DataStorageManager::getStationName(){
	preferences.begin("ConfigData",false);
	String stationName = preferences.getString("stationName");
	preferences.end();
	return stationName;
}

int Esp32DataStorageManager::getFieldId(){
	preferences.begin("ConfigData",false);
	int fieldId = preferences.getUInt("fieldId");
	preferences.end();
	return fieldId;
}



Esp32SecretManager::~Esp32SecretManager() {}
