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
	preferences.getString("Secret");
	preferences.begin("SecretManager",false);
	String ret = preferences.getString("Secret");

	preferences.getString("Secret", secretCode, 27);
}

String Esp32SecretManager::readSecret(){

	preferences.getString("Secret");
	preferences.begin("SecretManager",false);
	String ret = preferences.getString("Secret");
	preferences.end();
	return ret;
}

long Esp32SecretManager::generateCode(){



	long timestamp = timeManager.getTimeForCodeGeneration();

	char secretCode[SHARED_SECRET_LENGTH];
	String secretCode = readSecret();
	TOTP totp = TOTP(secretCode);
	long code=totp. gen_code  (timestamp ) ;
	//
	// now check to see if this code is already in the history
	boolean found=false;
	for(int i=0;i<numberOfCommandCodesInHistory;i++){
		if(commandCodeHistory[i]==code){
			found=true;
		}
	}

	if(!found){
		//
		// now check to see if this value is in the history
		if(currentCommandCodeHistoryPos<numberOfCommandCodesInHistory){
			commandCodeHistory[currentCommandCodeHistoryPos]=code;
			currentCommandCodeHistoryPos++;
		}else{
			for(int i=0;i<numberOfCommandCodesInHistory-1;i++){
				commandCodeHistory[i]=commandCodeHistory[i+1];
			}
			commandCodeHistory[numberOfCommandCodesInHistory-1]=code;
		}
	}

	return code;
}

Esp32SecretManager::~Esp32SecretManager() {}
