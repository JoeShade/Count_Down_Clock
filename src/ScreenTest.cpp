#include "ScreenTest.h"
#include "arduino.h"
#include "RGBmatrixPanel.h"

extern RGBmatrixPanel matrix;

// Create function for power on screen test
void runScreenTest() { // Declare screen test function 
  const byte delayTime = 400; // Declare variable for delay
  const byte TestColours[][3] = { //Create array for RGB values
    {5, 0, 0}, // Red
    {0, 5, 0}, // Green
    {0, 0, 5}, // Blue
    {5, 5, 5}, // White
    {0, 0, 0}  // Black (clear screen)
  };
  for (int i = 0; i < 5; i++) { // Loop 4 times
    matrix.fillScreen(matrix.Color333(TestColours[i][0], TestColours[i][1], TestColours[i][2])); // Render LED matrix
    delay(delayTime); // Wait before changing colour
  }
}