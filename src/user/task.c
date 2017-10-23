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

static uint8 buffer[3 * LCD_WIDTH * LCD_HEIGHT_PER_TRANSFER];

static void bmp24To16(void);
static void printBuf(uint8 *buf);

void lcdDisplayBmpTask(void *pdata)
{
	FATFS FatFs;
	FIL fil;
	UINT rc;
	char *str;
	uint16 i;

	(void)pdata;
	lcdInitFromTask();
	f_mount(&FatFs, "", 0);
	while (1) {
		uart0Read(&str);
		uart0Printf("%s\r\n", str);
		lcdDisplaySingleColor(0xFFE0);
		f_open(&fil, "demo.bmp", FA_READ);
		f_lseek(&fil, 54);
		for (i = 0; i < LCD_HEIGHT; i += LCD_HEIGHT_PER_TRANSFER) {
			f_read(&fil, buffer, 3 * LCD_WIDTH * LCD_HEIGHT_PER_TRANSFER, &rc);
			bmp24To16();
			lcdDisplayBmp((uint16 *)buffer, i, LCD_HEIGHT_PER_TRANSFER);
		}
		f_close(&fil);
	}
}

void lcdGramReadTask(void *pdata)
{
	FATFS FatFs;
	FIL fil;
	UINT rc;
	char *str;

	(void)pdata;
	lcdInitFromTask();
	f_mount(&FatFs, "", 0);
	while (1) {
		uart0Read(&str);
		uart0Printf("%s\r\n", str);
		lcdDisplaySingleColor(0xFFE0);
		f_open(&fil, "demo.bmp", FA_READ);
		f_lseek(&fil, 54);
		f_read(&fil, buffer, 3 * LCD_WIDTH * LCD_HEIGHT_PER_TRANSFER, &rc);
		f_close(&fil);
		bmp24To16();
		lcdDisplayBmp((uint16 *)buffer, 0, LCD_HEIGHT_PER_TRANSFER);

		uart0Printf("buffer data:\r\n");
		printBuf(buffer);
		strClr((char *)(buffer + 128), 128);
		lcdGramRead((uint16 *)(buffer + 128), 0, 0, 64);
		uart0Printf("GRAM data:\r\n");
		printBuf(buffer + 128);
		if (strCmp((char *)buffer, (char *)(buffer + 128), 128)) {
			uart0Printf("GRAM read success\r\n");
		} else {
			uart0Printf("GRAM read fail\r\n");
		}
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

static void bmp24To16(void)
{
	uint8 red, green, blue;
	uint32 i24, i16;

	i16 = 0;
	for (i24 = 0; i24 < 3 * LCD_WIDTH * LCD_HEIGHT_PER_TRANSFER;) {
		blue = buffer[i24++];
		green = buffer[i24++];
		red = buffer[i24++];
		buffer[i16++] = ((green << 3) & 0xE0) | (blue >> 3);
		buffer[i16++] = (red & 0xF8) | (green >> 5);
	}
}

static void printBuf(uint8 *buf)
{
	uint8 i;
	uint8 j;

	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			uart0Printf("%04x ", ((uint16 *)buf)[j * 8 + i]);
		}
		uart0Printf("\r\n");
	}
	uart0Printf("\r\n");
}
