#include "Restart.h"
#include "Arduino.h"
#include "RGBmatrixPanel.h"
#include "EEPROMfunctions.h"
#include <avr/wdt.h> // Load watch dog timer library

extern RGBmatrixPanel matrix;
volatile byte resetEnable = LOW;
volatile uint8_t timeoutCounter __attribute__ ((section (".noinit")));


// Create restart function
void restartArduino() {
  wdt_enable(WDTO_15MS);  // Enable watchdog timer with a 15ms timeout
  while (1);  // Wait for watchdog reset
}

// Create WDT reset function
void watchdogReset() {
wdt_reset();
}

// Create WDT start function
void watchdogStart() {
  wdt_enable(WDTO_8S); // Enable watchdog timer with 8s timeout
  while (1); // Wait fror watchdog reset
}

// Creat reset function 
void reset() {
  resetEnable = !resetEnable;
}

void handleReset() {
  if(resetEnable == HIGH) {

  for(int i = 0; i < 4; i++) {
    delay(200);
    matrix.fillScreen(matrix.Color333(5, 5, 5)); // Fill screen white
    delay(200);
    matrix.fillScreen(matrix.Color333(0, 0, 0)); // Clear screen
    }
    clearEEPROM(); // Clear EEPROM
    resetEnable = LOW; 
    restartArduino();
}
}