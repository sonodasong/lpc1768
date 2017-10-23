#ifndef __UTIL_H__
#define __UTIL_H__

#include <driver.h>

void strClr(char *str, uint8 size);
uint8 strCpy(char *src, char *dst, uint8 size);
boolean strCmp(char *str1, char *str2, uint8 size);
void ledInit(uint8 num);
void ledOn(uint8 num);
void ledOff(uint8 num);

#endif
