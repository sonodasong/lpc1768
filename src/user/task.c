#include "task.h"

void blink(void *pdata)
{
	(void)pdata;
	ledInit(0);
	while (1) {
		ledOn(0);
		OSTimeDly(25);
		ledOff(0);
		OSTimeDly(25);
	}
}

void serial(void *pdata)
{
	char *str;

	(void)pdata;
	while (1) {
		uart0Read(&str);
		uart0Printf("%s\r\n", str);
	}
}

#define LCD_HEIGHT_PER_TRANSFER		8

static uint32 buffer[LCD_WIDTH * LCD_HEIGHT_PER_TRANSFER];

static void displayBmp(void);

void lcdCmosTask(void *pdata)
{
	FATFS FatFs;
	char *str;

	(void)pdata;
	lcdInitFromTask();
	ov7670InitFromTask(lcdCmosHandler);
	f_mount(&FatFs, "", 0);
	while (1) {
		lcdCmosEnable();
		ov7670Enable();
		uart0Read(&str); uart0Printf("%s\r\n", str);
		ov7670Disable();
		lcdCmosDisable();

		lcdDisplaySingleColor(0xFFE0);
		uart0Read(&str); uart0Printf("%s\r\n", str);

		displayBmp();
		uart0Read(&str); uart0Printf("%s\r\n", str);
	}
}

void fatfsTimerTask(void *pdata)
{
	extern void disk_timerproc(void);

	(void)pdata;
	while (1) {
		disk_timerproc();
		OSTimeDly(1);
	}
}

static void bmp24To32(void)
{
	uint8 red, green, blue;
	uint32 i24, i32;

	i32 = LCD_WIDTH * LCD_HEIGHT_PER_TRANSFER;
	for (i24 = 3 * LCD_WIDTH * LCD_HEIGHT_PER_TRANSFER; i24 > 0;) {
		red = ((uint8 *)buffer)[--i24];
		green = ((uint8 *)buffer)[--i24];
		blue = ((uint8 *)buffer)[--i24];
		buffer[--i32] = ((blue << 12) & 0x00030000) | ((red << 8) & 0xFC00) | ((green << 2) & 0x03F0) | (blue >> 4);
	}
}

static void displayBmp(void)
{
	FIL fil;
	UINT rc;
	uint16 i;

	f_open(&fil, "demo.bmp", FA_READ);
	f_lseek(&fil, 54);
	for (i = 0; i < LCD_HEIGHT; i += LCD_HEIGHT_PER_TRANSFER) {
		f_read(&fil, buffer, 3 * LCD_WIDTH * LCD_HEIGHT_PER_TRANSFER, &rc);
		bmp24To32();
		lcdDisplayBmp(buffer, i, LCD_HEIGHT_PER_TRANSFER);
	}
	f_close(&fil);
}
