#ifndef LIBRARIES_DIGITALSTABLES_DIGITALSTABLESDATASERIALIZER_H_
#define LIBRARIES_DIGITALSTABLES_DIGITALSTABLESDATASERIALIZER_H_

#include "Arduino.h"
#include <DigitalStablesData.h>

class DigitalStablesDataSerializer{

    public:
        void pushToSerial(HardwareSerial& serial , DigitalStablesData g);
        virtual ~DigitalStablesDataSerializer();
};
#endif