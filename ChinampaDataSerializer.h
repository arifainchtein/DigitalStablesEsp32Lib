#ifndef LIBRARIES_DIGITALSTABLES_CHINAMPADATASERIALIZER_H_
#define LIBRARIES_DIGITALSTABLES_CHINAMPADATASERIALIZER_H_

#include "Arduino.h"
#include <ChinampaData.h>

class ChinampaDataSerializer{

    public:
        void pushToSerial(HardwareSerial& serial , ChinampaData g);
        virtual ~ChinampaDataSerializer();
};
#endif