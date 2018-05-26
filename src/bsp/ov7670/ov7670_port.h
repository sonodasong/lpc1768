#ifndef __LCD_PORT_H__
#define __LCD_PORT_H__

#include <driver.h>

void ov7670IRQInit(void (*handlerPtr)(void));
void ov7670IRQEnable(void);
void ov7670IRQDisable(void);
void ov7670IRQClear(void);
void ov7670ExternalClockInit(void);
void ov7670ExternalClockEnable(void);
void ov7670RegW(uint8 reg, uint8 data);

#endif
