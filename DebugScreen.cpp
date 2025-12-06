#include "DebugScreen.h"
#include "wrappedtext.h"
#include "RGBmatrixPanel.h"
#include "RTClib.h"  // Load RTC library
#include <Fonts/Picopixel.h>

extern RGBmatrixPanel matrix;
extern RTC_DS1307 rtc;

void debugScreen() {

  matrix.setFont(&Picopixel);
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  DateTime now = rtc.now();

  int day = now.day();
  int month = now.month();
  int year = now.year();

  int hour = now.hour();
  int minute = now.minute();

  char dateStr[11];  // Buffer to hold the formatted date
  sprintf(dateStr, "%02d/%02d/%04d", day, month, year);

  char timeStr[11];  // Buffer to hold the time
  sprintf(timeStr, "%02d:%02d", hour, minute);

  printWrappedText(matrix, 2, 6, 64, "VERSION 1.0");
  printWrappedText(matrix, 2, 18, 64, ("CURRENT DATE: " + String(dateStr)).c_str());
  printWrappedText(matrix, 2, 36, 64, ("CURRENT TIME: " + String(timeStr)).c_str());
  delay(5000);
}