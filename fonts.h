#ifndef __CN_H
#define __CN_H
#if defined(ARDUINO_ARCH_ESP32)
#include <pgmspace.h>
#else
#include "avr/pgmspace.h"
#endif

#define MAX_HEIGHT_FONT         64
#define MAX_WIDTH_FONT          64


typedef struct                  // Chinese font data structure 汉字字模数据结构
{
       unsigned char index[3];               // Chinese character inner code index 汉字内码索引
       const unsigned char matrix[MAX_HEIGHT_FONT*MAX_WIDTH_FONT/8];      // Dot matrix code data 点阵码数据
}CH_CN;

typedef struct
{    
  const CH_CN *table;
  uint16_t size;
  uint16_t Width;
  
}cFONT;

extern cFONT Font16CN;
extern cFONT Font32CN;
extern cFONT Font64CN;
#endif
