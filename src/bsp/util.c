#include "util.h"

void strClr(char *str, uint8 size)
{
	while (size != 0) {
		str[size - 1] = '\0';
		size--;
	}
}

uint8 strCpy(char *src, char *dst, uint8 size)
{
	uint8 i;

	i = 0;
	while (src[i] != '\0' && i < size) {
		dst[i] = src[i];
		i++;
	}
	return i;
}

boolean strCmp(char *str1, char *str2, uint8 size)
{
	uint8 i;

	for (i = 0; i < size; i++) {
		if (str1[i] != str2[i]) {
			return FALSE;
		}
	}
	return TRUE;
}

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
