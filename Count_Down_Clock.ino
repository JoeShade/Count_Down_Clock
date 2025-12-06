// Load required libraries
#include "RTClib.h"          // Load RTC library
#include "RGBmatrixPanel.h"  // Load RGB Matrix library
#include <EEPROM.h>          // Load EEPROM library
#include <pgmspace.h>    // Load PROGMEM library for ESP32

// Load program files
#include "Logo.h"             // Load Logo
#include "FrameBuffer.h"      // Load frame buffer
#include "PixelRefresher.h"   // Load Pixel Refresher
#include "EEPROMfunctions.h"  // Load EEPROM functions
#include "Restart.h"          // Load Restart functions
#include "ScreenTest.h"       // Load Screen test
#include "Flags.h"            // Load flags
#include "SetUpMode.h"        // Load Set up mode
#include "julianDay.h"        // Load julian day function
#include "DebugScreen.h"      // Load debug screen

RTC_DS1307 rtc;  // Create RTC object for use later

// Set RGB Matrix dimensions
constexpr int MATRIX_WIDTH = 64;   // Set Matrix width
constexpr int MATRIX_HEIGHT = 64;  // Set Matrix height

// Set pins for RGB Matrix (ESP32)
#define CLK 18  // Set Clock pin
#define OE 19   // Set Output enable pin
#define LAT 23  // Set Latch pin
#define A 12    // Set A address pin
#define B 13    // Set B address pin
#define C 14    // Set C address pin
#define D 15    // Set D address pin
#define E 27    // Set E address pin

// Set pins for Debug and Reset
#define RESET_PIN 21  // Clears EEPROM
#define DEBUG_PIN 22  // Enables Debug mode

// Set pins for RGB potentiometers (ESP32 ADC1 pins)
#define RED_POT_PIN 34
#define GREEN_POT_PIN 35
#define BLUE_POT_PIN 32

// Set pins for navigation button
#define UP_PIN 16
#define DOWN_PIN 17
#define LEFT_PIN 25
#define RIGHT_PIN 26
#define CENTRE_PIN 33

RGBmatrixPanel matrix(A, B, C, D, E, CLK, LAT, OE, false, MATRIX_WIDTH);  // Create matrix object for use later

// Declare global variables and constants
uint32_t birthDateJulian;               // Input birth date as Julian day, long int required to avoid integer overflow
constexpr int BLINK_TIME_ON = 500;      // Set blink time on time
constexpr int BLINK_TIME_OFF = 1500;    // Set blink time off time
bool isPixelExerciseScheduled = false;  // Declare exerciser timing variable


// Declare variables for set up mode
char pixelRefresherEnabled;
char sleepModeEnabled;
char scrollEffectEnabled;
extern byte framebuffer[];

// Declare timing variables for exerciser function
byte randHour, randMinute, randHour2, randMinute2;  // Store as byte to save memory

// Declare colour variables
byte Red, Green, Blue;  // Store as byte to save memory


// Declare Wake/Sleep time variables
byte sleepHour;  // Declare Sleep hour variable
byte wakeHour;   // Declare Wake Time variable

int readColourValue(uint8_t pin) {
  constexpr int ADC_MAX = 4095;
  return constrain(map(analogRead(pin), 0, ADC_MAX, 0, 7), 0, 7);
}

// Create function for adjusting brightness for blink effect
int adjustBrightness(int colour) {
  return (colour >= 3) ? (colour - 2) : (colour >= 2) ? (colour - 1)
                                                      : colour;
}

// Declare constants for Scrolling animation
constexpr uint32_t ANIMATION_INTERVAL = 600000;  // 10 minutes in milliseconds
constexpr int FADE_STEPS = 10;                   // Number of fade steps
constexpr int FADE_DELAY = 100;                  // Delay between fade steps (ms)

unsigned long lastAnimationTime = 0;  // Tracks last animation run time

// Declare variables for navigation button
bool upButtonState = LOW;
bool upLastButtonState = HIGH;

bool downButtonState = LOW;
bool downLastButtonState = HIGH;

bool leftButtonState = LOW;
bool leftLastButtonState = HIGH;

bool rightButtonState = LOW;
bool rightLastButtonState = HIGH;

bool centreButtonState = LOW;
bool centreLastButtonState = HIGH;
unsigned long centreButtonPressTime = 0;
bool centreButtonHeld = false;
const unsigned long centreHoldTime = 2000;

void setup() {  // Put your setup code here, to run once:

  // Configure I2C for the DS1307 RTC
  Wire.begin(I2C_SDA, I2C_SCL);

  // Set pin modes
  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(DEBUG_PIN, INPUT_PULLUP);

  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(CENTRE_PIN, INPUT_PULLUP);

  Serial.begin(9600);  // Start serial port and set Baudrate
  matrix.begin();      // Initialise LED matrix

  attachInterrupt(digitalPinToInterrupt(RESET_PIN), reset, FALLING);

  // Load flags from EEPROM
  flags = EEPROM.read(10);

  // Debug mode
  // Enable if debug pin is shorted to gnd
  if (digitalRead(DEBUG_PIN) == LOW) {
    SET_FLAG(flags, FLAG_DEBUG_MODE);
  } else {
    CLEAR_FLAG(flags, FLAG_DEBUG_MODE);
  }


  // Uncomment to explicitly enable set up mode mode
  //SET_FLAG(flags, FLAG_SETUP_MODE);

  if (CHECK_FLAG(flags, FLAG_DEBUG_MODE)) {
    Serial.print("Loaded Flags:");
    Serial.println(flags, BIN);
  }

  birthDateJulian = readFromEEPROM(0);  // Load birthdate from EEPROM use readFromEEPROM function as its 4 byte
  sleepHour = EEPROM.read(11);          // Load Sleep time from EEPROM
  wakeHour = EEPROM.read(12);           // Load Wake time from EEPROM

  if (sleepHour >= 24 || sleepHour < 0) {
    sleepHour = 22;
  }

  if (wakeHour >= 24 || wakeHour < 0) {
    wakeHour = 6; 
  }

  if (birthDateJulian  < 2436505 || birthDateJulian > 2816787) {
    birthDateJulian = 2446596;
  }

  if (CHECK_FLAG(flags, FLAG_DEBUG_MODE)) {
    Serial.print("Loaded from EEPROM:");
    Serial.print(birthDateJulian);
    Serial.print(", Sleep hour: ,");
    Serial.println(sleepHour);
    Serial.print(", Wake hour: ,");
    Serial.println(wakeHour);
  }

  if (!rtc.begin()) {                                                 // Check if RTC can be found
    Serial.println("Couldn't find RTC. Restarting in 5 seconds...");  // Print line if RTC is not present
    Serial.flush();                                                   // Make sure message is displayed before proceeding
    delay(5000);                                                      //  Wait 5 seconds
    restartArduino();
  }

  if (!rtc.isrunning()) {                                              // Check if RTC is running
    Serial.println("RTC is NOT running! Restarting in 5 seconds...");  // Print line if RTC is present but not running
    Serial.flush();                                                    // Make sure message is displayed before proceeding
    // Flash matrix red 3 times
    for (int i = 0; i < 3; i++) {
      matrix.fillScreen(matrix.Color333(5, 0, 0));
      delay(250);
      matrix.fillScreen(matrix.Color333(0, 0, 0));
      delay(50);
    }
    delay(4100);  // Wait 4.1 seconds
    restartArduino();
  }

  if (CHECK_FLAG(flags, FLAG_DEBUG_MODE)) {
    debugScreen();  // Show debug screen
  }

  // Only run screen test on full reset, first boot or debug mode
  if (flags == 0xFF || CHECK_FLAG(flags, FLAG_DEBUG_MODE)) {
    runScreenTest();
  }

  displayLogo();  // Show logo

  if (CHECK_FLAG(flags, FLAG_PIXEL_REFRESHER)) {
    runPixelRefresher();  // Run pixel exerciser
  }

  // If Set up mode hasn't been run or is enabled enter set up mode
  if ((flags == 0xFF) || CHECK_FLAG(flags, FLAG_SETUP_MODE)) {
    setUpMode();
  }
}

void loop() {  // Put your main code here, to run repeatedly:

  centreButtonState = digitalRead(CENTRE_PIN);

  // Check if centre button has been pressed
  if (centreButtonState == LOW && centreLastButtonState == HIGH) {
    centreButtonPressTime = millis();
    centreButtonHeld = false;
  }

  // Check to see if it was held for 2 seconds, enter set up mode
  if (centreButtonState == LOW && (millis() - centreButtonPressTime >= centreHoldTime) && !centreButtonHeld) {
    centreButtonHeld = true;
    setUpMode();
  }

  // Button released
  if (centreButtonState == HIGH) {
    centreButtonHeld = false;
  }

  centreLastButtonState = centreButtonState;  // Update last state

  handleReset();  // Reset if button pressed

  // Check to see if debug mode should be on
  if (digitalRead(DEBUG_PIN) == LOW) {
    flags |= FLAG_DEBUG_MODE;
  } else {
    flags &= ~FLAG_DEBUG_MODE;
  }

  DateTime now = rtc.now();  // Declare now variable
  unsigned long nowTime = julianDay(now.year(), now.month(), now.day());
  unsigned long daysPassed = (nowTime >= birthDateJulian) ? (nowTime - birthDateJulian) : 0;  // Work out how much time has passed, set to 0 if nowTime < Birthdate to prevent underflow
  unsigned long weeksPassed = daysPassed / 7;                                                 // Work out how many weeks have passed

  // Declare X/Y pixel pos. Variables
  byte pixelX = 0;                                                                                             // Declare X Pos.
  byte pixelY = 0;                                                                                             // Declare Y Pos.
  bool ledOff = CHECK_FLAG(flags, FLAG_SLEEP_MODE) && ((now.hour() >= sleepHour) || (now.hour() < wakeHour));  // Declare ledOff variable, check if sleepmode is enabled and if time is between sleep and wake time

  // Declare RGB pot variables
  // Read values from pin
  int redValue = readColourValue(RED_POT_PIN);
  int greenValue = readColourValue(GREEN_POT_PIN);
  int blueValue = readColourValue(BLUE_POT_PIN);

  if (CHECK_FLAG(flags, FLAG_DEBUG_MODE)) {
    Serial.print("RGB values: ");
    Serial.print(redValue);
    Serial.print(", ");
    Serial.print(greenValue);
    Serial.print(", ");
    Serial.print(blueValue);
  }

  // Set Matrix colour based on whether LED is Off
  Red = ledOff ? 0 : 7;      // Set Red value
  Green = ledOff ? 0 : 7;    // Set Green value
  Blue = ledOff ? 0 : 7;     // Set Blue value

  clearFramebuffer();

  for (int i = 0; i <= weeksPassed; i++) {
    int x = i % MATRIX_WIDTH;
    int y = i / MATRIX_WIDTH;
    setPixel(x, y, true);

    int index = (pixelY * MATRIX_WIDTH + pixelX) / 8;
    int bitPos = (pixelY * MATRIX_WIDTH + pixelX) % 8;
    framebuffer[index] |= (1 << bitPos);  // Set bit

    if (CHECK_FLAG(flags, FLAG_DEBUG_MODE)) {    // Print if debug mode is enabled
      Serial.println("Counter: " + String(i));  // Print current counter value
    }
  }

  renderFramebuffer();

  // Blink Effect

  // Declare timing variables
  static unsigned long lastBlinkTime = 0;
  static bool blinkOn = false;

  // Set Blink colour
  // Dims colour by subtracting 1 or 2 from RGB values
  byte redBlink = adjustBrightness(Red);
  byte greenBlink = adjustBrightness(Green);
  byte blueBlink = adjustBrightness(Blue);

  // Calculate Blink Position

  byte blinkPixelX = (weeksPassed + 1) % MATRIX_WIDTH;
  byte blinkPixelY = (weeksPassed + 1) / MATRIX_WIDTH;

  // Blink timing logic
  if (millis() - lastBlinkTime >= (blinkOn ? BLINK_TIME_ON : BLINK_TIME_OFF)) {
    lastBlinkTime = millis();
    blinkOn = !blinkOn;  // Toggle state
    matrix.drawPixel(blinkPixelX, blinkPixelY,
                     blinkOn ? matrix.Color333(redBlink, greenBlink, blueBlink) : matrix.Color333(0, 0, 0));  // Blink ON / OFF
  }

  // pixelRefresh timing
  // if enabled runs pixelExerciser at 2 random times during the day
  if (CHECK_FLAG(flags, FLAG_PIXEL_REFRESHER) && (!isPixelExerciseScheduled)) {

    // Generate 2 random times for the day
    randHour = random(wakeHour, sleepHour);
    randMinute = random(0, 60);

    randHour2 = random(wakeHour, sleepHour);
    randMinute2 = random(0, 60);

    isPixelExerciseScheduled = !isPixelExerciseScheduled;  // Toggle state
  }

  // Generate new times at 1 minute past midnight
  if ((now.hour() == 0) && (now.minute() == 1)) {
    isPixelExerciseScheduled = false;
  }

  if (((randHour == now.hour()) && (randMinute == now.minute()) || (randHour2 == now.hour()) && (randMinute2 == now.minute())) && CHECK_FLAG(flags, FLAG_PIXEL_REFRESHER)) {
    runPixelRefresher();
  }

  // Scroll effect
  unsigned long currentMillis = millis();  // Get current time in ms for scroll timing

  if (flags & FLAG_DEBUG_MODE) {  // Print if debug mode is enabled
    Serial.println(currentMillis - lastAnimationTime);
  }

  if (currentMillis - lastAnimationTime >= ANIMATION_INTERVAL) {
    matrix.fillScreen(matrix.Color333(0, 0, 0));  // Clear screen
    // Fill matrix to current week
    for (int i = 0; i < weeksPassed; i++) {
      int x = i % MATRIX_WIDTH;
      int y = i / MATRIX_WIDTH;
      if (y < MATRIX_HEIGHT) {
        matrix.drawPixel(x, y, matrix.Color333(Red, Green, Blue));
        delay(16);  // Slow down animation
      }
      lastAnimationTime = millis();  // Update animation time
    }

    delay(500);  // Hold the screen

    // Work out last rendered pixel
    int lastX = weeksPassed % MATRIX_WIDTH;
    int lastY = weeksPassed / MATRIX_WIDTH;

    // Fade out the pixels
    for (int fade = min(Red, min(Green, Blue)); fade >= 0; fade--) {
      for (int y = lastY; y >= 0; y--) {                                      // Start from last lit row and move up
        for (int x = (y == lastY ? lastX : MATRIX_WIDTH - 1); x >= 0; x--) {  // Fade only up to lastX on last row
          matrix.drawPixel(x, y, matrix.Color333(fade, fade, fade));
          delay(8);  // Slow down animation
        }
      }
    }
    delay(500);                                   // Hold the screen
    matrix.fillScreen(matrix.Color333(0, 0, 0));  // Clear screen
  }

  // Debug Prints
  if (flags & FLAG_DEBUG_MODE) {  // Print if debug mode is enabled

    Serial.println();  // Print blank line for formatting

    Serial.print(F("Birth date Julian: "));
    Serial.println(birthDateJulian);

    Serial.print(F("Current Unix time: "));
    Serial.println(now.unixtime());

    int convYear;
    int convMonth;
    int convDay;

    Serial.print("Gregorian Date: ");

    gregorianDate(birthDateJulian, &convYear, &convMonth, &convDay);
    char dateStr[12];  // Buffer to hold the formatted date
    sprintf(dateStr, "%02d/%02d/%04d", convDay, convMonth, convYear);
    Serial.println(dateStr);  // Print formatted date over Serial

    Serial.print(F("Current date: "));
    Serial.print(now.day());
    Serial.print(F("/"));
    Serial.print(now.month());
    Serial.print(F("/"));
    Serial.println(now.year());

    Serial.print(F("Weeks Passed: "));
    Serial.println(weeksPassed);

    Serial.print(F("Current Hour: "));
    Serial.println(now.hour());

    Serial.print(F("ledOff: "));
    Serial.println(ledOff);

    Serial.print(F("X: "));
    Serial.print(weeksPassed % MATRIX_WIDTH);
    Serial.print(F(" Y: "));
    Serial.println(weeksPassed / MATRIX_WIDTH);

    Serial.println();  // Print blank line for formatting

    delay(5000);  // Slow down program so debug info can be read
  
  }
}