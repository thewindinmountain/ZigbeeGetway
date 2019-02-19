/*
 * drv_adc.c
 *
 *  Created on: 2017年10月3日
 *      Author: xubov
 */
#include "jendefs.h"
#include "AppHardwareApi.h"
#include "drv_type.h"
#include "hw_config.h"
#include "dbg.h"

#define DRV_ADC_TRACE TRUE

/*
 * VREF | GAIN | MaxIn | VddRange
 * 1.2V |  0   | 1.2V  | 2.2-3.6V
 * 1.6V |  0   | 1.6V  | 2.2-3.6V
 * 1.2V |  1   | 2.4V  | 2.6-3.6V
 * 1.6V |  1   | 3.2V  | 3.4-3.6V
 *
 * VREF Internal: 1.2V @ VDD=2.0-3.6V
 * 10bits  1024
 *
 * Voltage to ADC = 0.666*VDD
 *
 */

void drvAdcInit(int32 mask1, int32 mask2)
{
	/* Init ADC */
	vAHI_ApConfigure(E_AHI_AP_REGULATOR_ENABLE,
					 E_AHI_AP_INT_DISABLE,
					 E_AHI_AP_SAMPLE_8,
					 E_AHI_AP_CLOCKDIV_500KHZ,
					 E_AHI_AP_INTREF);

	if(bAHI_APRegulatorEnabled())
	{
		DBG_vPrintf(DRV_ADC_TRACE, "\r\nDRV_ADC init success.");
	}else
	{
		DBG_vPrintf(DRV_ADC_TRACE, "\r\nDRV_ADC init err.");
	}
}

void drvAdcRead(int32 mask, int32 *vol)
{
	bool_t      bInputRange;
	uint8       u8Source;
//	uint16      u16pollCont = 0xFFF;

	if( ADC_GAIN == 2)
	{
		bInputRange = E_AHI_AP_INPUT_RANGE_2;
	}
	else
	{
		bInputRange = E_AHI_AP_INPUT_RANGE_1;
	}

	switch(mask)
	{
		case ADC_MASK_VDD:  u8Source = E_AHI_ADC_SRC_VOLT; break;
		case ADC_MASK_TEMP: u8Source = E_AHI_ADC_SRC_TEMP; break;
		default: *vol = 0; vAHI_AdcDisable(); return;
	}

	if(bAHI_APRegulatorEnabled())
	{
		vAHI_AdcEnable(E_AHI_ADC_SINGLE_SHOT,
					   bInputRange,
					   u8Source);

		vAHI_AdcStartSample();

		while(bAHI_AdcPoll());
		*vol = (int32)(u16AHI_AdcRead() * ADC_GAIN * 1200 / 1024 );
	}

	vAHI_AdcDisable();
}

ioDrvStruct drvAdc =
{
	.open = drvAdcInit,
	.read = drvAdcRead,
};



