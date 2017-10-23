#ifndef __SSP0_H__
#define __SSP0_H__

#include "define.h"

#define SSP0_TIMEOUT		0xFFFFFFFF

void ssp0Init(void);
INT8U ssp0BufR(uint8 *buf, uint16 size, uint8 fill);
INT8U ssp0BufW(uint8 *buf, uint16 size);
INT8U ssp0BufRw(uint8 *rxBuf, uint8 *txBuf, uint16 size);
void ssp0ClockSlow(void);
void ssp0ClockFast(void);

#endif
