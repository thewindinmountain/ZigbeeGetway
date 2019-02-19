/*
 * drv_led.c
 *
 *  Created on: 2017楠烇拷閺堬拷7閺冿拷
 *      Author: xubov
 */
#include "hw_gpio.h"
#include "drv_Type.h"
#include "hw_config.h"

void drvLedInit(int32 mask1, int32 mask2)
{
	cfgGpioOutput(LED_ALL_MASK);
}

// value: 0-off, 1-on
void drvLedWrite(int32 mask, int32 *status)
{
	(*status == 0) ? (setGpio(0, (uint32)mask)) : (setGpio((uint32)mask, 0));
}

ioDrvStruct drvLed =
{
	.open  = drvLedInit,
	.write = drvLedWrite,
};

