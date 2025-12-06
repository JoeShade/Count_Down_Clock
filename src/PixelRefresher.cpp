#include "PixelRefresher.h"
#include "arduino.h"
#include "RGBmatrixPanel.h"

int MATRIX_HEIGHT = 64;
int MATRIX_WIDTH = 64;
extern RGBmatrixPanel matrix;

// Create function for pixel exerciser 
void runPixelRefresher() { // Declare pixel refresher function
    for (int y = 0; y < MATRIX_HEIGHT; y++) {  // Loop through Y coords
      for (int x = 0; x < MATRIX_WIDTH; x++) { // Loop through x coords

        byte randRed = random(1, 7); // Generate random red value
        byte randGreen = random(1, 7); // Generate random green value
        byte randBlue = random(1, 7); // Generate random blue value
        matrix.drawPixel(x, y, matrix.Color333(randRed, randGreen, randBlue)); // Render pixel
      }
    } 

  delay(500); //Hold full screen for a moment
    for (int y = MATRIX_HEIGHT - 1; y >= 0; y--) {  // Loop through Y coords
        for (int x = MATRIX_WIDTH - 1; x >= 0; x--) { // Loop through x coords
        matrix.drawPixel(x, y, matrix.Color333(0, 0, 0)); // Clear Pixel
    }
  }
}