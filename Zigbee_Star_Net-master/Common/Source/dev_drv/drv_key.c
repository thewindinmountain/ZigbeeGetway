/*
 * drv_key.c
 *
 *  Created on: 2017锟斤拷9锟斤拷28锟斤拷
 *      Author: xubov
 */

#include "hw_gpio.h"
#include "drv_Type.h"
#include "hw_config.h"

void drvKeyInit(int32 mask1, int32 mask2)
{
	cfgGpioInputWithIrq(KEY1_MASK | KEY2_MASK, FALLING_EDGE);
}

//
void drvKeyRead(int32 mask, int32 *value)
{
	getGpio((uint32)mask, (uint32*)value);
}

ioDrvStruct drvKey =
{
	.open = drvKeyInit,
	.read = drvKeyRead,
};
