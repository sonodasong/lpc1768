#include "pin.h"
#include "spi.h"

static uint8 *spiRxBuf;
static uint8 *spiTxBuf;
static uint16 spiSize;
static uint16 spiCnt;
static uint8 spiFill;
static OS_EVENT *spiRdy;

static void (*spiHandlerPtr)(void);
static void spiBufRHandler(void);
static void spiBufWHandler(void);
static void spiBufWRHandler(void);

void spiInit(void)
{
	/* enable power */
	LPC_SC->PCONP |= ex(8);

	/* peripheral clock, 100 MHz */
	LPC_SC->PCLKSEL0 &= rex(17);
	LPC_SC->PCLKSEL0 |= ex(16);

	/* MOSI pin configuration */
	pinSel(0, 18, 3);
	pinMode(0, 18, 0);
	pinModeOD(0, 18, 0);

	/* MISO pin configuration */
	pinSel(0, 17, 3);
	pinMode(0, 17, 0);
	pinModeOD(0, 17, 0);

	/* SCK pin configuration */
	pinSel(0, 15, 3);
	pinMode(0, 15, 0);
	pinModeOD(0, 15, 0);

	/* 8-bit, CPHA = 0, CPOL = 0 */
	LPC_SPI->SPCR = 0x0820;

	/* 12.5 MHz */
	LPC_SPI->SPCCR = 8;

	/* semaphore */
	spiRdy = OSSemCreate(0);

	/* setup interrupt */
	LPC_SPI->SPCR |= ex(7);
	NVIC_ClearPendingIRQ(SPI_IRQn);
	NVIC_EnableIRQ(SPI_IRQn);
}

INT8U spiBufR(uint8 *buf, uint16 size, uint8 fill)
{
	INT8U err;

	spiRxBuf = buf;
	spiSize = size;
	spiCnt = 0;
	spiFill = fill;
	spiHandlerPtr = spiBufRHandler;
	LPC_SPI->SPDR = fill;
	OSSemPend(spiRdy, SPI_TIMEOUT, &err);
	return err;
}

INT8U spiBufW(uint8 *buf, uint16 size)
{
	INT8U err;

	spiTxBuf = buf;
	spiSize = size;
	spiCnt = 1;
	spiHandlerPtr = spiBufWHandler;
	LPC_SPI->SPDR = buf[0];
	OSSemPend(spiRdy, SPI_TIMEOUT, &err);
	return err;
}

INT8U spiBufWR(uint8 *txBuf, uint8 *rxBuf, uint16 size)
{
	INT8U err;

	spiTxBuf = txBuf;
	spiRxBuf = rxBuf;
	spiSize = size;
	spiCnt = 0;
	spiHandlerPtr = spiBufWRHandler;
	LPC_SPI->SPDR = txBuf[0];
	OSSemPend(spiRdy, SPI_TIMEOUT, &err);
	return err;
}

void spiClockSlow(void)
{
	/* 250 kHz */
	LPC_SPI->SPCCR = 400;
}

void spiClockFast(void)
{
	/* 12.5 MHz */
	LPC_SPI->SPCCR = 8;
}

void SPI_IRQHandler(void)
{
	spiHandlerPtr();
}

static void spiBufRHandler(void)
{
	uint32 status;

	status = LPC_SPI->SPSR;
	LPC_SPI->SPINT = ex(0);
	if (status & 0x80) {
		spiRxBuf[spiCnt] = LPC_SPI->SPDR;
		spiCnt++;
		if (spiCnt < spiSize) {
			LPC_SPI->SPDR = spiFill;
		} else {
			OSIntEnter();
			OSSemPost(spiRdy);
			OSIntExit();
		}
	}
	//LPC_SPI->SPINT = ex(0);
}

static void spiBufWHandler(void)
{
	uint32 status;

	status = LPC_SPI->SPSR;
	LPC_SPI->SPINT = ex(0);
	if (status & 0x80) {
		if (spiCnt < spiSize) {
			LPC_SPI->SPDR = spiTxBuf[spiCnt];
			spiCnt++;
		} else {
			OSIntEnter();
			OSSemPost(spiRdy);
			OSIntExit();
		}
	}
	//LPC_SPI->SPINT = ex(0);
}

static void spiBufWRHandler(void)
{
	uint32 status;

	status = LPC_SPI->SPSR;
	LPC_SPI->SPINT = ex(0);
	if (status & 0x80) {
		spiRxBuf[spiCnt] = LPC_SPI->SPDR;
		spiCnt++;
		if (spiCnt < spiSize) {
			LPC_SPI->SPDR = spiTxBuf[spiCnt];
		} else {
			OSIntEnter();
			OSSemPost(spiRdy);
			OSIntExit();
		}
	}
	//LPC_SPI->SPINT = ex(0);
}
