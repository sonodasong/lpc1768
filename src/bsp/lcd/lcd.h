#ifndef __LCD_H__
#define __LCD_H__

#include <driver.h>

#define LCD_WIDTH		240
#define LCD_HEIGHT		320

void lcdInitFromTask(void);
void lcdGramRead(uint16 *buf, uint16 x, uint16 y, uint32 size);
void lcdDisplaySingleColor(uint16 color);
void lcdDisplayBmp(uint16 *buf, uint16 y, uint16 size);

#endif
