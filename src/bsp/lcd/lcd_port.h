#ifndef __LCD_PORT_H__
#define __LCD_PORT_H__

#include <driver.h>

#define LCD_DIR_LOW()		LPC_GPIO0->FIOCLR = ex(21)
#define LCD_DIR_HIGH()		LPC_GPIO0->FIOSET = ex(21)
#define LCD_LE_LOW()		LPC_GPIO0->FIOCLR = ex(20)
#define LCD_LE_HIGH()		LPC_GPIO0->FIOSET = ex(20)
#define LCD_EN_LOW()		LPC_GPIO0->FIOCLR = ex(19)
#define LCD_EN_HIGH()		LPC_GPIO0->FIOSET = ex(19)
#define LCD_CS_LOW()		LPC_GPIO0->FIOCLR = ex(22)
#define LCD_CS_HIGH()		LPC_GPIO0->FIOSET = ex(22)
#define LCD_RS_LOW()		LPC_GPIO0->FIOCLR = ex(23)
#define LCD_RS_HIGH()		LPC_GPIO0->FIOSET = ex(23)
#define LCD_RD_LOW()		LPC_GPIO0->FIOCLR = ex(25)
#define LCD_RD_HIGH()		LPC_GPIO0->FIOSET = ex(25)
#define LCD_WR_LOW()		LPC_GPIO0->FIOCLR = ex(24)
#define LCD_WR_HIGH()		LPC_GPIO0->FIOSET = ex(24)

#define LCD_WR_IN()			LPC_GPIO0->FIODIR &= rex(24)
#define LCD_WR_OUT()		LPC_GPIO0->FIODIR |= ex(24)

void lcdPinInit(void);

static void delay(void)
{
	uint32 i;

	for (i = 0; i < 25; i++);
}

__attribute__((always_inline)) void lcdPortIn(void)
{
	LPC_GPIO2->FIODIR &= 0xFFFFFF00;
	LCD_DIR_LOW();
}

__attribute__((always_inline)) void lcdPortOut(void)
{
	LCD_DIR_HIGH();
	LPC_GPIO2->FIODIR |= 0xFF;
}

__attribute__((always_inline)) uint16 lcdPortRead(void)
{
	uint16 data;

	delay();
	//OSTimeDly(1);
	data = LPC_GPIO2->FIOPIN0;
	LCD_EN_HIGH();
	delay();
	//OSTimeDly(0);
	data = (data << 8) | LPC_GPIO2->FIOPIN0;
	LCD_EN_LOW();
	return data;
}

__attribute__((always_inline)) void lcdPortWrite(uint16 data)
{
	LPC_GPIO2->FIOPIN = data;
	LCD_LE_HIGH();
	LCD_LE_LOW();
	LPC_GPIO2->FIOPIN = data >> 8;
}

__attribute__((always_inline)) uint16 lcdStatusR(void)
{
	uint16 status;

	LCD_RS_LOW();
	LCD_RD_LOW();
	status = lcdPortRead();
	LCD_RD_HIGH();
	LCD_RS_HIGH();
	return status;
}

__attribute__((always_inline)) void lcdIndexW(uint16 index)
{
	LCD_RS_LOW();
	LCD_WR_LOW();
	lcdPortWrite(index);
	LCD_WR_HIGH();
	LCD_RS_HIGH();
}

__attribute__((always_inline)) uint16 lcdDataR(void)
{
	uint16 data;

	LCD_RD_LOW();
	data = lcdPortRead();
	LCD_RD_HIGH();
	return data;
}

__attribute__((always_inline)) void lcdDataW(uint16 data)
{
	LCD_WR_LOW();
	lcdPortWrite(data);
	LCD_WR_HIGH();
}

#endif
