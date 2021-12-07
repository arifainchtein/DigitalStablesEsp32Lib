/*
 * Esp32SecretManager.cpp
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#include <Esp32SecretManager.h>
#include <TimeManager.h>

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

void Esp32SecretManager::readSecret(char *secretCode){
	preferences.getString("Secret", secretCode, 27);
}

String Esp32SecretManager::readSecret(){

	preferences.getString("Secret");
	preferences.begin("SecretManager",false);
	String ret = preferences.getString("Secret");
	preferences.end();
	return ret;
}
Esp32SecretManager::~Esp32SecretManager() {}
