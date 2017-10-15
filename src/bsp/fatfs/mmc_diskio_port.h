#ifndef __MMC_DISKIO_PORT_H__
#define __MMC_DISKIO_PORT_H__

#include <driver.h>
#include "diskio.h"

#define CS_LOW()					LPC_GPIO1->FIOCLR = ex(21)
#define CS_HIGH()					LPC_GPIO1->FIOSET = ex(21)
#define MMC_CD()					TRUE
#define MMC_WP()					FALSE
#define FCLK_SLOW()					ssp0ClockSlow()
#define FCLK_FAST()					ssp0ClockFast()
#define rcvr_spi_multi(p, cnt)		ssp0BufR(p, cnt, 0xFF)
#define xmit_spi_multi(p, cnt);		ssp0BufW((uint8 *)p, cnt)

void power_on(void);
void power_off(void);
BYTE xchg_spi(BYTE dat);

#endif
