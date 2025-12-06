#ifndef RESTART_H
#define RESTART_H
#include "Arduino.h"

void restartArduino();
void reset();
void handleReset();
void watchdogReset();
void watchdogStart();

extern volatile byte resetEnable;
#endif