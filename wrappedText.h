#ifndef WRAPPEDTEXT_H
#define WRAPPEDTEXT_H

#include "RGBmatrixPanel.h" 

extern RGBmatrixPanel matrix;

void printWrappedText(RGBmatrixPanel &matrix, int x, int y, int maxWidth, const char *text);

#endif