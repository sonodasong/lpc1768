#ifndef __OV7670_H__
#define __OV7670_H__

#include <driver.h>

#define OV7670_TIMEOUT		0xFFFFFFFF

void ov7670InitFromTask(void (*handlerPtr)(void));
void ov7670Enable(void);
INT8U ov7670Disable(void);

#endif
