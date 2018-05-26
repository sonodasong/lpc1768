#ifndef __TASK_H__
#define __TASK_H__

#include <bsp.h>

void blink(void *pdata);
void serial(void *pdata);
void lcdCmosTask(void *pdata);
void fatfsTimerTask(void *pdata);

#endif
