# ESP32 Wiring Guide (DOIT DevKit V1)

Pin map adjusted to avoid GPIO6/7. Buttons use INPUT_PULLUP where supported; input-only pins need external pull-ups to 3.3 V.

## RGB LED matrix (HUB75)
- R1/G1/B1/R2/G2/B2: GPIO **16 / 17 / 25 / 26 / 27 / 32**
- CLK: GPIO **18**
- OE: GPIO **19**
- LAT: GPIO **23**
- Address A/B/C/D/E: GPIO **12 / 13 / 14 / 15 / 4**
- Power: External **5 V**; share **GND** with the ESP32

## RTC (DS1307)
- SDA: GPIO **21**
- SCL: GPIO **22**
- Power: **3.3 V** and **GND** (plus coin cell)

## Controls and debug/reset
- Reset (clear EEPROM): GPIO **36** (external pull-up to 3.3 V; momentary to GND)
- Debug enable: GPIO **39** (external pull-up to 3.3 V; short to GND)
- Navigation buttons (active-low to GND; internal pull-ups on 2/5/21/22; add pull-up on 25 if needed):
  - UP: GPIO **2**
  - DOWN: GPIO **5**
  - LEFT: GPIO **21**
  - RIGHT: GPIO **22**
  - CENTRE/OK: GPIO **25**

## Notes
- If any of your wired pins differ from this map, tell me the exact wiring and I’ll remap the code.
- Do not power the panel from the ESP32’s 5 V pin; use a supply sized for the panel and tie grounds together.
