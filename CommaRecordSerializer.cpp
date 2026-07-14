#include <CommaRecordSerializer.h>

void CommaRecordSerializer::pushToSerial(HardwareSerial& serial, CommaRecord r) {
    serial.print("CommaRecordDeserializer");
    serial.print(F("#"));
    serial.print(r.time);
    serial.print(F("#"));
    serial.print(r.voltage, 4);
    serial.print(F("#"));
    for (int i = 0; i < (int)sizeof(r.devicename); i++) {
        if (r.devicename[i] != '\0') serial.print(r.devicename[i]);
    }
    serial.print(F("#"));
    for (int i = 0; i < (int)sizeof(r.serialnumber); i++) {
        serial.print(r.serialnumber[i], HEX);
    }
    serial.print(F("#"));
    serial.print(r.totpcode);
    serial.print(F("#"));
    serial.print(r.index);
    serial.print(F("#"));
    serial.print(r.total);
    serial.print(F("#"));
    serial.println(r.checksum);
}

CommaRecordSerializer::~CommaRecordSerializer() {}
