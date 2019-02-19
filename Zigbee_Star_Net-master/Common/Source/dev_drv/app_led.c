/*
 * app_led.c
 *
 *  Created on: 2017年12月5日
 *      Author: xubov
 */
#include "drv_led.h"
#include "hw_config.h"
#include "delay.h"
#include "usr_timer.h"
#include "dbg.h"

uint8 initStatus = 0;
uint8 usrTimerIdLed = 0;
int32 ledBlinkMask = 0;


#ifdef DEBUG_APP_LED
#define APP_LED_TRACE TRUE
#else
#define APP_LED_TRACE FALSE
#endif

void app_led_blink(void);

void app_led_init(void)
{
	if(initStatus != 1)
	{
		drvLed.open(0,0);
		initStatus = 1;
		usrTimerOpen(&usrTimerIdLed,app_led_blink);
		DBG_vPrintf(APP_LED_TRACE, "\r\n app_led_init timer id = %d", usrTimerIdLed);
	}
}

void app_led_set(int32 mask, int32 value)
{
	app_led_init();
	drvLed.write(mask, &value);
}

void app_led_blink(void)
{
	static int32 status = 0;

	app_led_set(ledBlinkMask & LED_ALL_MASK, status);

	if(status == 0)
	{
		status = 1;
	}else
	{
		status = 0;
	}
}


void app_led_set_all(bool value)
{
	int32 status = 0;

	app_led_init();

	if(value)
	{
		status = 1;
		drvLed.write(LED_ALL_MASK, &status);
	}else
	{
		status = 0;
		drvLed.write(LED_ALL_MASK, &status);
	}
}

void app_led_blink_set(int32 mask, int32 msHalfPeriod)
{
	app_led_init();

	ledBlinkMask = mask & LED_ALL_MASK;

	usrTimerStart(&usrTimerIdLed, msHalfPeriod);
}

void app_led_blink_reset(void)
{
	int32 status = 1;

	usrTimerStop(&usrTimerIdLed);
	app_led_set(ledBlinkMask & LED_ALL_MASK, status); //set all blink led to closed
}

void app_led_start_blink_1s(void)
{
	app_led_set(LED_ALL_MASK, 1);
	app_led_blink_set(LED1_MASK, 100);/* 1s */
}
