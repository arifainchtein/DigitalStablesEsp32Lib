 #include "ErrorManager.h"
ErrorManager::ErrorManager() { 
    errors.i2cErrors = 0; 
    errors.loraErrors = 0; 
   // errors.lastRssi = 0; 
}

// I2C Error Methods 
void ErrorManager::setI2CError(I2CDeviceError deviceBit) { errors.i2cErrors |= (1 << deviceBit); }

void ErrorManager::clearI2CError(I2CDeviceError deviceBit) { errors.i2cErrors &= ~(1 << deviceBit); }

bool ErrorManager::hasI2CError(I2CDeviceError deviceBit) { 
    return (errors.i2cErrors & (1 << deviceBit)) != 0; 
}

byte ErrorManager::getAllI2CErrors() { return errors.i2cErrors; }

void ErrorManager::setLoRaError(LoRaError error, float rssi) {
     if(error != LORA_OK) { 
        errors.loraErrors |= (1 << (error - 1)); 
        if(error == LORA_CHANNEL_BUSY) { 
           // errors.lastRssi = rssi; // Store RSSI when channel is busy 
        } 
    } 
}

float ErrorManager::getLastRssi() { return 0;}//errors.lastRssi; }



void ErrorManager::clearLoRaError(LoRaError error) { 
    if(error != LORA_OK) { 
        errors.loraErrors &= ~(1 << (error - 1)); 
    } 
}

bool ErrorManager::hasLoRaError(LoRaError error) {
    if(error == LORA_OK) {
        return errors.loraErrors == 0;  // Returns true if no errors are set
    }
    return (errors.loraErrors & (1 << (error - 1))) != 0;
}

byte ErrorManager::getAllLoRaErrors() { return errors.loraErrors; }

// General Error Management 
void ErrorManager::clearAllErrors() { errors.i2cErrors = 0; errors.loraErrors = 0; }

bool ErrorManager::hasAnyErrors() { return (errors.i2cErrors != 0 || errors.loraErrors != 0); }

ErrorFlags ErrorManager::getErrorFlags() { return errors; }

void ErrorManager::setErrorFlags(ErrorFlags newErrors) { errors = newErrors; } 