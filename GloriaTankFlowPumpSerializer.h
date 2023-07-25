#ifndef LIBRARIES_DIGITALSTABLES_GLORIATANKFLOWPUMPSERIALIZER_H_
#define LIBRARIES_DIGITALSTABLES_GLORIATANKFLOWPUMPSERIALIZER_H_

#include "Arduino.h"
#include <GloriaTankFlowPumpData.h>

class GloriaTankFlowPumpSerializer{

    public:
        GloriaTankFlowPumpSerializer();
        void pushToSerial(HardwareSerial& serial , GloriaTankFlowPumpData& g);
        virtual ~GloriaTankFlowPumpSerializer();
};
#endif