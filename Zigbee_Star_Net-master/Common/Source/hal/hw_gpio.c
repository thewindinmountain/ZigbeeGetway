/*
 * hw_Gpio.c
 *
 *  Created on: 2017骞�鏈�6鏃�
 *      Author: xubov
 */

#include "AppHardwareApi.h"
#include "PlatformDIOAssignments.h"
#include "jendefs.h"
#include "hw_config.h"
#include "drv_Type.h"
 
void initGpio(uint32 input, uint32 output)
{
	if(output&(0x03<<20))
	{
		bAHI_DoEnableOutputs(TRUE);
	}

	vAHI_DioSetDirection(input&0xFFFFF, output&0xFFFFF);
}

void cfgGpioOutput(uint32 mask)
{
	vAHI_DioSetDirection(0, mask);
}

void cfgGpioInput(uint32 mask)
{
	vAHI_DioSetDirection(mask, 0);
}

//type: 0-Irq Disable; 1-rising edge, 2-falling  edge; 3-rising or falling  edge
void cfgGpioInputWithIrq(uint32 mask, char irqType)
{
	cfgGpioInput(mask);
    // IO Trig wakeup Enable.
	switch(irqType)
	{
		case 0: break;
		case RISING_EDGE : vAHI_DioInterruptEdge(0   , mask); vAHI_DioInterruptEnable(mask, 0); break;
		case FALLING_EDGE: vAHI_DioInterruptEdge(mask, 0   ); vAHI_DioInterruptEnable(mask, 0); break;
		case BOTH_EDGE   : vAHI_DioInterruptEdge(mask, mask); vAHI_DioInterruptEnable(mask, 0); break;
		default: break;
	}
}

void setGpioPull(uint32 on, uint32 off)
{
	vAHI_DioSetPullup(on, off);
}

void setGpio(uint32 highMask, uint32 lowMask)
{
	vAHI_DoSetDataOut((uint8)(highMask>>20), (uint8)(lowMask>>20));
	vAHI_DioSetOutput(highMask&0xFFFFF, lowMask&0xFFFFF);
}

// void setDO(uint32 onMask, uint32 offMask)
// {
// 	// bit0 for DO0; bit1 for DO1,
// 	vAHI_DoSetDataOut((uint8)(onMask>>20), (uint8)(offMask>>20));
// }

void getGpio(uint32 mask, uint32 *value)
{
	*value = (u32AHI_DioReadInput() & mask) ^ mask;
}

uint32  u32GetGpio(uint32 mask)
{
	return (u32AHI_DioReadInput() & mask);// ^ mask;
}

void drvGpioInit(int32 in,int32 out)
{
	initGpio(in, out);
}

void drvGpioWrite(int32 mask, int32 *status)
{
	(*status == 0) ? (setGpio(0, (uint32)mask)) : (setGpio((uint32)mask, 0));
}

void drvGpioRead(int32 mask, int32 *value)
{
	*value = (u32AHI_DioReadInput() & mask) ^ mask;
}

ioDrvStruct drvGpio =
{
	.open  = drvGpioInit,
	.write = drvGpioWrite,
	.read  = drvGpioRead
};





