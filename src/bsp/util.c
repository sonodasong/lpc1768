#include "util.h"

void ledInit(uint8 num)
{
	pinSel(2, num, 0);
	pinMode(2, num, 0);
	pinModeOD(2, num, 0);
	LPC_GPIO2->FIOMASK &= rex(num);
	LPC_GPIO2->FIOCLR = ex(num);
	LPC_GPIO2->FIODIR |= ex(num);
}

void ledOn(uint8 num)
{
	LPC_GPIO2->FIOSET = ex(num);
}

void ledOff(uint8 num)
{
	LPC_GPIO2->FIOCLR = ex(num);
}
