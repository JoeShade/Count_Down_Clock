#include "wrappedtext.h"
#include "RGBmatrixPanel.h" 

extern RGBmatrixPanel matrix;

void printWrappedText(RGBmatrixPanel &matrix, int x, int y, int maxWidth, const char *text) {
  int cursorX = x;
  int cursorY = y;
  int charWidth = 4;   // Approximate width per character
  int charHeight = 6;  // Line height

  matrix.setCursor(cursorX, cursorY);

  while (*text) {
    // Skip leading spaces
    while (*text == ' ') {
      text++;
    }

    const char *wordStart = text;
    int wordPixelWidth = 0;

    // Find the end of the current word
    while (*text && *text != ' ') {
      wordPixelWidth += charWidth;
      text++;
    }

    // If the word doesn't fit on the current line, move to the next line
    if (cursorX + wordPixelWidth > maxWidth) {
      cursorX = x;            // Reset X position
      cursorY += charHeight;  // Move to the next line
      matrix.setCursor(cursorX, cursorY);
    }

    // Print the word
    while (wordStart < text) {
      matrix.print(*wordStart++);
      cursorX += charWidth;
    }

    // Print space if needed
    if (*text == ' ') {
      matrix.print(' ');
      cursorX += charWidth;
      text++;
    }
  }
}
