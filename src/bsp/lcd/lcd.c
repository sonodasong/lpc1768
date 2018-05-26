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
	lcdRegW(0x00,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x07,0x0001); OSTimeDly(MS_10);
	lcdRegW(0x08,0x0405); OSTimeDly(MS_10);
	lcdRegW(0x30,0x0707); OSTimeDly(MS_10);
	lcdRegW(0x31,0x0407); OSTimeDly(MS_10);
	lcdRegW(0x32,0x0203); OSTimeDly(MS_10);
	lcdRegW(0x33,0x0303); OSTimeDly(MS_10);
	lcdRegW(0x34,0x0303); OSTimeDly(MS_10);
	lcdRegW(0x35,0x0202); OSTimeDly(MS_10);
	lcdRegW(0x36,0x1007); OSTimeDly(MS_10);
	lcdRegW(0x37,0x0707); OSTimeDly(MS_10);
	lcdRegW(0x38,0x0407); OSTimeDly(MS_10);
	lcdRegW(0x39,0x0203); OSTimeDly(MS_10);
	lcdRegW(0x3A,0x0202); OSTimeDly(MS_10);
	lcdRegW(0x3B,0x0303); OSTimeDly(MS_10);
	lcdRegW(0x3C,0x0606); OSTimeDly(MS_10);
	lcdRegW(0x3D,0x1313); OSTimeDly(MS_10);
	lcdRegW(0x07,0x0101); OSTimeDly(MS_10);
	lcdRegW(0x17,0x0001); OSTimeDly(MS_10);
	lcdRegW(0x10,0x08B0); OSTimeDly(MS_10);
	lcdRegW(0x11,0x0001); OSTimeDly(MS_10);
	lcdRegW(0x12,0x0118); OSTimeDly(MS_10);
	lcdRegW(0x13,0x0B00); OSTimeDly(MS_10);
	lcdRegW(0x29,0x0004); OSTimeDly(MS_10);
	lcdRegW(0x2A,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x12,0x01B8); OSTimeDly(MS_10);
	lcdRegW(0x01,0x0100); OSTimeDly(MS_10);
	lcdRegW(0x02,0x0300); OSTimeDly(MS_10);
	/* 18-byte RAM data in 2 transfer */
	lcdRegW(0x03,0x8030); OSTimeDly(MS_10);
	lcdRegW(0x04,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x09,0x0028); OSTimeDly(MS_10);
	lcdRegW(0x0C,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x50,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x51,0x00EF); OSTimeDly(MS_10);
	lcdRegW(0x52,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x53,0x013F); OSTimeDly(MS_10);
	lcdRegW(0x60,0x2700); OSTimeDly(MS_10);
	lcdRegW(0x61,0x0001); OSTimeDly(MS_10);
	lcdRegW(0x90,0x0015); OSTimeDly(MS_10);
	lcdRegW(0x92,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x93,0x0000); OSTimeDly(MS_10);
	lcdRegW(0x20,0x00EF); OSTimeDly(MS_10);
	lcdRegW(0x21,0x013F); OSTimeDly(MS_10);
	lcdRegW(0x07,0x0021); OSTimeDly(MS_10);
	lcdRegW(0x07,0x0061); OSTimeDly(MS_10);
	lcdRegW(0x07,0x0173); OSTimeDly(MS_10);
	lcdRegW(0x10,0x16B0); OSTimeDly(MS_10);
	lcdRegW(0x20,0x0000);
	lcdRegW(0x21,0x0000);
}

void lcdDisplaySingleColor(uint32 color)
{
	uint32 i;

	lcdSetRamAddr(0, 0);
	LCD_CS_LOW();
	lcdPortOut();
	lcdIndexW(0x22);
	for(i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
		lcdDataW(color);
		lcdDataW(color >> 16);
	}
	lcdPortIn();
	LCD_CS_HIGH();
}

void lcdDisplayBmp(uint32 *buf, uint16 y, uint16 size)
{
	uint32 i;
	uint32 color;

	lcdSetRamAddr(0, y);
	LCD_CS_LOW();
	lcdPortOut();
	lcdIndexW(0x22);
	for (i = 0; i < LCD_WIDTH * size; i++) {
		color = buf[i];
		lcdDataW(color);
		lcdDataW(color >> 16);
	}
	lcdPortIn();
	LCD_CS_HIGH();
}

void lcdCmosEnable(void)
{
	lcdRegW(0x03,0x8038); OSTimeDly(MS_10);
}

void lcdCmosDisable(void)
{
	LCD_WR_OUT();
	LCD_CS_HIGH();
	lcdRegW(0x03,0x8030); OSTimeDly(MS_10);
}

void lcdCmosHandler(void)
{
	LCD_WR_OUT();
	LCD_CS_HIGH();
	lcdSetRamAddr(0, 0);
	LCD_CS_LOW();
	lcdPortOut();
	lcdIndexW(0x22);
	lcdPortIn();
	LCD_WR_IN();
}

static uint16 lcdRegR(uint16 reg)
{
	uint16 data;

	LCD_CS_LOW();
	lcdPortOut();
	lcdIndexW(reg);
	lcdPortIn();
	data = lcdDataR();
	LCD_CS_HIGH();
	return data;
}

static void lcdRegW(uint16 reg, uint16 data)
{
	LCD_CS_LOW();
	lcdPortOut();
	lcdIndexW(reg);
	lcdDataW(data);
	lcdPortIn();
	LCD_CS_HIGH();
}

static void lcdSetRamAddr(uint16 x, uint16 y)
{
	lcdRegW(0x20, x);
	lcdRegW(0x21, y);
}
