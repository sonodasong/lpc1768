#ifndef __TASK_H__
#define __TASK_H__

#include <bsp.h>

void blink(void *pdata);
void serial(void *pdata);
void lcdDisplayBmpTask(void *pdata);
void lcdGramReadTask(void *pdata);
void fatfsTimerTask(void *pdata);

#endif
