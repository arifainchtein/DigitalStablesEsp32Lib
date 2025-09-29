#ifndef LIBRARIES_DIGITALSTABLES_SEEDLINGMONITORDATASERIALIZER_H_
#define LIBRARIES_DIGITALSTABLES_SEEDLINGMONITORDATASERIALIZER_H_

#include "Arduino.h"
#include <SeedlingMonitoringData.h>

class SeedlingMonitorDataSerializer{

    public:
        void pushToSerial(HardwareSerial& serial , SeedlingMonitorData g);
        virtual ~SeedlingMonitorDataSerializer();
};
#endif