#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

#include <Arduino.h>
#include <ErrorDefinitions.h>

struct ErrorFlags {
    byte i2cErrors;   // Each bit represents a different I2C device
    byte loraErrors;  // Each bit represents a different LoRa error from enum
};


class ErrorManager {
    private:
        ErrorFlags errors;
       

    public:
        ErrorManager();  // Constructor
        
        // I2C Error management methods
        void setI2CError(I2CDeviceError deviceBit);
        void clearI2CError(I2CDeviceError deviceBit);
        bool hasI2CError(I2CDeviceError deviceBit);
        byte getAllI2CErrors();
        
        
         void setLoRaError(LoRaError error, float rssi = 0);  
        void clearLoRaError(LoRaError error);
        bool hasLoRaError(LoRaError error);
        byte getAllLoRaErrors();
        float getLastRssi();  
        // General error management
        void clearAllErrors();
        bool hasAnyErrors();

        // Get/Set complete error structure
        ErrorFlags getErrorFlags();
        void setErrorFlags(ErrorFlags newErrors);
};

#endif