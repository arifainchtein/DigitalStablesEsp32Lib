#ifndef LIBRARIES_DIGITALSTABLES_ASYNCDATATRACKER_H_
#define LIBRARIES_DIGITALSTABLES_ASYNCDATATRACKER_H_
#include "Arduino.h"
#include <SerialNumber.h>

class AsyncDataTracker {

  #define MSG_TYPE_DIGITAL_STABLES 1
  #define MSG_TYPE_GLORIA 2
  #define MSG_TYPE_COMMAND 3
  #define MAX_DAFFODILS 20
  
  private:
    bool debug=false;
    struct DaffodilCounter {
      SerialNumber serialNumber;
      int digitalStablesCount;
      int gloriaCount;
      bool inUse;
      
      DaffodilCounter() : digitalStablesCount(0), gloriaCount(0), inUse(false) {}
    };
    
    DaffodilCounter counters[MAX_DAFFODILS];
    
    // Find the index of a Daffodil by serial number, or -1 if not found
    int findDaffodil(const SerialNumber& serialNumber) {
      for (int i = 0; i < MAX_DAFFODILS; i++) {
        if (counters[i].inUse && counters[i].serialNumber.equals(serialNumber)) {
          return i;
        }
      }
      return -1;
    }
    
    // Find an empty slot for a new Daffodil, or -1 if full
    int findEmptySlot() {
      for (int i = 0; i < MAX_DAFFODILS; i++) {
        if (!counters[i].inUse) {
          return i;
        }
      }
      return -1;
    }
  
  public:


    // Increment counter for a specific Daffodil based on data type
    void incrementAsyncCounter(const SerialNumber& serialNumber, uint8_t dataType) {
      int index = findDaffodil(serialNumber);
      
      // If not found, add a new entry
      if (index == -1) {
        index = findEmptySlot();
        if (index == -1) {
          if(debug)Serial.println("ERROR: Maximum number of Daffodils reached!");
          return;
        }
        
        counters[index].serialNumber = serialNumber;
        counters[index].inUse = true;
      }
      
      if (dataType == MSG_TYPE_DIGITAL_STABLES) {
        counters[index].digitalStablesCount++;
        
        if(debug)Serial.print("Received DigitalStables async data from Daffodil #");
        if(debug)Serial.print(serialNumber.toString());
        if(debug)Serial.print(". Total count: ");
        if(debug)Serial.println(counters[index].digitalStablesCount);
      } 
      else if (dataType == MSG_TYPE_GLORIA) {
        counters[index].gloriaCount++;
        
        if(debug)Serial.print("Received Gloria async data from Daffodil #");
        if(debug)Serial.print(serialNumber.toString());
        if(debug)Serial.print(". Total count: ");
        if(debug)Serial.println(counters[index].gloriaCount);
      }
    }
    
    // Get counter for a specific Daffodil based on data type
    int getAsyncCounter(const SerialNumber& serialNumber, uint8_t dataType) {
      int index = findDaffodil(serialNumber);
      if (index == -1) {
        return 0;
      }
      
      if (dataType == MSG_TYPE_DIGITAL_STABLES) {
        return counters[index].digitalStablesCount;
      } 
      else if (dataType == MSG_TYPE_GLORIA) {
        return counters[index].gloriaCount;
      }
      return 0;
    }
    
    // Reset counter for a specific Daffodil based on data type
    void resetAsyncCounter(const SerialNumber& serialNumber, uint8_t dataType) {
      int index = findDaffodil(serialNumber);
      if (index == -1) {
        return;
      }
      
      if (dataType == MSG_TYPE_DIGITAL_STABLES) {
        counters[index].digitalStablesCount = 0;
        if(debug)Serial.print("Reset DigitalStables async counter for Daffodil #");
        if(debug)Serial.println(serialNumber.toString());
      } 
      else if (dataType == MSG_TYPE_GLORIA) {
        counters[index].gloriaCount = 0;
        if(debug)Serial.print("Reset Gloria async counter for Daffodil #");
        if(debug)Serial.println(serialNumber.toString());
      }
    }
  };
#endif