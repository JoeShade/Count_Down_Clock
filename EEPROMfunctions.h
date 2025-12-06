#include <Arduino.h>

#ifndef EEPROMFUNCTIONS_H
#define EEPROMFUNCTIONS_H

void clearEEPROM();
void writeToEEPROM(int address, uint32_t value);
uint32_t readFromEEPROM(int address); 

#endif