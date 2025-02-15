#ifndef ERROR_DEFINITIONS_H
#define ERROR_DEFINITIONS_H


// Define bit positions for I2C devices 
enum I2CDeviceError { 
    I2C_DEVICE0_ERROR = 0, 
    I2C_DEVICE1_ERROR, 
    I2C_DEVICE2_ERROR, 
    I2C_DEVICE3_ERROR, 
    I2C_DEVICE4_ERROR, 
    I2C_DEVICE5_ERROR, 
    I2C_DEVICE6_ERROR, 
    I2C_DEVICE7_ERROR 
};

// LoRa error enum
enum LoRaError { 
    LORA_OK = 0, 
    LORA_INIT_FAILED, 
    LORA_CAD_TIMEOUT, 
    LORA_CHANNEL_BUSY, 
    LORA_TX_FAILED, 
    LORA_MAX_RETRIES_REACHED 
};

struct LoRaErrorInfo { 
    float lastRssi; // Store the RSSI value when error occurred LoRaError error; // The actual error 
};
#endif