/*
 * app_adc.c
 *
 *  Created on: 2017年10月3日
 *      Author: xubov
 */
#include "jendefs.h"
#include "AppHardwareApi.h"
#include "hw_config.h"
#include "drv_adc.h"
#include "drv_type.h"
#include "dbg.h"
#include "usr_Timer.h"
#include "string.h"
#include "app_adc.h"
#include "node.h"
#include "sjsonApp.h"
#include "string.h"
#include "msg.h"

#ifndef DEBUG_APP_ADC
    #define APP_ADC_TRACE FALSE
#else
    #define APP_ADC_TRACE TRUE
#endif


uint8 adcInitStatus = 0;
uint8 usrTimerIdAdc = 0;

void txVdd(void);

void adcOpen(void)
{
	if(adcInitStatus != 1)
	{
		drvAdc.open(0,0);
		usrTimerOpen(&usrTimerIdAdc, txVdd);
		adcInitStatus = 1;
	}
}

void txVdd(void)
{
	int32 vdd;
	char dataBuf[100];
	uint16 dataLen = 0;

	vdd = i32AppGetVdd();

	if(NODE_sData.eNwkState == NODE_NWKSTATE_UP)
	{
		dataLen = sjson_PktVoltage(vdd, &(dataBuf[0]), &(dataBuf[99]));
		NODE_vTx((uint64)NODE_sData.u16Parent, dataLen, (uint8*)dataBuf,JSON_PKT);
	}
}

int32 i32AppGetVdd(void)
{
	int32 value = 0;

	adcOpen();

	drvAdc.read(ADC_MASK_VDD, &value);

	value = (int32)(value / 0.666);
	DBG_vPrintf(APP_ADC_TRACE, "\r\ni32AppGetVDD = %d mV", value);
	return value;
}

int16 i16AppGetTemp(void)
{
	int32 value = 0;
	int16 temp;

	adcOpen();

	drvAdc.read(ADC_MASK_TEMP, &value);

	/*
	 * 730mV ---- 25C
	 * temp = (value - 730)/1.66 + 25
	 */
	temp = (int16)(value -730)/1.66 + 25;
	if(temp > 0)
	{
		DBG_vPrintf(APP_ADC_TRACE, "\r\ni32AppGetTemp = %dC", temp);
	}else
	{
		DBG_vPrintf(APP_ADC_TRACE, "\r\ni32AppGetTemp = -%dC", 0-temp);
	}

	return temp;
}

void app_adc_start_loop_per10s(void)
{
	adcOpen();
	usrTimerStart(&usrTimerIdAdc, 1000);/* 10s */
}
