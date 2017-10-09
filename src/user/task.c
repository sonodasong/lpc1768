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
