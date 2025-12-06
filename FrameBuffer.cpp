#include "FrameBuffer.h"
#include "RGBmatrixPanel.h" // Load RGB Matrix library
#include "Arduino.h"

constexpr int MATRIX_WIDTH =  64; // Set Matrix width
constexpr int MATRIX_HEIGHT = 64; // Set Matrix height
extern RGBmatrixPanel matrix;
extern int Red;
extern int Green;
extern int Blue;

byte framebuffer[(MATRIX_WIDTH * MATRIX_HEIGHT) / 8];  // Declare Array to store pixel values

// Create function compress to framebuffer
void setPixel(int x, int y, bool state) {
  int index = (y * MATRIX_WIDTH + x) / 8; // Find byte storing pixel 
  int bitPos = (y * MATRIX_WIDTH + x) % 8; // Find bit pos
  bool oldState = framebuffer[index] & (1 << bitPos);

  if(state != oldState) { // Only update if changed
    if (state) {
      framebuffer[index] |= (1 << bitPos); // Set bit (ON)
      } else {
       framebuffer[index] &= ~(1 << bitPos); // Clear bit (OFF)
      }
  } 
}

// Create function to decompress from framebuffer
void getPixel(int x, int y) {
  int index = x / 2;
  return (x % 2 == 0) ? (framebuffer[index] >> 4) : (framebuffer[index] & 0x0F);
}

// Create function for rendering frame buffer 
void renderFramebuffer() { // Declare render framebuffer function
  for (int y = 0; y < MATRIX_HEIGHT; y++) { // Loop through Y coords
    for (int x = 0; x < MATRIX_WIDTH; x++) { // Loop through X coords
      int index = (y * MATRIX_WIDTH + x) / 8;
      int bitPos = (y * MATRIX_WIDTH + x) % 8; 
      bool pixelOn = (framebuffer[index] & (1 << bitPos)) !=0;
        matrix.drawPixel(x, y, pixelOn ? matrix.Color333(Red, Green, Blue) :matrix.Color333(0, 0, 0)); // Use same colour for all pixels
    }
  }
}

// Create function to clear framebuffer
void clearFramebuffer() {
  memset(framebuffer, 0, sizeof(framebuffer)); // Clear frame buffer 
}