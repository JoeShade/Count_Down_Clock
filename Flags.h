#ifndef FLAGS_H
#define FLAGS_H
#include <Arduino.h>

// Flags
extern uint8_t flags; 
#define FLAG_SLEEP_MODE 0x01
#define FLAG_DEBUG_MODE 0x02
#define FLAG_SETUP_MODE 0x04  
#define FLAG_PIXEL_REFRESHER 0x08 
#define FLAG_SCROLL_EFFECT 0x10

// Flag Macros
#define SET_FLAG(var, flag) (var |= (flag)) // Set flag
#define CLEAR_FLAG(var, flag) (var &= ~(flag)) // Clear flag
#define CHECK_FLAG(var, flag) (var & (flag)) // Check flag

#endif