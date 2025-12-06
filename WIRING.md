# ESP32 Wiring Guide

Configured for an ESP32 driving a 64x64 HUB75 RGB panel and a DS1307 RTC. Buttons are `INPUT_PULLUP` (read LOW when pressed). Default pin map assumes the Adafruit RGBmatrixPanel defaults for data lines.

## RGB LED matrix (HUB75)
- R1/G1/B1/R2/G2/B2: GPIO 2 / 3 / 4 / 5 / 6 / 7
- CLK: GPIO 18
- OE: GPIO 19
- LAT: GPIO 23
- Address A/B/C/D/E: GPIO 12 / 13 / 14 / 15 / 27
- Power: External 5 V; share GND with the ESP32

## RTC (DS1307)
- SDA: GPIO 21
- SCL: GPIO 22
- Power: 3.3 V and GND (plus coin cell)

## Controls and debug/reset
- Reset (clear EEPROM): GPIO 16 (momentary to GND)
- Debug enable: GPIO 17 (short to GND)
- Navigation (active-low to GND): UP 25, DOWN 33, LEFT 32, RIGHT 34, CENTRE/OK 35

## Notes
- If your panel wiring uses different ESP32 pins for R1/G1/B1/R2/G2/B2, we need to remap them in `RGBmatrixPanel` to match.
- Do not power the panel from the ESP32’s 5 V pin; use a supply sized for the panel and tie grounds together.
