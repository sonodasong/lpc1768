#include "user.h"

static OS_STK stack0[512];
static OS_STK stack1[128];

int main(void)
{
	SystemInit();
	__disable_irq();
	SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);

	OSInit();

	i2c0Init();
	ssp0Init();
	uart0Init();

	OSTaskCreate(lcdCmosTask, (void*)0, &stack0[511], 0);
	OSTaskCreate(fatfsTimerTask, (void*)0, &stack1[127], 1);

	OSStart();

	return 0;
}
