#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

void setPixel(int x, int y, bool state);
bool getPixel(int x, int y);
void renderFramebuffer();
void clearFramebuffer(); 

#endif
