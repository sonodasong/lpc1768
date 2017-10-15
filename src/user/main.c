#include "user.h"

static OS_STK stack0[512];
static OS_STK stack1[128];
static OS_STK stack2[128];

int main(void)
{
	SystemInit();
	__disable_irq();
	SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);

	OSInit();

	ssp0Init();
	uart0Init();

	OSTaskCreate(fatfsTask, (void*)0, &stack0[511], 0);
	OSTaskCreate(fatfsTimerTask, (void*)0, &stack1[127], 1);
	OSTaskCreate(blink, (void*)0, &stack2[127], 2);

	OSStart();

	return 0;
}
