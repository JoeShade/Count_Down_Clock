#include "SetUpMode.h"
#include <Arduino.h>
#include "EEPROMfunctions.h"
#include "Restart.h"
#include "RTClib.h"  // Load RTC library
#include "Flags.h"
#include <EEPROM.h>
#include "julianDay.h"
#include "RGBmatrixPanel.h"  // Load RGB Matrix library
#include <Fonts/Picopixel.h>
#include "wrappedtext.h"

extern RTC_DS1307 rtc;
extern RGBmatrixPanel matrix;

extern bool centreButtonState;
extern bool centreLastButtonState;
extern bool upButtonState;
extern bool upLastButtonState;
extern bool downButtonState;
extern bool downLastButtonState;
extern bool leftButtonState;
extern bool leftLastButtonState;
extern bool rightButtonState;
extern bool rightLastButtonState;

#define UP_PIN 2
#define DOWN_PIN 5
#define LEFT_PIN 21
#define RIGHT_PIN 22
#define CENTRE_PIN 25

// Declare date variables

int day;
int month;
int year;
int field = 0;
int pixelField;
int sleepField;
int scrollField;
int sleepTimeField = 0;
int adjustTimeField = 0;
int adjustDateField = 0;
int adjustHour = 0;
int adjustMinute = 0;
int adjustDay = 0;
int adjustMonth = 0;
int adjustYear = 0;
bool firstTimeInLoop = true;
bool firstTimeInDateLoop = true;

extern uint32_t birthDateJulian;
extern byte wakeHour;
extern byte sleepHour;

void updateDisplay(int day, int month, int year) {
  // Define the area where the date is displayed
  int textX = 2;
  int textY = 24;
  int textWidth = 64;   // Adjust based on your text size
  int textHeight = 12;  // Adjust based on font height

  matrix.setCursor(textX, textY);

  // Clear only the area where the date is displayed
  matrix.fillRect(textX, textY - 5, textWidth, textHeight, matrix.Color333(0, 0, 0));

  // Print new date
  char dateStr[11];  // Buffer to hold the formatted date
  sprintf(dateStr, "%02d/%02d/%04d", day, month, year);
  matrix.print(dateStr);

  // Determine underline position
  int underlineX = 2;      // Start position for day
  int underlineWidth = 6;  // Default width for day and month
  if (field == 1) {
    underlineX += 11;  // Move to month
  } else if (field == 2) {
    underlineX += 22;     // Move to year
    underlineWidth += 7;  // Make line longer
  }

  // Draw underline
  matrix.drawLine(underlineX, 26, underlineX + underlineWidth, 26, matrix.Color333(255, 255, 255));  // Adjust width as needed
}

int daysInMonth(int month, int year) {
  if (month == 2) {
    // Leap year check
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) ? 29 : 28;
  }
  if (month == 4 || month == 6 || month == 9 || month == 11) {
    return 30;
  }
  return 31;
}

void validateDate(int &day, int &month, int &year) {
  if (month < 1) month = 1;
  if (month > 12) month = 12;

  int maxDay = daysInMonth(month, year);
  if (day < 1) day = 1;
  if (day > maxDay) day = maxDay;
}

void setUpMode() {

  if (CHECK_FLAG(flags, FLAG_PIXEL_REFRESHER)) {
    pixelField = 0;
  } else {
    pixelField = 1;
  }

  if (CHECK_FLAG(flags, FLAG_SLEEP_MODE)) {
    sleepField = 0;
  } else {
    sleepField = 1;
  }

  if (CHECK_FLAG(flags, FLAG_SCROLL_EFFECT)) {
    scrollField = 0;
  } else {
    scrollField = 1;
  }

  Serial.print(birthDateJulian);
  if (birthDateJulian > 2436505) {  // Ensure valid range
    gregorianDate(birthDateJulian, &year, &month, &day);
  } else {
    day = 15;
    month = 6;
    year = 1986;
  }
  rtc.begin();
  DateTime now = rtc.now();

  matrix.setFont(&Picopixel);
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  Serial.println("Entered Set up mode");
  printWrappedText(matrix, 2, 6, 64, "ENTERED SETUP MODE");

  delay(2000);
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  Serial.print("Enter Date of Birth: DD/MM/YYYY:");  // Prompt user for input

  CLEAR_FLAG(flags, FLAG_DEBUG_MODE);  // Turn off debug mode by default

  int i = 0;                            // Declare counter variable
  bool displayNeedsUpdate = true;       // Flag to track if an update is needed
  bool pixelDisplayNeedsUpdate = true;  // Flag to track if an update is needed
  bool sleepDisplayNeedsUpdate = true;  // Flag to track if an update is needed
  bool scrollDisplayNeedsUpdate = true;
  bool sleepTimeDisplayNeedsUpdate = true;
  bool adjustTimeDisplayNeedsUpdate = true;
  bool adjustDateDisplayNeedsUpdate = true;


  while (i <= 7) {

    centreButtonState = digitalRead(CENTRE_PIN);

    if (centreButtonState == LOW && centreLastButtonState == HIGH) {
      i++;
      centreLastButtonState = centreButtonState;
    }

    while (i == 0) {

      if (displayNeedsUpdate) {
        matrix.setCursor(2, 6);
        printWrappedText(matrix, 2, 6, 64, "ENTER DATE OF BIRTH:");
        updateDisplay(day, month, year);
        displayNeedsUpdate = false;  // Reset flag
        delay(100);
      }

      // Read button inputs
      centreButtonState = digitalRead(CENTRE_PIN);
      leftButtonState = digitalRead(LEFT_PIN);
      rightButtonState = digitalRead(RIGHT_PIN);
      upButtonState = digitalRead(UP_PIN);
      downButtonState = digitalRead(DOWN_PIN);

      // Adjust fields based on button presses
      if (leftButtonState == LOW && leftLastButtonState == HIGH) {
        field = (field == 0) ? 2 : field - 1;
        displayNeedsUpdate = true;  // Set flag to refresh display
      }
      if (rightButtonState == LOW && rightLastButtonState == HIGH) {
        field = (field == 2) ? 0 : field + 1;
        displayNeedsUpdate = true;
      }
      if (upButtonState == LOW && upLastButtonState == HIGH) {
        if (field == 0) { day = (day >= 31) ? 1 : day + 1; }
        if (field == 1) { month = (month >= 12) ? 1 : month + 1; }
        if (field == 2) { year++; }
        displayNeedsUpdate = true;
      }
      if (downButtonState == LOW && downLastButtonState == HIGH) {
        if (field == 0) { day = (day <= 1) ? 31 : day - 1; }
        if (field == 1) { month = (month <= 1) ? 12 : month - 1; }
        if (field == 2) { year--; }
        displayNeedsUpdate = true;
      }

      if (centreButtonState == LOW && centreLastButtonState == HIGH) {
        i++;
        centreLastButtonState = centreButtonState;
        break;
      }
      validateDate(day, month, year);
      DateTime date(year, month, day, 0, 0, 0);
      uint32_t dobJulian = julianDay(year, month, day);
      writeToEEPROM(0, dobJulian);
      centreLastButtonState = centreButtonState;
    }

    matrix.fillScreen(matrix.Color333(0, 0, 0));

    while (i == 1) {
      // Read buttons
      centreButtonState = digitalRead(CENTRE_PIN);
      leftButtonState = digitalRead(LEFT_PIN);
      rightButtonState = digitalRead(RIGHT_PIN);

      if (pixelDisplayNeedsUpdate) {
        matrix.drawLine(0, 14, 64, 14, matrix.Color333(0, 0, 0));
        printWrappedText(matrix, 2, 6, 64, "PIXEL REFRESHER ENABLED? Y/N");
        if (pixelField == 0) {
          matrix.drawLine(37, 14, 39, 14, matrix.Color333(7, 7, 7));
        } else {
          matrix.drawLine(45, 14, 48, 14, matrix.Color333(7, 7, 7));
        }
        pixelDisplayNeedsUpdate = false;
      }

      if (pixelField == 0) {
        SET_FLAG(flags, FLAG_PIXEL_REFRESHER);  // Enable FLAG_PIXEL_REFRESHER
      }
      if (pixelField == 1) {
        CLEAR_FLAG(flags, FLAG_PIXEL_REFRESHER);  // Disable FLAG_PIXEL_REFESHER
      }

      if (leftButtonState == LOW && leftLastButtonState == HIGH) {
        Serial.println("Left button pressed");
        if (pixelField == 0) pixelField = 1;
        else pixelField -= 1;
        pixelDisplayNeedsUpdate = true;
        Serial.println(pixelField);
      }

      if (rightButtonState == LOW && rightLastButtonState == HIGH) {
        Serial.println("Right button pressed");
        if (pixelField == 1) pixelField = 0;
        else pixelField += 1;
        pixelDisplayNeedsUpdate = true;
        Serial.println(pixelField);
      }

      if (centreButtonState == LOW && centreLastButtonState == HIGH) {
        i++;
        centreLastButtonState = centreButtonState;
        break;
      }
      centreLastButtonState = centreButtonState;
      leftLastButtonState = leftButtonState;
      rightLastButtonState = rightButtonState;
    }

    EEPROM.write(10, flags);
    matrix.fillScreen(matrix.Color333(0, 0, 0));

    while (i == 2) {

      // Read buttons
      centreButtonState = digitalRead(CENTRE_PIN);
      leftButtonState = digitalRead(LEFT_PIN);
      rightButtonState = digitalRead(RIGHT_PIN);

      if (scrollDisplayNeedsUpdate) {
        matrix.drawLine(0, 14, 64, 14, matrix.Color333(0, 0, 0));
        printWrappedText(matrix, 2, 6, 64, "SCROLL EFFECT ENABLED? Y/N");
        if (scrollField == 0) {
          matrix.drawLine(37, 14, 39, 14, matrix.Color333(7, 7, 7));
        } else {
          matrix.drawLine(45, 14, 48, 14, matrix.Color333(7, 7, 7));
        }
        scrollDisplayNeedsUpdate = false;
      }

      if (scrollField == 0) {
        SET_FLAG(flags, FLAG_SCROLL_EFFECT);  // Enable FLAG_SCROLL_EFFECT
      }
      if (scrollField == 1) {
        CLEAR_FLAG(flags, FLAG_SCROLL_EFFECT);  // Disable FLAG_SCROLL_EFFECT
      }

      if (centreButtonState == LOW && centreLastButtonState == HIGH) {
        i++;
        centreLastButtonState = centreButtonState;
        break;
      }

      if (leftButtonState == LOW && leftLastButtonState == HIGH) {
        if (scrollField == 0) scrollField = 1;
        else scrollField -= 1;
        scrollDisplayNeedsUpdate = true;
      }

      if (rightButtonState == LOW && rightLastButtonState == HIGH) {
        if (scrollField == 1) scrollField = 0;
        else scrollField += 1;
        scrollDisplayNeedsUpdate = true;
      }

      centreLastButtonState = centreButtonState;
      leftLastButtonState = leftButtonState;
      rightLastButtonState = rightButtonState;
    }

    matrix.fillScreen(matrix.Color333(0, 0, 0));
    while (i == 3) {

      // Read buttons
      centreButtonState = digitalRead(CENTRE_PIN);
      leftButtonState = digitalRead(LEFT_PIN);
      rightButtonState = digitalRead(RIGHT_PIN);

      if (sleepDisplayNeedsUpdate) {
        matrix.drawLine(0, 14, 64, 14, matrix.Color333(0, 0, 0));
        printWrappedText(matrix, 2, 6, 64, "SLEEP MODE ENABLED? Y/N");
        if (sleepField == 0) {
          matrix.drawLine(37, 14, 39, 14, matrix.Color333(7, 7, 7));
        } else {
          matrix.drawLine(45, 14, 48, 14, matrix.Color333(7, 7, 7));
        }
        sleepDisplayNeedsUpdate = false;
      }

      if (sleepField == 0) {
        SET_FLAG(flags, FLAG_SLEEP_MODE);  // Enable FLAG_SLEEP_MODE
      }
      if (sleepField == 1) {
        CLEAR_FLAG(flags, FLAG_SLEEP_MODE);  // Disable FLAG_SLEEP_MODE
      }

      if (centreButtonState == LOW && centreLastButtonState == HIGH) {
        i++;
        break;
      }

      if (leftButtonState == LOW && leftLastButtonState == HIGH) {
        if (sleepField == 0) sleepField = 1;
        else sleepField -= 1;
        sleepDisplayNeedsUpdate = true;
      }

      if (rightButtonState == LOW && rightLastButtonState == HIGH) {
        if (sleepField == 1) sleepField = 0;
        else sleepField += 1;
        sleepDisplayNeedsUpdate = true;
      }
      centreLastButtonState = centreButtonState;
      leftLastButtonState = leftButtonState;
      rightLastButtonState = rightButtonState;
    }

    centreLastButtonState = centreButtonState;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    while (i == 4) {

      // Read buttons
      centreButtonState = digitalRead(CENTRE_PIN);
      leftButtonState = digitalRead(LEFT_PIN);
      rightButtonState = digitalRead(RIGHT_PIN);
      upButtonState = digitalRead(UP_PIN);
      downButtonState = digitalRead(DOWN_PIN);

      if (sleepTimeDisplayNeedsUpdate) {

        matrix.fillRect(42, 0, 64, 64, matrix.Color333(0, 0, 0));

        if (sleepTimeField == 0) {
          matrix.drawLine(44, 8, 50, 8, matrix.Color333(7, 7, 7));
        } else {
          matrix.drawLine(42, 20, 48, 20, matrix.Color333(7, 7, 7));
        }

        matrix.setCursor(2, 6);

        char sleepTimeStr[6];  // Buffer for formatted time (e.g., "06")
        char wakeTimeStr[6];   // Buffer for formatted time (e.g., "08")

        sprintf(sleepTimeStr, "%02d", sleepHour);  // Format with leading zero
        sprintf(wakeTimeStr, "%02d", wakeHour);    // Format with leading zero
        printWrappedText(matrix, 2, 6, 64, ("SLEEP TIME: " + String(sleepTimeStr)).c_str());
        printWrappedText(matrix, 2, 18, 64, ("WAKE TIME: " + String(wakeTimeStr)).c_str());
        sleepTimeDisplayNeedsUpdate = false;  // Reset flag
      }

      // Adjust fields based on button presses
      if (leftButtonState == LOW && leftLastButtonState == HIGH) {
        sleepTimeField = (sleepTimeField == 0) ? 1 : 0;
        sleepTimeDisplayNeedsUpdate = true;  // Set flag to refresh display
        Serial.print("Left button pressed, sleepTimeField: ");
        Serial.println(sleepTimeField);
      }
      if (rightButtonState == LOW && rightLastButtonState == HIGH) {
        sleepTimeField = (sleepTimeField == 1) ? 0 : 1;
        sleepTimeDisplayNeedsUpdate = true;
        Serial.print("Right button pressed, sleepTimeField: ");
        Serial.println(sleepTimeField);
      }
      if (upButtonState == LOW && upLastButtonState == HIGH) {
        if (sleepTimeField == 0) { sleepHour = (sleepHour >= 23) ? 0 : sleepHour + 1; }
        if (sleepTimeField == 1) { wakeHour = (wakeHour >= 23) ? 0 : wakeHour + 1; }
        sleepTimeDisplayNeedsUpdate = true;
      }
      if (downButtonState == LOW && downLastButtonState == HIGH) {
        if (sleepTimeField == 0) { sleepHour = (sleepHour <= 0) ? 23 : sleepHour - 1; }
        if (sleepTimeField == 1) { wakeHour = (wakeHour <= 0) ? 23 : wakeHour - 1; }
        sleepTimeDisplayNeedsUpdate = true;
      }

      if (centreButtonState == LOW && centreLastButtonState == HIGH) {
        i++;
        centreLastButtonState = centreButtonState;
        break;
      }
      centreLastButtonState = centreButtonState;
      leftLastButtonState = leftButtonState;
      rightLastButtonState = rightButtonState;
      upLastButtonState = upButtonState;
      downLastButtonState = downButtonState;
    }

    centreLastButtonState = centreButtonState;
    matrix.fillScreen(matrix.Color333(0, 0, 0));  // Blank Screen

    while (i == 5) {

      // Read buttons
      centreButtonState = digitalRead(CENTRE_PIN);
      leftButtonState = digitalRead(LEFT_PIN);
      rightButtonState = digitalRead(RIGHT_PIN);
      upButtonState = digitalRead(UP_PIN);
      downButtonState = digitalRead(DOWN_PIN);


      if (firstTimeInLoop) {
        DateTime now = rtc.now();  // Get the current time once
        adjustHour = now.hour();
        adjustMinute = now.minute();
        firstTimeInLoop = false;
        adjustTimeDisplayNeedsUpdate = true;
      }

      if (adjustTimeDisplayNeedsUpdate) {
        matrix.fillScreen(matrix.Color333(0, 0, 0));  // Blank Screen
        char timeStr[11];
        sprintf(timeStr, "%02d:%02d", adjustHour, adjustMinute);  // Use the adjusted time

        printWrappedText(matrix, 2, 6, 64, "ADJUST CURRENT TIME:");
        printWrappedText(matrix, 2, 20, 64, timeStr);

        if (adjustTimeField == 0) {
          matrix.drawLine(2, 22, 7, 22, matrix.Color333(7, 7, 7));
        } else {
          matrix.drawLine(12, 22, 17, 22, matrix.Color333(7, 7, 7));
        }

        adjustTimeDisplayNeedsUpdate = false;
      }

      // Button handling logic
      if (leftButtonState == LOW && leftLastButtonState == HIGH) {
        adjustTimeField = (adjustTimeField == 0) ? 1 : 0;
        adjustTimeDisplayNeedsUpdate = true;
      }

      if (rightButtonState == LOW && rightLastButtonState == HIGH) {
        adjustTimeField = (adjustTimeField == 1) ? 0 : 1;
        adjustTimeDisplayNeedsUpdate = true;
      }

      if (upButtonState == LOW && upLastButtonState == HIGH) {
        if (adjustTimeField == 0) {
          adjustHour = (adjustHour + 1) % 24;
        } else {
          adjustMinute = (adjustMinute + 1) % 60;
        }
        adjustTimeDisplayNeedsUpdate = true;
      }

      if (downButtonState == LOW && downLastButtonState == HIGH) {
        if (adjustTimeField == 0) {
          adjustHour = (adjustHour == 0) ? 23 : adjustHour - 1;
        } else {
          adjustMinute = (adjustMinute == 0) ? 59 : adjustMinute - 1;
        }
        adjustTimeDisplayNeedsUpdate = true;
      }

      // When user confirms (center button)
      if (centreButtonState == LOW && centreLastButtonState == HIGH) {
        DateTime now = rtc.now();
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), adjustHour, adjustMinute, now.second()));
        i++;
        break;
      }

      // Save button states
      centreLastButtonState = centreButtonState;
      leftLastButtonState = leftButtonState;
      rightLastButtonState = rightButtonState;
      upLastButtonState = upButtonState;
      downLastButtonState = downButtonState;
    }

    centreLastButtonState = centreButtonState;
    matrix.fillScreen(matrix.Color333(0, 0, 0));

    while (i == 6) {
      // Read buttons
      centreButtonState = digitalRead(CENTRE_PIN);
      leftButtonState = digitalRead(LEFT_PIN);
      rightButtonState = digitalRead(RIGHT_PIN);
      upButtonState = digitalRead(UP_PIN);
      downButtonState = digitalRead(DOWN_PIN);

      // Initialize the date adjustments once
      if (firstTimeInDateLoop) {
        DateTime now = rtc.now();  // Get the current time once
        adjustDay = now.day();
        adjustMonth = now.month();
        adjustYear = now.year();
        firstTimeInDateLoop = false;
        adjustDateDisplayNeedsUpdate = true;
      }

      // Display the current date and adjustment fields
      if (adjustDateDisplayNeedsUpdate) {
        matrix.fillScreen(matrix.Color333(0, 0, 0));  // Blank Screen
        char dateStr[11];
        sprintf(dateStr, "%02d/%02d/%04d", adjustDay, adjustMonth, adjustYear);  // Use the adjusted date

        printWrappedText(matrix, 2, 6, 64, "ADJUST CURRENT DATE:");
        printWrappedText(matrix, 2, 20, 64, dateStr);

        // Draw a line to indicate the currently selected field
        if (adjustDateField == 0) {  // Day selected
          matrix.drawLine(2, 22, 7, 22, matrix.Color333(7, 7, 7));
        } else if (adjustDateField == 1) {  // Month selected
          matrix.drawLine(14, 22, 19, 22, matrix.Color333(7, 7, 7));
        } else {  // Year selected
          matrix.drawLine(26, 22, 39, 22, matrix.Color333(7, 7, 7));
        }

        adjustDateDisplayNeedsUpdate = false;
      }

      // Button handling logic
      if (leftButtonState == LOW && leftLastButtonState == HIGH) {
        adjustDateField = (adjustDateField == 0) ? 2 : adjustDateField - 1;  // Cycle through Day -> Month -> Year
        adjustDateDisplayNeedsUpdate = true;
      }

      if (rightButtonState == LOW && rightLastButtonState == HIGH) {
        adjustDateField = (adjustDateField == 2) ? 0 : adjustDateField + 1;  // Cycle through Year -> Day -> Month
        adjustDateDisplayNeedsUpdate = true;
      }

      // Increase the selected date field
      if (upButtonState == LOW && upLastButtonState == HIGH) {
        if (adjustDateField == 0) {              // Adjust Day
          adjustDay = (adjustDay % 31) + 1;      // Wrap around after 31
        } else if (adjustDateField == 1) {       // Adjust Month
          adjustMonth = (adjustMonth % 12) + 1;  // Wrap around after December
        } else if (adjustDateField == 2) {       // Adjust Year
          adjustYear++;                          // Increment year
        }
        adjustDateDisplayNeedsUpdate = true;
      }

      // Decrease the selected date field
      if (downButtonState == LOW && downLastButtonState == HIGH) {
        if (adjustDateField == 0) {                                 // Adjust Day
          adjustDay = (adjustDay == 1) ? 31 : adjustDay - 1;        // Wrap around to 31
        } else if (adjustDateField == 1) {                          // Adjust Month
          adjustMonth = (adjustMonth == 1) ? 12 : adjustMonth - 1;  // Wrap around to December
        } else if (adjustDateField == 2) {                          // Adjust Year
          adjustYear--;                                             // Decrement year
        }
        adjustDateDisplayNeedsUpdate = true;
      }

      // When user confirms (center button)
      if (centreButtonState == LOW && centreLastButtonState == HIGH) {
        DateTime now = rtc.now();
        rtc.adjust(DateTime(adjustYear, adjustMonth, adjustDay, now.hour(), now.minute(), now.second()));
        i++;
        break;
      }

      // Save button states for debounce logic
      centreLastButtonState = centreButtonState;
      leftLastButtonState = leftButtonState;
      rightLastButtonState = rightButtonState;
      upLastButtonState = upButtonState;
      downLastButtonState = downButtonState;
    }
  }
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  CLEAR_FLAG(flags, FLAG_SETUP_MODE);  // Clear setup flag
  EEPROM.write(11, sleepHour);
  EEPROM.write(12, wakeHour);
  EEPROM.write(10, flags);  // Update stored flags
  EEPROM.commit();
  delay(2500);
  Serial.flush();
  restartArduino();
}
