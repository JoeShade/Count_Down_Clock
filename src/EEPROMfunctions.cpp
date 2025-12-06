#include "EEPROMfunctions.h"
#include <EEPROM.h>
#include <Arduino.h>

// Create function to clear EEPROM
void clearEEPROM() {
 for(int i = 0; i < EEPROM.length(); i++) {
      EEPROM.write(i, 255);
    }
   EEPROM.commit();
   delay(5000);
   Serial.println("EEPROM Cleared");
}

// Create function to write Julian Date to EEPROM
void writeToEEPROM(int address, uint32_t value) {
    EEPROM.write(address, value & 0xFF);
    EEPROM.write(address + 1, (value >> 8 ) & 0xFF);
    EEPROM.write(address + 2, (value >> 16 ) & 0xFF);
    EEPROM.write(address + 3, (value >> 24 ) & 0xFF);
    EEPROM.commit();
}

// Create function to read Julian Date from EEPROM
uint32_t readFromEEPROM(int address) {
    uint32_t value = EEPROM.read(address);
    value |= ((uint32_t)EEPROM.read(address + 1) << 8);
    value |= ((uint32_t)EEPROM.read(address + 2) << 16);
    value |= ((uint32_t)EEPROM.read(address + 3) << 24);
    return value;
}
