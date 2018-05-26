#include "pin.h"
#include "timer3.h"

void timer3Init(void)
{
	/* enable power */
	LPC_SC->PCONP |= ex(23);

	/* peripheral clock, 100 MHz */
	LPC_SC->PCLKSEL1 &= rex(15);
	LPC_SC->PCLKSEL1 |= ex(14);

	/* MAT0 pin configuration */
	pinSel(0, 10, 3);
	pinMode(0, 10, 0);
	pinModeOD(0, 10, 0);

	/* timer3 setup */
	LPC_TIM3->TCR = 0x00;
	LPC_TIM3->CTCR = 0x00;
	LPC_TIM3->MCR = 0x02;
	LPC_TIM3->EMR = 0x30;

	/* 100 / 2 / (PR + 1) / (MR0 + 1) */
	LPC_TIM3->PR = 1;
	LPC_TIM3->MR0 = 1;
}

void timer3Enable(void)
{
	LPC_TIM3->TCR |= ex(0);
}
