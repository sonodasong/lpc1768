#include "lcd_port.h"
#include "lcd.h"

#define MS_10		(10 * OS_TICKS_PER_SEC / 1000)

static uint16 lcdRegR(uint16 reg);
static void lcdRegW(uint16 reg, uint16 data);
static void lcdSetRamAddr(uint16 x, uint16 y);

void lcdInitFromTask(void)
{
	lcdPinInit();
	uart0Printf("LCD device code: %#06x\r\n", lcdRegR(0x0000));
	lcdRegW(0x0000,0x0001); OSTimeDly(MS_10);
	lcdRegW(0x0003,0xA8A4); OSTimeDly(MS_10);
	lcdRegW(0x000C,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x000D,0x080C); OSTimeDly(MS_10);
	lcdRegW(0x000E,0x2B00); OSTimeDly(MS_10);
	lcdRegW(0x001E,0x00B0); OSTimeDly(MS_10);
	lcdRegW(0x0001,0x2B3F); OSTimeDly(MS_10);
	lcdRegW(0x0002,0x0600); OSTimeDly(MS_10);
	lcdRegW(0x0010,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0011,0x6070); OSTimeDly(MS_10);
	lcdRegW(0x0005,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0006,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0016,0xEF1C); OSTimeDly(MS_10);
	lcdRegW(0x0017,0x0003); OSTimeDly(MS_10);
	lcdRegW(0x0007,0x0133); OSTimeDly(MS_10);
	lcdRegW(0x000B,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x000F,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0041,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0042,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0048,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0049,0x013F); OSTimeDly(MS_10);
	lcdRegW(0x004A,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x004B,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0044,0xEF00); OSTimeDly(MS_10);
	lcdRegW(0x0045,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0046,0x013F); OSTimeDly(MS_10);
	lcdRegW(0x0030,0x0707); OSTimeDly(MS_10);
	lcdRegW(0x0031,0x0204); OSTimeDly(MS_10);
	lcdRegW(0x0032,0x0204); OSTimeDly(MS_10);
	lcdRegW(0x0033,0x0502); OSTimeDly(MS_10);
	lcdRegW(0x0034,0x0507); OSTimeDly(MS_10);
	lcdRegW(0x0035,0x0204); OSTimeDly(MS_10);
	lcdRegW(0x0036,0x0204); OSTimeDly(MS_10);
	lcdRegW(0x0037,0x0502); OSTimeDly(MS_10);
	lcdRegW(0x003A,0x0302); OSTimeDly(MS_10);
	lcdRegW(0x003B,0x0302); OSTimeDly(MS_10);
	lcdRegW(0x0023,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0024,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x0025,0x8000); OSTimeDly(MS_10);
	lcdRegW(0x004F,0);
	lcdRegW(0x004E,0);
}

void lcdGramRead(uint16 *buf, uint16 x, uint16 y, uint32 size)
{
	uint32 i;

	lcdSetRamAddr(x, y);
	lcdIndexW(0x22);
	lcdPortIn();
	lcdDataR();
	for (i = 0; i < size; i++) {
		buf[i] = lcdDataR();
	}
	lcdPortOut();
}

void lcdDisplaySingleColor(uint16 color)
{
	uint32 i;

	lcdSetRamAddr(0, 0);
	lcdIndexW(0x22);
	for(i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
		lcdDataW(color);
	}
}

void lcdDisplayBmp(uint16 *buf, uint16 y, uint16 size)
{
	uint32 i;

	lcdSetRamAddr(0, y);
	lcdIndexW(0x22);
	for (i = 0; i < LCD_WIDTH * size; i++) {
		lcdDataW(buf[i]);
	}
}

static uint16 lcdRegR(uint16 reg)
{
	uint16 data;

	lcdIndexW(reg);
	lcdPortIn();
	data = lcdDataR();
	lcdPortOut();
	return data;
}

static void lcdRegW(uint16 reg, uint16 data)
{
	lcdIndexW(reg);
	lcdDataW(data);
}

static void lcdSetRamAddr(uint16 x, uint16 y)
{
	lcdRegW(0x4E, x);
	lcdRegW(0x4F, y);
}
