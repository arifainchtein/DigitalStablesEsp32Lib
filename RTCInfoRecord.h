 #ifndef  RTCINFORECORD_H
 #define RTCINFORECORD_H
#include <stdint.h>
struct RTCInfoRecord{

	uint16_t year;
		uint8_t month;
		uint8_t date;
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
		uint8_t dayOfWeek;
		const char* timezoneInfo;
		long timezoneOffset;
		long epoch;
};
#endif
