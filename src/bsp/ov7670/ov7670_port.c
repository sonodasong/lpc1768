#include "ov7670_port.h"

static void (*ov7670HandlerPtr)(void);

void ov7670IRQInit(void (*handlerPtr)(void))
{
	ov7670HandlerPtr = handlerPtr;
	pinSel(2, 10, 1);
	pinMode(2, 10, 0);
	pinModeOD(2, 10, 0);
	LPC_SC->EXTMODE |= ex(0);
	LPC_SC->EXTPOLAR |= ex(0);
}

void ov7670IRQEnable(void)
{
	NVIC_EnableIRQ(EINT0_IRQn);
}

void ov7670IRQDisable(void)
{
	NVIC_DisableIRQ(EINT0_IRQn);
}

void ov7670IRQClear(void)
{
	LPC_SC->EXTINT = ex(0);
	NVIC_ClearPendingIRQ(EINT0_IRQn);
}

void ov7670ExternalClockInit(void)
{
	timer3Init();
}

void ov7670ExternalClockEnable(void)
{
	timer3Enable();
}

void ov7670RegW(uint8 reg, uint8 data)
{
	i2c0RegW(0x21, reg, &data, 1);
}

void EINT0_IRQHandler(void)
{
	ov7670IRQClear();
	ov7670HandlerPtr();
}
