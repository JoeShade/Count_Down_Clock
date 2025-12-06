#include "julianDay.h"

// Create function to convert Gregorian date to Julian Day
long julianDay(int year, int month, int day) {
  if (month <= 2) {
          year -= 1;
          month += 12;
      }
      int A = year / 100;
      long B = (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))  
             ? (2 - A + (A / 4))  // Apply Gregorian correction
             : 0;  // No correction for Julian dates
      long jd = (long)(365.25 * (year + 4716)) + (int)(30.6001 * (month + 1)) + day + B - 1524;
  return jd;
}

// Function to convert Julian Day to Gregorian date
void gregorianDate(long jd, int *year, int *month, int *day) {
    long a, b, c, d, e, m;
    
    long z = jd;
    long alpha = 0;
    
    // Check if the Julian Day is after the switch to the Gregorian calendar
    if (jd >= 2299161) {  
        alpha = (long)((jd - 1867216.25) / 36524.25);
        z = jd + 1 + alpha - (alpha / 4);
    }
    
    b = z + 1524;
    c = (long)((b - 122.1) / 365.25);
    d = (long)(365.25 * c);
    e = (long)((b - d) / 30.6001);
    
    *day = b - d - (long)(30.6001 * e);
    *month = (e < 14) ? e - 1 : e - 13;
    *year = (*month > 2) ? c - 4716 : c - 4715;
}