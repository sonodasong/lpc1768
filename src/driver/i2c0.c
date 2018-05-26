#include "pin.h"
#include "i2c0.h"

#define i2c0IntFlagClr()		LPC_I2C0->I2CONCLR = ex(3)

#define i2c0Start()				LPC_I2C0->I2CONSET = ex(5)
#define i2c0StartClr()			LPC_I2C0->I2CONCLR = ex(5)
#define i2c0Ack()				LPC_I2C0->I2CONSET = ex(2)
#define i2c0NAck()				LPC_I2C0->I2CONCLR = ex(2)
#define i2c0Stop()				LPC_I2C0->I2CONSET = ex(4)

static uint8 i2c0Address;
static uint8 i2c0Reg;
static uint8 i2c0ErrorCode;

static uint8 *i2c0RxPtr;
static uint8 i2c0RxCnt;
static uint8 i2c0RxSize;

static uint8 *i2c0TxPtr;
static uint8 i2c0TxCnt;
static uint8 i2c0TxSize;

static OS_EVENT *i2c0Ready;

static void (*i2c0HandlerPtr)(void);

static void i2c0RegRHandler(void);
static void i2c0RegWHandler(void);

void i2c0Init(void)
{
	/* enable power */
	LPC_SC->PCONP |= ex(7);

	/* peripheral clock, 50 MHz */
	LPC_SC->PCLKSEL0 &= 0xFFFF3FFF;
	LPC_SC->PCLKSEL0 |= 0x8000;

	/* SDA0 pin configuration */
	pinSel(0, 27, 1);
	pinMode(0, 27, 2);
	pinModeOD(0, 27, 1);

	/* SCL0 pin configuration */
	pinSel(0, 28, 1);
	pinMode(0, 28, 2);
	pinModeOD(0, 28, 1);

	LPC_PINCON->I2CPADCFG = 0x00;

	/* 400 kHz */
	LPC_I2C0->I2CONCLR = 0x64;
	LPC_I2C0->MMCTRL = 0x00;
	LPC_I2C0->I2SCLH = 62;
	LPC_I2C0->I2SCLL = 63;

	/* semaphore */
	i2c0Ready = OSSemCreate(0);

	/* interrupt */
	i2c0IntFlagClr();
	NVIC_ClearPendingIRQ(I2C0_IRQn);
	NVIC_EnableIRQ(I2C0_IRQn);

	/* enable I2C0 */
	LPC_I2C0->I2CONSET = ex(6);
}

#include "uart0.h"
INT8U i2c0RegR(uint8 address, uint8 reg, uint8 *rx, uint8 rxSize)
{
	INT8U err;

	i2c0Address = address;
	i2c0Reg = reg;
	i2c0RxPtr = rx;
	i2c0RxCnt = 0;
	i2c0RxSize = rxSize;
	i2c0HandlerPtr = i2c0RegRHandler;
	i2c0Start();
	OSSemPend(i2c0Ready, I2C0_TIME_OUT, &err);
	return err;
}

uint8 i2c0RegRCount(void)
{
	return i2c0RxCnt + 1;
}

INT8U i2c0RegW(uint8 address, uint8 reg, uint8 *tx, uint8 txSize)
{
	INT8U err;

	i2c0Address = address;
	i2c0Reg = reg;
	i2c0TxPtr = tx;
	i2c0TxCnt = 0;
	i2c0TxSize = txSize;
	i2c0HandlerPtr = i2c0RegWHandler;
	i2c0Start();
	OSSemPend(i2c0Ready, I2C0_TIME_OUT, &err);
	return err;
}

uint8 i2c0RegWCount(void)
{
	return i2c0TxCnt;
}

void I2C0_IRQHandler(void)
{
	(*i2c0HandlerPtr)();
}

static void i2c0RegRHandler(void)
{
	i2c0ErrorCode = LPC_I2C0->I2STAT & 0xF8;
	if (i2c0ErrorCode == 0x08) {
		i2c0StartClr();
		LPC_I2C0->I2DAT = i2c0Address << 1;
		i2c0IntFlagClr();
	} else if (i2c0ErrorCode == 0x10) {
		i2c0StartClr();
		LPC_I2C0->I2DAT = (i2c0Address << 1) | 0x01;
		i2c0IntFlagClr();
	} else if (i2c0ErrorCode == 0x18) {
		LPC_I2C0->I2DAT = i2c0Reg;
		i2c0IntFlagClr();
	} else if (i2c0ErrorCode == 0x20) {
		i2c0Stop();
		i2c0IntFlagClr();
		OSIntEnter();
		OSSemPost(i2c0Ready);
		OSIntExit();
	} else if (i2c0ErrorCode == 0x28) {
		i2c0Start();
		i2c0IntFlagClr();
	} else if (i2c0ErrorCode == 0x30) {
		i2c0Stop();
		i2c0IntFlagClr();
		OSIntEnter();
		OSSemPost(i2c0Ready);
		OSIntExit();
	} else if (i2c0ErrorCode == 0x38) {
		i2c0IntFlagClr();
		OSIntEnter();
		OSSemPost(i2c0Ready);
		OSIntExit();
	} else if (i2c0ErrorCode == 0x40) {
		if (i2c0RxSize < 2) {
			i2c0NAck();
			i2c0IntFlagClr();
		} else {
			i2c0Ack();
			i2c0IntFlagClr();
		}
	} else if (i2c0ErrorCode == 0x48) {
		i2c0Stop();
		i2c0IntFlagClr();
		OSIntEnter();
		OSSemPost(i2c0Ready);
		OSIntExit();
	} else if (i2c0ErrorCode == 0x50) {
		*(i2c0RxPtr + i2c0RxCnt) = LPC_I2C0->I2DAT;
		i2c0RxCnt++;
		if (i2c0RxCnt < (i2c0RxSize - 1)) {
			i2c0Ack();//
		} else {
			i2c0NAck();
		}
		i2c0IntFlagClr();
	} else if (i2c0ErrorCode == 0x58) {
		*(i2c0RxPtr + i2c0RxCnt) = LPC_I2C0->I2DAT;
		i2c0Stop();
		i2c0IntFlagClr();
		OSIntEnter();
		OSSemPost(i2c0Ready);
		OSIntExit();
	}
}

static void i2c0RegWHandler(void)
{
	i2c0ErrorCode = LPC_I2C0->I2STAT & 0xF8;
	if (i2c0ErrorCode == 0x08) {
		i2c0StartClr();
		LPC_I2C0->I2DAT = i2c0Address << 1;
		i2c0IntFlagClr();
	} else if (i2c0ErrorCode == 0x10) {
		i2c0StartClr();
		LPC_I2C0->I2DAT = i2c0Address << 1;
		i2c0IntFlagClr();
	} else if (i2c0ErrorCode == 0x18) {
		LPC_I2C0->I2DAT = i2c0Reg;
		i2c0IntFlagClr();
	} else if (i2c0ErrorCode == 0x20) {
		i2c0Stop();
		i2c0IntFlagClr();
		OSIntEnter();
		OSSemPost(i2c0Ready);
		OSIntExit();
	} else if (i2c0ErrorCode == 0x28) {
		if (i2c0TxCnt < i2c0TxSize) {
			LPC_I2C0->I2DAT = *(i2c0TxPtr + i2c0TxCnt);
			i2c0TxCnt++;
			i2c0IntFlagClr();
		} else {
			i2c0Stop();
			i2c0IntFlagClr();
			OSIntEnter();
			OSSemPost(i2c0Ready);
			OSIntExit();
		}
	} else if (i2c0ErrorCode == 0x30) {
		i2c0Stop();
		i2c0IntFlagClr();
		OSIntEnter();
		OSSemPost(i2c0Ready);
		OSIntExit();
	} else if (i2c0ErrorCode == 0x38) {
		i2c0IntFlagClr();
		OSIntEnter();
		OSSemPost(i2c0Ready);
		OSIntExit();
	}
}
