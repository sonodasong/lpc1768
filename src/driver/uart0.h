#ifndef __UART0_H__
#define __UART0_H__

#include "define.h"

/* queue size is 2 ^ UART0_RX_Q_SIZE */
#define UART0_RX_Q_SIZE				2
/* buffer size is 2 ^ UART0_RX_SIZE */
#define UART0_RX_SIZE				4
#define UART0_RX_TIMEOUT			0xFFFFFFFF
#define UART0_RX_TRIGGER_LEVEL		8
#define UART0_RX_EOF				'\r'

#define UART0_TX_TIMEOUT			0xFFFFFFFF

#define UART0_ENABLE_PRINTF			TRUE

void uart0Init(void);
INT8U uart0Read(char **str);
INT8U uart0Print(char *str);
#if UART0_ENABLE_PRINTF
void uart0Printf(char *fmt, ...);
#endif

#endif
