#ifndef __LCD_H__
#define __LCD_H__

#include <driver.h>

#define LCD_WIDTH		240
#define LCD_HEIGHT		320

void lcdInitFromTask(void);
void lcdDisplaySingleColor(uint32 color);
void lcdDisplayBmp(uint32 *buf, uint16 y, uint16 size);
void lcdCmosEnable(void);
void lcdCmosDisable(void);
void lcdCmosHandler(void);

#endif
