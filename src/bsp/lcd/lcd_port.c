#include "lcd_port.h"

void lcdPinInit(void)
{
	/* CS */
	pinSel(0, 22, 0);
	pinMode(0, 22, 0);
	pinModeOD(0, 22, 0);
	LPC_GPIO0->FIOMASK &= rex(22);
	LCD_CS_HIGH();
	LPC_GPIO0->FIODIR |= ex(22);

	/* RS */
	pinSel(0, 23, 0);
	pinMode(0, 23, 0);
	pinModeOD(0, 23, 0);
	LPC_GPIO0->FIOMASK &= rex(23);
	LCD_RS_LOW();
	LPC_GPIO0->FIODIR |= ex(23);

	/* RD */
	pinSel(0, 25, 0);
	pinMode(0, 25, 0);
	pinModeOD(0, 25, 0);
	LPC_GPIO0->FIOMASK &= rex(25);
	LCD_RD_HIGH();
	LPC_GPIO0->FIODIR |= ex(25);

	/* WR */
	pinSel(0, 24, 0);
	pinMode(0, 24, 0);
	pinModeOD(0, 24, 0);
	LPC_GPIO0->FIOMASK &= rex(24);
	LCD_WR_HIGH();
	LPC_GPIO0->FIODIR |= ex(24);

	/* DIR */
	pinSel(0, 21, 0);
	pinMode(0, 21, 0);
	pinModeOD(0, 21, 0);
	LPC_GPIO0->FIOMASK &= rex(21);
	LCD_DIR_HIGH();
	LPC_GPIO0->FIODIR |= ex(21);

	/* LE */
	pinSel(0, 20, 0);
	pinMode(0, 20, 0);
	pinModeOD(0, 20, 0);
	LPC_GPIO0->FIOMASK &= rex(20);
	LCD_LE_LOW();
	LPC_GPIO0->FIODIR |= ex(20);

	/* EN */
	pinSel(0, 19, 0);
	pinMode(0, 19, 0);
	pinModeOD(0, 19, 0);
	LPC_GPIO0->FIOMASK &= rex(19);
	LCD_EN_LOW();
	LPC_GPIO0->FIODIR |= ex(19);

	/* Data */
	LPC_PINCON->PINSEL4 &= 0xFFFF0000;
	LPC_PINCON->PINMODE4 &= 0xFFFF0000;
	LPC_PINCON->PINMODE_OD2 &= 0xFFFFFF00;
	LPC_GPIO2->FIOMASK &= 0xFFFFFF00;
	LPC_GPIO2->FIODIR |= 0xFF;
}
