#include "user.h"

static OS_STK stack0[128];
static OS_STK stack1[128];

int main(void)
{
	SystemInit();
	__disable_irq();
	SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);

	OSInit();

	uart0Init();

	OSTaskCreate(serial, (void*)0, &stack0[127], 0);
	OSTaskCreate(blink, (void*)0, &stack1[127], 1);

	OSStart();

	return 0;
}
