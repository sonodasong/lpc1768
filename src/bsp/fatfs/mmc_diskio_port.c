#include "mmc_diskio_port.h"

void power_on(void)
{
	/* CS pin configuration */
	pinSel(1, 21, 0);
	pinMode(1, 21, 0);
	pinModeOD(1, 21, 0);
	LPC_GPIO1->FIOMASK &= rex(21);
	LPC_GPIO1->FIODIR |= ex(21);

	/* CD pin configuration */
	pinSel(3, 25, 0);
	pinMode(3, 25, 0);
	pinModeOD(3, 25, 0);
	LPC_GPIO3->FIODIR &= rex(25);

	/* power pin configuration */
	pinSel(3, 26, 0);
	pinMode(3, 26, 0);
	pinModeOD(3, 26, 0);
	LPC_GPIO3->FIOMASK &= rex(26);
	LPC_GPIO3->FIODIR |= ex(26);

	LPC_GPIO3->FIOCLR = ex(26);
	OSTimeDly(100 / (1000 /OS_TICKS_PER_SEC));
}

void power_off(void)
{
	LPC_GPIO3->FIOSET = ex(26);
}

BYTE xchg_spi(BYTE dat)
{
	ssp0BufRw(&dat, &dat, 1);
	return dat;
}
