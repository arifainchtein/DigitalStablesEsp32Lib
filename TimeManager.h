/*
 * TimeManager.h
 *
 *  Created on: 13 Feb. 2019
 *      Author: arifainchtein
 */

#ifndef TIMEMANAGER_H_
#define TIMEMANAGER_H_
#include <Arduino.h>
#include <RTCInfoRecord.h>
#include <GeneralFunctions.h>

class TimeManager{

	protected:
		HardwareSerial& _HardSerial;


	public:
	
		#define LEAP_YEAR(_year) ((_year%4)==0)

		int timeZoneHours=10;
		int SECONDOFFSET=10;

		TimeManager( HardwareSerial& serial);

		virtual void start()=0;
		virtual long dateAsSeconds(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second)=0;
		virtual void hourlyTasks(long time, int previousHour )=0;
		virtual void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear )=0;
		virtual void monthlyTasks(long time)=0;
		virtual void yearlyTasks(long time)=0;
		virtual long getCurrentTimeInSeconds()=0;
		virtual RTCInfoRecord now()=0;
		virtual long getTimeForCodeGeneration(RTCInfoRecord)=0;
		
		virtual long getTimeForCodeGeneration()=0;
		//
		// Functions that represent Serial commands
		//
		virtual bool printTimeToSerial(RTCInfoRecord)=0;
		virtual bool setTime(String)=0;
		//virtual char* epochToString(unsigned long epoch)=0;

        virtual ~TimeManager();

	protected:
		char timezoneInfo[50]; 
		
	private:
		
		virtual long getTimezoneOffset(const uint16_t year,uint8_t month,uint8_t date)=0;
		virtual int calculateNthWeekday(int month, int nth, int weekday, int year)=0;

	};
#endif /* TIMEMANAGER_H_ */
