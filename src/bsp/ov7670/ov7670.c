#include "ov7670_port.h"
#include "ov7670.h"

#define MS_10		(10 * OS_TICKS_PER_SEC / 1000)

static boolean ov7670Enabled = FALSE;
static OS_EVENT *ov7670Disabled;

static void (*ov7670HandlerPtr)(void);

static void ov7670Handler(void);

void ov7670InitFromTask(void (*handlerPtr)(void))
{
	ov7670IRQDisable();
	ov7670HandlerPtr = handlerPtr;
	ov7670IRQInit(ov7670Handler);
	ov7670ExternalClockInit();
	ov7670ExternalClockEnable();
	ov7670Disabled = OSSemCreate(0);

	ov7670RegW(0x12, 0x80); OSTimeDly(MS_10);
	/* delay? */

	ov7670RegW(0x3a, 0x04); OSTimeDly(MS_10);
	ov7670RegW(0x40, 0xd0); OSTimeDly(MS_10);
	ov7670RegW(0x12, 0x14); OSTimeDly(MS_10);
	ov7670RegW(0x32, 0x80); OSTimeDly(MS_10);
	ov7670RegW(0x17, 0x16); OSTimeDly(MS_10);
	ov7670RegW(0x18, 0x04); OSTimeDly(MS_10);
	ov7670RegW(0x19, 0x02); OSTimeDly(MS_10);
	ov7670RegW(0x1a, 0x7b); OSTimeDly(MS_10);
	ov7670RegW(0x03, 0x06); OSTimeDly(MS_10);
	ov7670RegW(0x0c, 0x04); OSTimeDly(MS_10);
	ov7670RegW(0x3e, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x70, 0x3a); OSTimeDly(MS_10);
	ov7670RegW(0x71, 0x35); OSTimeDly(MS_10);
	ov7670RegW(0x72, 0x11); OSTimeDly(MS_10);
	ov7670RegW(0x73, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0xa2, 0x02); OSTimeDly(MS_10);
	ov7670RegW(0x11, 0x81); OSTimeDly(MS_10);

	ov7670RegW(0x7a, 0x20); OSTimeDly(MS_10);
	ov7670RegW(0x7b, 0x1c); OSTimeDly(MS_10);
	ov7670RegW(0x7c, 0x28); OSTimeDly(MS_10);
	ov7670RegW(0x7d, 0x3c); OSTimeDly(MS_10);
	ov7670RegW(0x7e, 0x55); OSTimeDly(MS_10);
	ov7670RegW(0x7f, 0x68); OSTimeDly(MS_10);
	ov7670RegW(0x80, 0x76); OSTimeDly(MS_10);
	ov7670RegW(0x81, 0x80); OSTimeDly(MS_10);
	ov7670RegW(0x82, 0x88); OSTimeDly(MS_10);
	ov7670RegW(0x83, 0x8f); OSTimeDly(MS_10);
	ov7670RegW(0x84, 0x96); OSTimeDly(MS_10);
	ov7670RegW(0x85, 0xa3); OSTimeDly(MS_10);
	ov7670RegW(0x86, 0xaf); OSTimeDly(MS_10);
	ov7670RegW(0x87, 0xc4); OSTimeDly(MS_10);
	ov7670RegW(0x88, 0xd7); OSTimeDly(MS_10);
	ov7670RegW(0x89, 0xe8); OSTimeDly(MS_10);

	ov7670RegW(0x13, 0xe0); OSTimeDly(MS_10);
	ov7670RegW(0x00, 0x00); OSTimeDly(MS_10);

	ov7670RegW(0x10, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x0d, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x14, 0x28); OSTimeDly(MS_10);
	ov7670RegW(0xa5, 0x05); OSTimeDly(MS_10);
	ov7670RegW(0xab, 0x07); OSTimeDly(MS_10);
	ov7670RegW(0x24, 0x75); OSTimeDly(MS_10);
	ov7670RegW(0x25, 0x63); OSTimeDly(MS_10);
	ov7670RegW(0x26, 0xA5); OSTimeDly(MS_10);
	ov7670RegW(0x9f, 0x78); OSTimeDly(MS_10);
	ov7670RegW(0xa0, 0x68); OSTimeDly(MS_10);
	ov7670RegW(0xa1, 0x03); OSTimeDly(MS_10);
	ov7670RegW(0xa6, 0xdf); OSTimeDly(MS_10);
	ov7670RegW(0xa7, 0xdf); OSTimeDly(MS_10);
	ov7670RegW(0xa8, 0xf0); OSTimeDly(MS_10);
	ov7670RegW(0xa9, 0x90); OSTimeDly(MS_10);
	ov7670RegW(0xaa, 0x94); OSTimeDly(MS_10);
	ov7670RegW(0x13, 0xe5); OSTimeDly(MS_10);

	ov7670RegW(0x0e, 0x61); OSTimeDly(MS_10);
	ov7670RegW(0x0f, 0x4b); OSTimeDly(MS_10);
	ov7670RegW(0x16, 0x02); OSTimeDly(MS_10);
	/* image orientation */
	ov7670RegW(0x1e, 0x07); OSTimeDly(MS_10);
	ov7670RegW(0x21, 0x02); OSTimeDly(MS_10);
	ov7670RegW(0x22, 0x91); OSTimeDly(MS_10);
	ov7670RegW(0x29, 0x07); OSTimeDly(MS_10);
	ov7670RegW(0x33, 0x0b); OSTimeDly(MS_10);
	ov7670RegW(0x35, 0x0b); OSTimeDly(MS_10);
	ov7670RegW(0x37, 0x1d); OSTimeDly(MS_10);
	ov7670RegW(0x38, 0x71); OSTimeDly(MS_10);
	ov7670RegW(0x39, 0x2a); OSTimeDly(MS_10);
	ov7670RegW(0x3c, 0x78); OSTimeDly(MS_10);
	ov7670RegW(0x4d, 0x40); OSTimeDly(MS_10);
	ov7670RegW(0x4e, 0x20); OSTimeDly(MS_10);
	ov7670RegW(0x69, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x6b, 0x60); OSTimeDly(MS_10);
	ov7670RegW(0x74, 0x19); OSTimeDly(MS_10);
	ov7670RegW(0x8d, 0x4f); OSTimeDly(MS_10);
	ov7670RegW(0x8e, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x8f, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x90, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x91, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x92, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x96, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x9a, 0x80); OSTimeDly(MS_10);
	ov7670RegW(0xb0, 0x84); OSTimeDly(MS_10);
	ov7670RegW(0xb1, 0x0c); OSTimeDly(MS_10);
	ov7670RegW(0xb2, 0x0e); OSTimeDly(MS_10);
	ov7670RegW(0xb3, 0x82); OSTimeDly(MS_10);
	ov7670RegW(0xb8, 0x0a); OSTimeDly(MS_10);

	ov7670RegW(0x43, 0x14); OSTimeDly(MS_10);
	ov7670RegW(0x44, 0xf0); OSTimeDly(MS_10);
	ov7670RegW(0x45, 0x34); OSTimeDly(MS_10);
	ov7670RegW(0x46, 0x58); OSTimeDly(MS_10);
	ov7670RegW(0x47, 0x28); OSTimeDly(MS_10);
	ov7670RegW(0x48, 0x3a); OSTimeDly(MS_10);
	ov7670RegW(0x59, 0x88); OSTimeDly(MS_10);
	ov7670RegW(0x5a, 0x88); OSTimeDly(MS_10);
	ov7670RegW(0x5b, 0x44); OSTimeDly(MS_10);
	ov7670RegW(0x5c, 0x67); OSTimeDly(MS_10);
	ov7670RegW(0x5d, 0x49); OSTimeDly(MS_10);
	ov7670RegW(0x5e, 0x0e); OSTimeDly(MS_10);
	ov7670RegW(0x64, 0x04); OSTimeDly(MS_10);
	ov7670RegW(0x65, 0x20); OSTimeDly(MS_10);
	ov7670RegW(0x66, 0x05); OSTimeDly(MS_10);
	ov7670RegW(0x94, 0x04); OSTimeDly(MS_10);
	ov7670RegW(0x95, 0x08); OSTimeDly(MS_10);
	ov7670RegW(0x6c, 0x0a); OSTimeDly(MS_10);
	ov7670RegW(0x6d, 0x55); OSTimeDly(MS_10);
	ov7670RegW(0x6e, 0x11); OSTimeDly(MS_10);
	ov7670RegW(0x6f, 0x9f); OSTimeDly(MS_10);
	ov7670RegW(0x6a, 0x40); OSTimeDly(MS_10);
	ov7670RegW(0x01, 0x40); OSTimeDly(MS_10);
	ov7670RegW(0x02, 0x40); OSTimeDly(MS_10);
	ov7670RegW(0x13, 0xe7); OSTimeDly(MS_10);
	/* PCLK does not toggle during horizontal blank */
	ov7670RegW(0x15, 0x20); OSTimeDly(MS_10);

	ov7670RegW(0x4f, 0x80); OSTimeDly(MS_10);
	ov7670RegW(0x50, 0x80); OSTimeDly(MS_10);
	ov7670RegW(0x51, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x52, 0x22); OSTimeDly(MS_10);
	ov7670RegW(0x53, 0x5e); OSTimeDly(MS_10);
	ov7670RegW(0x54, 0x80); OSTimeDly(MS_10);
	ov7670RegW(0x58, 0x9e); OSTimeDly(MS_10);

	ov7670RegW(0x41, 0x08); OSTimeDly(MS_10);
	ov7670RegW(0x3f, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x75, 0x05); OSTimeDly(MS_10);
	ov7670RegW(0x76, 0xe1); OSTimeDly(MS_10);
	ov7670RegW(0x4c, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x77, 0x01); OSTimeDly(MS_10);
	ov7670RegW(0x3d, 0xc2); OSTimeDly(MS_10);
	ov7670RegW(0x4b, 0x09); OSTimeDly(MS_10);
	ov7670RegW(0xc9, 0x60); OSTimeDly(MS_10);
	ov7670RegW(0x41, 0x38); OSTimeDly(MS_10);
	ov7670RegW(0x56, 0x40); OSTimeDly(MS_10);

	ov7670RegW(0x34, 0x11); OSTimeDly(MS_10);
	ov7670RegW(0x3b, 0x02); OSTimeDly(MS_10);

	ov7670RegW(0xa4, 0x89); OSTimeDly(MS_10);
	ov7670RegW(0x96, 0x00); OSTimeDly(MS_10);
	ov7670RegW(0x97, 0x30); OSTimeDly(MS_10);
	ov7670RegW(0x98, 0x20); OSTimeDly(MS_10);
	ov7670RegW(0x99, 0x30); OSTimeDly(MS_10);
	ov7670RegW(0x9a, 0x84); OSTimeDly(MS_10);
	ov7670RegW(0x9b, 0x29); OSTimeDly(MS_10);
	ov7670RegW(0x9c, 0x03); OSTimeDly(MS_10);
	ov7670RegW(0x9d, 0x4c); OSTimeDly(MS_10);
	ov7670RegW(0x9e, 0x3f); OSTimeDly(MS_10);
	ov7670RegW(0x78, 0x04); OSTimeDly(MS_10);
}

void ov7670Enable(void)
{
	ov7670Enabled = TRUE;
	ov7670IRQClear();
	ov7670IRQEnable();
}

INT8U ov7670Disable(void)
{
	INT8U err;

	ov7670Enabled = FALSE;
	OSSemPend(ov7670Disabled, OV7670_TIMEOUT, &err);
	return err;
}

static void ov7670Handler(void)
{
	if (ov7670Enabled == FALSE) {
		ov7670IRQDisable();
		OSSemPost(ov7670Disabled);
	} else {
		ov7670HandlerPtr();
	}
}
