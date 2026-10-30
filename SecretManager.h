/*
  SecretManager.h - Library Managing the TOTP
  Created by Ari Fainchtein, Feb 13, 2019.
  Released into the public domain.
*/
#ifndef SecretManager_h
#define SecretManager_h
#include "Arduino.h"
#include <TimeManager.h>


class SecretManager{
	protected:
	TimeManager & timeManager;



	public:
		SecretManager(TimeManager & t) ;
		virtual void saveSecret(String secret, int numberDigits, int periodSeconds )=0;
		virtual String  readSecret()=0;
		long generateCode();
		// Same as generateCode()/checkCode() but at a time the caller already has (a sketch's
		// currentTimerRecord), so the RTC is not read again - 2026-10-13, Daffodil reads it once a tick.
		long generateCode(long timestamp);
		long generateCodeAt(long timestamp);  // code for a given time; does not touch the code history
		boolean checkCode(long userCode);
		boolean checkCode(long userCode, long timestamp);
		static constexpr int SHARED_SECRET_LENGTH=27;
		long* getCommandCodeHistory() ;
	private:

};

#endif
