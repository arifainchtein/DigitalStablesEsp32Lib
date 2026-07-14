#ifndef LIBRARIES_DIGITALSTABLES_LANGLEYDATASERIALIZER_H_
#define LIBRARIES_DIGITALSTABLES_LANGLEYDATASERIALIZER_H_

#include "Arduino.h"
#include <LangleyData.h>

class LangleyDataSerializer {
public:
    void pushToSerial(HardwareSerial& serial, LangleyData data);
    virtual ~LangleyDataSerializer();
};
#endif
