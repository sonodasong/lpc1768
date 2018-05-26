#include "pin.h"
#include "ssp0.h"

#define SSP0_FIFO_SIZE		8

static uint8 *ssp0RxBuf;
static uint8 *ssp0TxBuf;
static uint16 ssp0Size;
static uint16 ssp0RxCnt;
static uint16 ssp0TxCnt;
static uint8 ssp0Fill;
static OS_EVENT *ssp0Rdy;

static void (*ssp0HandlerPtr)(void);

static void ssp0BufRHandler(void);
static void ssp0BufWHandler(void);
static void ssp0BufRwHandler(void);

void ssp0Init(void)
{
	/* enable power */
	LPC_SC->PCONP |= ex(21);

	/* peripheral clock, 100 MHz */
	LPC_SC->PCLKSEL1 &= rex(11);
	LPC_SC->PCLKSEL1 |= ex(10);

	/* MOSI pin configuration */
	pinSel(1, 24, 3);
	pinMode(1, 24, 0);
	pinModeOD(1, 24, 0);

	/* MISO pin configuration */
	pinSel(1, 23, 3);
	pinMode(1, 23, 0);
	pinModeOD(1, 23, 0);

	/* SCK pin configuration */
	pinSel(1, 20, 3);
	pinMode(1, 20, 0);
	pinModeOD(1, 20, 0);

	/* 8-bit, CPHA = 0, CPOL = 0 */
	LPC_SSP0->CR0 = 0x0107;

	/* 250 kHz */
	LPC_SSP0->CPSR = 200;

	/* semaphore */
	ssp0Rdy = OSSemCreate(0);

	/* setup interrupt */
	LPC_SSP0->IMSC = ex(1);
	NVIC_ClearPendingIRQ(SSP0_IRQn);
	NVIC_EnableIRQ(SSP0_IRQn);

	/* enable SSP0 */
	LPC_SSP0->CR1 |= ex(1);

}

INT8U ssp0BufR(uint8 *buf, uint16 size, uint8 fill)
{
	OS_CPU_SR cpu_sr;
	INT8U err;

	ssp0RxBuf = buf;
	ssp0Size = size;
	ssp0RxCnt = 0;
	ssp0TxCnt = 0;
	ssp0Fill = fill;
	ssp0HandlerPtr = ssp0BufRHandler;
	if (size > SSP0_FIFO_SIZE) {
		size = SSP0_FIFO_SIZE;
	}
	OS_ENTER_CRITICAL();
	while (ssp0TxCnt < size) {
		LPC_SSP0->DR = fill;
		ssp0TxCnt++;
	}
	OS_EXIT_CRITICAL();
	OSSemPend(ssp0Rdy, SSP0_TIMEOUT, &err);
	return err;
}

INT8U ssp0BufW(uint8 *buf, uint16 size)
{
	OS_CPU_SR cpu_sr;
	INT8U err;

	ssp0TxBuf = buf;
	ssp0Size = size;
	ssp0RxCnt = 0;
	ssp0TxCnt = 0;
	ssp0HandlerPtr = ssp0BufWHandler;
	if (size > SSP0_FIFO_SIZE) {
		size = SSP0_FIFO_SIZE;
	}
	OS_ENTER_CRITICAL();
	while (ssp0TxCnt < size) {
		LPC_SSP0->DR = buf[ssp0TxCnt];
		ssp0TxCnt++;
	}
	OS_EXIT_CRITICAL();
	OSSemPend(ssp0Rdy, SSP0_TIMEOUT, &err);
	return err;
}

INT8U ssp0BufRw(uint8 *rxBuf, uint8 *txBuf, uint16 size)
{
	OS_CPU_SR cpu_sr;
	INT8U err;

	ssp0RxBuf = rxBuf;
	ssp0TxBuf = txBuf;
	ssp0Size = size;
	ssp0RxCnt = 0;
	ssp0TxCnt = 0;
	ssp0HandlerPtr = ssp0BufRwHandler;
	if (size > SSP0_FIFO_SIZE) {
		size = SSP0_FIFO_SIZE;
	}
	OS_ENTER_CRITICAL();
	while (ssp0TxCnt < size) {
		LPC_SSP0->DR = txBuf[ssp0TxCnt];
		ssp0TxCnt++;
	}
	OS_EXIT_CRITICAL();
	OSSemPend(ssp0Rdy, SSP0_TIMEOUT, &err);
	return err;
}

void ssp0ClockSlow(void)
{
	/* 250 kHz */
	LPC_SSP0->CPSR = 200;
}

void ssp0ClockFast(void)
{
	/* 25 MHz */
	LPC_SSP0->CPSR = 2;
}

void SSP0_IRQHandler(void)
{
	ssp0HandlerPtr();
}

static void ssp0BufRHandler(void)
{
	uint8 dummy;
	uint16 i;

	(void)dummy;
	while (LPC_SSP0->SR & ex(2)) {
		ssp0RxBuf[ssp0RxCnt] = LPC_SSP0->DR;
		ssp0RxCnt++;
		if (ssp0RxCnt == ssp0Size) {
			while (LPC_SSP0->SR & ex(2)) {
				dummy = LPC_SSP0->DR;
			}
			LPC_SSP0->ICR = ex(1);
			OSIntEnter();
			OSSemPost(ssp0Rdy);
			OSIntExit();
		}
	}
	LPC_SSP0->ICR = ex(1);
	for (i = 0; (ssp0TxCnt < ssp0Size) && (i < SSP0_FIFO_SIZE); i++) {
		LPC_SSP0->DR = ssp0Fill;
		ssp0TxCnt++;
	}
}

static void ssp0BufWHandler(void)
{
	uint8 dummy;
	uint16 i;

	(void)dummy;
	while (LPC_SSP0->SR & ex(2)) {
		dummy = LPC_SSP0->DR;
		ssp0RxCnt++;
		if (ssp0RxCnt == ssp0Size) {
			while (LPC_SSP0->SR & ex(2)) {
				dummy = LPC_SSP0->DR;
			}
			LPC_SSP0->ICR = ex(1);
			OSIntEnter();
			OSSemPost(ssp0Rdy);
			OSIntExit();
		}
	}
	LPC_SSP0->ICR = ex(1);
	for (i = 0; (ssp0TxCnt < ssp0Size) && (i < SSP0_FIFO_SIZE); i++) {
		LPC_SSP0->DR = ssp0TxBuf[ssp0TxCnt];
		ssp0TxCnt++;
	}
}

static void ssp0BufRwHandler(void)
{
	uint8 dummy;
	uint16 i;

	(void)dummy;
	while (LPC_SSP0->SR & ex(2)) {
		ssp0RxBuf[ssp0RxCnt] = LPC_SSP0->DR;
		ssp0RxCnt++;
		if (ssp0RxCnt == ssp0Size) {
			while (LPC_SSP0->SR & ex(2)) {
				dummy = LPC_SSP0->DR;
			}
			LPC_SSP0->ICR = ex(1);
			OSIntEnter();
			OSSemPost(ssp0Rdy);
			OSIntExit();
		}
	}
	LPC_SSP0->ICR = ex(1);
	for (i = 0; (ssp0TxCnt < ssp0Size) && (i < SSP0_FIFO_SIZE); i++) {
		LPC_SSP0->DR = ssp0TxBuf[ssp0TxCnt];
		ssp0TxCnt++;
	}
}
