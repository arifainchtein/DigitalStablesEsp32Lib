#ifndef LIBRARIES_DIGITALSTABLES_PANCHOTANKFLOWSERIALIZER_H_
#define LIBRARIES_DIGITALSTABLES_PANCHOTANKFLOWSERIALIZER_H_

#include "Arduino.h"
#include <PanchoTankFlowData.h>

class PanchoTankFlowSerializer{

    public:
        void pushToSerial(HardwareSerial& serial , PanchoTankFlowData& g);
        virtual ~PanchoTankFlowSerializer();
};
#endif