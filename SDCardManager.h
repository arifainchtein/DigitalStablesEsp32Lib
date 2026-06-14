#ifndef SDCARDMANAGER_H
#define SDCARDMANAGER_H

#include <SD.h>
#include <SPI.h>

class SDCardManager {
private:
    int _csPin;
    const char* _logFile = "/data.txt";
    const uint32_t _minFreeMB = 50;
    const uint32_t _recordsToKeep = 1000; 

public:
    SDCardManager(int csPin) : _csPin(csPin) {}

    bool begin() {
        if (!SD.begin(_csPin)) return false;
        return true;
    }

    /**
     * Trims data.txt to keep only the most recent N records.
     */
    void trimFile() {
        Serial.print("Trimming file: "); Serial.println(_logFile);

        File original = SD.open(_logFile, FILE_READ);
        if (!original) return;

        uint32_t totalLines = 0;
        while (original.available()) {
            original.readStringUntil('\n');
            totalLines++;
        }

        if (totalLines <= _recordsToKeep) {
            original.close();
            return;
        }

        uint32_t linesToRemove = totalLines - _recordsToKeep;
        original.seek(0); 

        for (uint32_t i = 0; i < linesToRemove; i++) {
            original.readStringUntil('\n');
        }

        File temp = SD.open("/temp.txt", FILE_WRITE);
        while (original.available()) {
            temp.write(original.read());
        }

        original.close();
        temp.close();

        SD.remove(_logFile);
        if (SD.rename("/temp.txt", _logFile)) {
            Serial.println("Trim successful.");
        } else {
            Serial.println("Trim failed during rename.");
        }
    }

    void maintainFreeSpace() {
        uint64_t freeBytes = SD.totalBytes() - SD.usedBytes();
        uint32_t freeMB = (uint32_t)(freeBytes / (1024 * 1024));

        if (freeMB < _minFreeMB) {
            Serial.println("Low SD space! Cleaning oldest records...");
            trimFile(); 
        }
    }

    // --- Unified Logging ---
    void logGeneric(String dataString) {
        maintainFreeSpace();
        File dataFile = SD.open(_logFile, FILE_APPEND);
        if (dataFile) {
            dataFile.println(dataString);
            dataFile.close();
        }
    }

    // --- Stream Methods ---
    void streamRecords(uint32_t startTime, uint32_t endTime, Print& output) {
        File dataFile = SD.open(_logFile, FILE_READ);
        if (!dataFile) {
            output.println("Error: Could not open file.");
            return;
        }

        while (dataFile.available()) {
            String line = dataFile.readStringUntil('\n');
            if (line.length() < 5) continue;

            int firstHash = line.indexOf('#');
            int secondHash = line.indexOf('#', firstHash + 1);
            
            if (firstHash != -1 && secondHash != -1) {
                uint32_t recordTime = line.substring(firstHash + 1, secondHash).toInt();
                if (recordTime >= startTime && recordTime <= endTime) {
                    output.println(line);
                }
            }
        }
        dataFile.close();
    }

    void streamRecordsByDevice(const char* device, uint32_t startTime, uint32_t endTime, Print& output) {
        File dataFile = SD.open(_logFile, FILE_READ);
        if (!dataFile) {
            output.println("Error: Could not open file.");
            return;
        }

        while (dataFile.available()) {
            String line = dataFile.readStringUntil('\n');
            if (line.length() < 5) continue;

            int firstHash = line.indexOf('#');
            String devName = line.substring(0, firstHash);

            if (devName.equals(device)) {
                int secondHash = line.indexOf('#', firstHash + 1);
                if (firstHash != -1 && secondHash != -1) {
                    uint32_t recordTime = line.substring(firstHash + 1, secondHash).toInt();
                    if (recordTime >= startTime && recordTime <= endTime) {
                        output.println(line);
                    }
                }
            }
        }
        dataFile.close();
    }
};

#endif