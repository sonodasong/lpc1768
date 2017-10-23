#include "pin.h"
#include "uart0.h"

static char uart0RxBuf[ex(UART0_RX_Q_SIZE)][ex(UART0_RX_SIZE)];
static uint8 uart0RxCnt;
static uint8 uart0RxQCnt;
static void *uart0RxQPtr[ex(UART0_RX_Q_SIZE)];
static OS_EVENT *uart0RxQ;

static char *uart0TxPtr;
static uint8 uart0TxCnt;
static OS_EVENT *uart0TxRdy;

static void uart0RDAHandler(void);
static void uart0CTIHandler(void);
static void uart0THREHandler(void);

void uart0Init(void)
{
	/* enable power */
	LPC_SC->PCONP |= ex(3);

	/* peripheral clock, 100 MHz */
	LPC_SC->PCLKSEL0 &= rex(7);
	LPC_SC->PCLKSEL0 |= ex(6);

	/* RX0 pin configuration */
	pinSel(0, 3, 1);
	pinMode(0, 3, 0);
	pinModeOD(0, 3, 0);

	/* TXD0 pin configuration */
	pinSel(0, 2, 1);
	pinMode(0, 2, 0);
	pinModeOD(0, 2, 0);

	/* baud rate 115200 */
	LPC_UART0->LCR |= ex(7);
	LPC_UART0->DLL = 0x1F;
	LPC_UART0->DLM = 0x00;
	LPC_UART0->LCR &= rex(7);
	LPC_UART0->FDR = 0xC9;

	/* line control, 8-bit, 1 stop bit, no parity check */
	LPC_UART0->LCR = 0x03;

	/* enable FIFO */
	LPC_UART0->FCR = 0x81;
	LPC_UART0->FCR |= 0x06;

	/* transmit enable */
	LPC_UART0->TER = 0x80;

	/* other */
	LPC_UART0->ACR = 0x00;
	LPC_UART0->ICR = 0x00;

	/* semaphore and queue */
	uart0RxQ = OSQCreate(uart0RxQPtr, ex(UART0_RX_Q_SIZE));
	uart0RxCnt = 0;
	uart0RxQCnt = 0;
	uart0TxRdy = OSSemCreate(0);

	/* set up interrupt */
	LPC_UART0->IER = 0x03;
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
}

INT8U uart0Read(char **str)
{
	INT8U err;

	*str = (char *)OSQPend(uart0RxQ, UART0_RX_TIMEOUT, &err);
	return err;
}

INT8U uart0Print(char *str)
{
	OS_CPU_SR cpu_sr;
	INT8U err;
	char temp;

	uart0TxPtr = str;
	uart0TxCnt = 0;
	temp = str[0];
	if (temp == '\0') {
		return 0;
	}
	OS_ENTER_CRITICAL();
	do {
		LPC_UART0->THR = temp;
		uart0TxCnt++;
		temp = str[uart0TxCnt];
	} while ((temp != '\0') && (uart0TxCnt < 16));
	OS_EXIT_CRITICAL();
	OSSemPend(uart0TxRdy, UART0_TX_TIMEOUT, &err);
	return err;
}

#if UART0_ENABLE_PRINTF

#include <stdio.h>
#include <stdarg.h>

static char uart0TxBuf[64];

void uart0Printf(char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vsprintf(uart0TxBuf, fmt, ap);
	va_end(ap);
	uart0Print(uart0TxBuf);
}

#endif

void UART0_IRQHandler(void)
{
	uint32 intID;

	intID = LPC_UART0->IIR & 0x0E;
	if (intID == 0x04) {
		uart0RDAHandler();
	} else if (intID == 0x0C) {
		uart0CTIHandler();
	} else if (intID == 0x02) {
		uart0THREHandler();
	}
}

static void uart0RDAHandler(void)
{
	char temp;
	uint8 i;
	boolean osCalled;

	for (i = 0; i < UART0_RX_TRIGGER_LEVEL; i++) {
		temp = LPC_UART0->RBR;
		if (temp == UART0_RX_EOF) {
			uart0RxBuf[uart0RxQCnt][uart0RxCnt] = '\0';
			uart0RxCnt = 0;
			if (osCalled == FALSE) {
				osCalled = TRUE;
				OSIntEnter();
			}
			OSQPost(uart0RxQ, uart0RxBuf[uart0RxQCnt]);
			uart0RxQCnt++;
			uart0RxQCnt &= ex(UART0_RX_Q_SIZE) - 1;
		} else {
			uart0RxBuf[uart0RxQCnt][uart0RxCnt] = temp;
			uart0RxCnt++;
			uart0RxCnt &= ex(UART0_RX_SIZE) - 1;
		}
	}
	if (osCalled == TRUE) {
		OSIntExit();
	}
}

static void uart0CTIHandler(void)
{
	char temp;
	boolean osCalled;

	while (LPC_UART0->LSR & 0x01) {
		temp = LPC_UART0->RBR;
		if (temp == UART0_RX_EOF) {
			uart0RxBuf[uart0RxQCnt][uart0RxCnt] = '\0';
			uart0RxCnt = 0;
			if (osCalled == FALSE) {
				osCalled = TRUE;
				OSIntEnter();
			}
			OSQPost(uart0RxQ, uart0RxBuf[uart0RxQCnt]);
			uart0RxQCnt++;
			uart0RxQCnt &= ex(UART0_RX_Q_SIZE) - 1;
		} else {
			uart0RxBuf[uart0RxQCnt][uart0RxCnt] = temp;
			uart0RxCnt++;
			uart0RxCnt &= ex(UART0_RX_SIZE) - 1;
		}
	}
	if (osCalled == TRUE) {
		OSIntExit();
	}
}

static void uart0THREHandler(void)
{
	char temp;
	uint8 txFIFOCnt;

	temp = uart0TxPtr[uart0TxCnt];
	if (temp == '\0') {
		OSIntEnter();
		OSSemPost(uart0TxRdy);
		OSIntExit();
	} else {
		txFIFOCnt = 0;
		while ((temp != '\0') && (txFIFOCnt < 16)) {
			LPC_UART0->THR = temp;
			txFIFOCnt++;
			uart0TxCnt++;
			temp = uart0TxPtr[uart0TxCnt];
		}
	}
}
