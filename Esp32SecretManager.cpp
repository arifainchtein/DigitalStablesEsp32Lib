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


Esp32SecretManager::Esp32SecretManager(TimeManager& t):SecretManager(t){
	preferences.begin("SecretManager", false);
}

void Esp32SecretManager::saveSecret(String secret, int numberDigits, int periodSeconds ){
	preferences.putString("Secret", secret);
}
void Esp32SecretManager::readSecret(char *secretCode){
	preferences.getString("secret"y, secretCode, 10);
}

Esp32SecretManager::~Esp32SecretManager() {}
