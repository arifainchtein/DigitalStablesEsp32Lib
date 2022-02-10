/*
 * Esp32SecretManager.cpp
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#include <ESP32DataStorageManager.h>

#include <Preferences.h>

Preferences preferences;


ESP32DataStorageManager::ESP32DataStorageManager(){}


void ESP32DataStorageManager::saveSecret(String secret, int numberDigits, int periodSeconds ){

	preferences.begin("SecretManager", false);
	preferences.putString("Secret", secret);
	preferences.putUInt("NumberDigits", numberDigits);
	preferences.putUInt("PeriodSeconds", periodSeconds);
	preferences.end();
}



String Esp3ESP32DataStorageManager2SecretManager::readSecret(){
	preferences.begin("SecretManager",false);
	String ret = preferences.getString("Secret");
	preferences.end();
	return ret;
}



Esp32SecretManager::~Esp32SecretManager() {}
