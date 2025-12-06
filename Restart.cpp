#include "Restart.h"
#include "Arduino.h"
#include "RGBmatrixPanel.h"
#include "EEPROMfunctions.h"

#include <esp_task_wdt.h>
#include <esp_system.h>

extern RGBmatrixPanel matrix;
volatile byte resetEnable = LOW;
volatile uint8_t timeoutCounter;


// Create restart function
void restartArduino() {
  ESP.restart();
}

// Create WDT reset function
void watchdogReset() {
  esp_task_wdt_reset();
}

// Create WDT start function
void watchdogStart() {
  esp_task_wdt_init(8, true);
  esp_task_wdt_add(NULL);
  while (true) {
    // Wait for watchdog to trigger reset
  }
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