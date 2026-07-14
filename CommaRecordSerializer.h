#ifndef LIBRARIES_DIGITALSTABLES_COMMARECORDSERIALIZER_H_
#define LIBRARIES_DIGITALSTABLES_COMMARECORDSERIALIZER_H_

#include "Arduino.h"
#include <CommaRecord.h>

class CommaRecordSerializer {
public:
    void pushToSerial(HardwareSerial& serial, CommaRecord r);
    virtual ~CommaRecordSerializer();
};
#endif
