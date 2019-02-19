/*
 * drv_rgb.c
 *
 *  Created on: 2018年10月14日
 *      Author: xubov
 */

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#include "drv_rgb.h"
#include "AppHardwareApi.h"
#include "hw_timer.h"
#include "delay.h"
#include "dbg.h"

#define HI 256
uint16 groupLevel = HI;
uint8 red_level;
uint8 grn_level;
uint8 blu_level;

PRIVATE void vTimer1ISR(uint32 u32DeviceId, uint32 u32ItemBitmap)
{
	DBG_vPrintf(TRUE, "\r\nvTimer1ISR.\r\n");
}
PRIVATE void vTimer2ISR(uint32 u32DeviceId, uint32 u32ItemBitmap)
{
	DBG_vPrintf(TRUE, "\r\nvTimer2ISR.\r\n");
}
PRIVATE void vTimer3ISR(uint32 u32DeviceId, uint32 u32ItemBitmap)
{
	DBG_vPrintf(TRUE, "\r\nvTimer3ISR.\r\n");
}

// not use
PUBLIC bool_t bRGB_LED_Enable(void)
{
	pwmEn(1);
	pwmEn(2);
	pwmEn(3);
	return TRUE;
}

// not use
PUBLIC bool_t bRGB_LED_Disable(void)
{
	return TRUE;
}

PUBLIC bool_t bRGB_LED_SetLevel(uint8 u8RedLevel, uint8 u8GreenLevel, uint8 u8BlueLevel)
{
	red_level = u8RedLevel;
	grn_level = u8GreenLevel;
	blu_level = u8BlueLevel;

	bRGB_LED_Enable();
	bRGB_LED_Off();
	bRGB_LED_On();

	return TRUE;
}
PUBLIC bool_t bRGB_LED_On(void)
{
	pwmSet(1, groupLevel-red_level, groupLevel);
	pwmSet(2, groupLevel-grn_level, groupLevel);
	pwmSet(3, groupLevel-blu_level, groupLevel);
	return TRUE;
}
PUBLIC bool_t bRGB_LED_Off(void)
{
	pwmStop(1);
	pwmStop(2);
	pwmStop(3);
	return TRUE;
}
PUBLIC bool_t bRGB_LED_SetGroupLevel(uint8 u8Level)
{
	groupLevel = u8Level+HI;
	return TRUE;
}

PUBLIC bool_t bRGB_Init(void)
{
	vAHI_Timer1RegisterCallback((void*)vTimer1ISR);
	vAHI_Timer2RegisterCallback((void*)vTimer2ISR);
	vAHI_Timer3RegisterCallback((void*)vTimer3ISR);
	initPwm();

	return TRUE;
}

void rgb_test()
{
	DBG_vPrintf(TRUE, "\r\nbRGB_Init.\r\n");
	bRGB_Init();
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel CLOSE.\r\n");
	bRGB_LED_SetLevel(0,0,0);
	delay_xms(500);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 0.\r\n");
	bRGB_LED_SetLevel(1,1,1);
	delay_xms(500);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 1.\r\n");
	bRGB_LED_SetLevel(50,50,50);
	delay_xms(500);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 2.\r\n");
	bRGB_LED_SetLevel(100,100,100);
	delay_xms(500);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 2.1.\r\n");
	bRGB_LED_SetLevel(120,120,120);
	delay_xms(500);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 3.\r\n");
	bRGB_LED_SetLevel(200,200,200);
	delay_xms(500);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 4 RED.\r\n");
	bRGB_LED_SetLevel(0,0,255);
	delay_xms(1000);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 5 GREEN.\r\n");
	bRGB_LED_SetLevel(0,255,0);
	delay_xms(1000);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 6 BLUE.\r\n");
	bRGB_LED_SetLevel(255,0,0);
	delay_xms(1000);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 7 B*G.\r\n");
	bRGB_LED_SetLevel(255,255,0);
	delay_xms(1000);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 7 R*B.\r\n");
	bRGB_LED_SetLevel(255,0,255);
	delay_xms(1000);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 8 R*G.\r\n");
	bRGB_LED_SetLevel(0,255,255);
	delay_xms(1000);
	DBG_vPrintf(TRUE, "\r\nbRGB_LED_SetLevel 7 WHITE.\r\n");
	bRGB_LED_SetLevel(255,255,255);
	delay_xms(1000);
}
