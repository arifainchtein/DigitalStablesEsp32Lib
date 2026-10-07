#ifndef LIBRARIES_DIGITALSTABLES_VITALSIGNSSERIALIZER_H_
#define LIBRARIES_DIGITALSTABLES_VITALSIGNSSERIALIZER_H_

#include "Arduino.h"
#include <VitalSignsRecord.h>

// One line per record, read by VitalSignsDeserializer.java (Teleonome framework):
// VitalSignsDeserializer#<serialHex>#<version>#<firmwareBuild>#<resetCount>#<lastResetReason>
//   #<lastResetTime>#<wakeCount>#<earlyWakeCount>#<commaWakeCount>#<awakeSecondsTotal>
//   #<sleptSecondsTotal>#<lastWakeCause>#<lastWakeDriftSec>#<lastAwakeSec>#<wakeVoltage_mV>
//   #<minVoltageSinceReport_mV>#<txDurationMs>#<txBatteryPre_mA>#<txBatteryPeak_mA>
//   #<txBatteryPost_mA>#<txPanel_mA>#<txV50i_mV>#<txMinVoltage_mV>#<loraTxFailCount>#<seq>
//   #<i2cDeviceMask>#<rssi>#<snr>#<receivedAgeSeconds>
// = 30 tokens. receivedAgeSeconds is how long ago Annabelle received the record; the Hypothalamus
// subtracts it from its own clock. Annabelle's RTC is local wall time and isn't trusted for an
// absolute timestamp (no DST switch, no timezone conversion in TimeUtils::getEpochTime unless
// parseTimezone() was called - see conversation 2026-10-05). serialHex uses the same per-byte HEX (no leading zeros) convention as the other
// serializers, so it matches the "Serial Number" the Teleonome already knows each device by.
class VitalSignsSerializer {
public:
    void pushToSerial(HardwareSerial& serial, const VitalSignsRecord& r, float rssi, float snr, uint32_t receivedAgeSeconds);
    virtual ~VitalSignsSerializer();
};
#endif
