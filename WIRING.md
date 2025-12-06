# ESP32 Wiring Guide

The sketch is configured for an ESP32 driving a HUB75 64×64 RGB panel and a DS1307 RTC. All buttons use `INPUT_PULLUP`, so they read **LOW** when pressed (shorted to GND). The display colour is fixed to white; no RGB potentiometers are required.

## RGB LED matrix (HUB75)
- **CLK** → GPIO **18**
- **OE** → GPIO **19**
- **LAT** → GPIO **23**
- **Address A/B/C/D/E** → GPIO **12 / 13 / 14 / 15 / 27**
- **Power** → External **5 V** supply; tie matrix **GND** to ESP32 **GND**

## RTC (DS1307)
- **SDA** → GPIO **21**
- **SCL** → GPIO **22**
- **Power** → **3.3 V** and **GND** (plus backup coin cell on the module)

## Controls and debug/reset
- **Reset (clear EEPROM)** → GPIO **4** (momentary to **GND**)
- **Debug enable** → GPIO **5** (short to **GND** to enable)
- **Navigation buttons** (active-low to **GND**)
  - UP: GPIO **25**
  - DOWN: GPIO **33**
  - LEFT: GPIO **32**
  - RIGHT: GPIO **34**
  - CENTRE/OK: GPIO **35**

## Grounding and power tips
- Share **GND** between the ESP32, matrix, RTC, and buttons.
- Do **not** power the matrix from the ESP32’s 5 V pin; use a supply sized for the panel’s current draw.
