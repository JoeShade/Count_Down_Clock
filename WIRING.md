# ESP32 Wiring Guide (DOIT DevKit V1)

Updated: B2 on GPIO5, DOWN button on GPIO33.

## RGB LED matrix (HUB75)
- R1/G1/B1/R2/G2/B2: GPIO **16 / 17 / 25 / 26 / 27 / 5**
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
- Navigation buttons (active-low to GND; external pull-ups needed on 34/35/32):
  - UP: GPIO **2** (internal pull-up)
  - DOWN: GPIO **33** (internal pull-up)
  - LEFT: GPIO **34** (external pull-up)
  - RIGHT: GPIO **35** (external pull-up)
  - CENTRE/OK: GPIO **32** (external pull-up)

## Notes
- All matrix data pins are output-capable.
- External pull-ups are required on GPIO34/35/32/36/39 since they lack internal pull-ups.
- Do not power the panel from the ESP32’s 5 V pin; use a supply sized for the panel and tie grounds together.
