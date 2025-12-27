#ifndef LIBRARIES_DIGITALSTABLES_SERIALNUMBER_H_
#define LIBRARIES_DIGITALSTABLES_SERIALNUMBER_H_
#include "Arduino.h"

class SerialNumber {
  public:
    uint8_t bytes[8];
    
    SerialNumber() {
      memset(bytes, 0, 8);
    }
    
    SerialNumber(const uint8_t* array) {
      memcpy(bytes, array, 8);
    }
    
    // For equality comparison
    bool equals(const SerialNumber& other) const {
      return memcmp(bytes, other.bytes, 8) == 0;
    }
    
    // Convert to string for display and commands
    String toString() const {
      String result = "";
      for (int i = 0; i < 8; i++) {
        if (bytes[i] < 16) result += "0"; // Ensure two digits
        result += String(bytes[i], HEX);
      }
      return result;
    }
    
    // Create from hex string
    static SerialNumber fromString(const String& hexString) {
      SerialNumber sn;
      if (hexString.length() != 16) return sn; // Invalid length
      
      for (int i = 0; i < 8; i++) {
        String byteStr = hexString.substring(i*2, i*2+2);
        sn.bytes[i] = strtol(byteStr.c_str(), NULL, 16);
      }
      return sn;
    }
};
#endif