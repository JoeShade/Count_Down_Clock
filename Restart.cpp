#include "Restart.h"
#include "Arduino.h"
#include "RGBmatrixPanel.h"
#include "EEPROMfunctions.h"

#if defined(ARDUINO_ARCH_ESP32)
#include <esp_task_wdt.h>
#include <esp_system.h>
#else
#include <avr/wdt.h> // Load watch dog timer library
#endif

extern RGBmatrixPanel matrix;
volatile byte resetEnable = LOW;
volatile uint8_t timeoutCounter
#if !defined(ARDUINO_ARCH_ESP32)
__attribute__ ((section (".noinit")))
#endif
;


// Create restart function
void restartArduino() {
#if defined(ARDUINO_ARCH_ESP32)
  ESP.restart();
#else
  wdt_enable(WDTO_15MS);  // Enable watchdog timer with a 15ms timeout
  while (1);  // Wait for watchdog reset
#endif
}

// Create WDT reset function
void watchdogReset() {
#if defined(ARDUINO_ARCH_ESP32)
  esp_task_wdt_reset();
#else
wdt_reset();
#endif
}

// Create WDT start function
void watchdogStart() {
#if defined(ARDUINO_ARCH_ESP32)
  esp_task_wdt_init(8, true);
  esp_task_wdt_add(NULL);
  while (true) {
    // Wait for watchdog to trigger reset
  }
#else
  wdt_enable(WDTO_8S); // Enable watchdog timer with 8s timeout
  while (1); // Wait fror watchdog reset
#endif
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