/*
 * ed_esl.c
 *
 *  Created on: 2017闁跨喐鏋婚幏锟�0闁跨喐鏋婚幏锟介柨鐔告灮閹凤拷
 *      Author: xubov
 */

#include <jendefs.h>
#include "dbg.h"
#include <AppHardwareApi.h>
#include <Appapi_JN516X.h>
#include <AppQueueApi.h>
#include "hw_config.h"
#include <string.h>

#include "ed_esl.h"
#include "error.h"
//#include "appHwIndHandle.h"
#include "usr_timer.h"
#include "eeprom.h"
#include "app_adc.h"
#include "app_flash.h"
#include "sleep.h"
//#include "app_key.h"
#include "app_led.h"
#include "usr_timer.h"
#include "app_childTable.h"
#include "delay.h"
#include "ed_node_Simulate.h"
#include "sjson_app.h"
#include "drv_key.h"

//#include "EPD_SSD1673_test.h"
#include "epd_api.h"
#include "drv_rgb.h"
#include "msg.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define DEBUG_START_UP
#ifndef DEBUG_START_UP
    #define TRACE_ESL FALSE
#else
    #define TRACE_ESL TRUE
#endif

uint8 dataReqTimerId = 0;

PUBLIC void printfNode_sData(NODE_tsData *NODE_sData)
{
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->u64Address %#x:%#x", NODE_sData->u64Address >> 32, NODE_sData->u64Address);
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->u64Parent %#x:%#x", NODE_sData->u64Parent >> 32, NODE_sData->u64Parent);
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->u64ExtendId %#x:%#x", NODE_sData->u64ExtendId >> 32, NODE_sData->u64ExtendId);
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->u32Timer %d", NODE_sData->u32Timer);
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->u32TimerSync %d", NODE_sData->u32TimerSync);
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->u16Address %#x", NODE_sData->u16Address);
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->u16Parent %#x", NODE_sData->u16Parent);
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->u16PanId %#x", NODE_sData->u16PanId);
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->u8Channel %d", NODE_sData->u8Channel);
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->eNwkState %d", NODE_sData->eNwkState);
	DBG_vPrintf(TRACE_ESL, "\r\n NODE_sData->u8Channel %d", NODE_sData->u8Channel);
}

PUBLIC void esl_Init(void)
{
	DBG_vPrintf(TRACE_ESL, VERSION);
	DBG_vPrintf(TRACE_ESL, ESL_DEV);
	delay_xms(100);

	EEPROM_readNodeParm(&NODE_sData); //it should follow EEPROM_init

	DBG_vPrintf(TRACE_ESL, "\r\n%d ED u64Id=%x:%x, panID = %#x, channel = %d",
			   NODE_sData.u32Timer,
			   (uint32)(NODE_sData.u64Address>>32),
			   (uint32)NODE_sData.u64Address,
			   NODE_sData.u16PanId,
			   NODE_sData.u8Channel);

#ifdef END_DEV_SIMULATE
	// reconfig dev 64bits ID.
	MAC_ExtAddr_s sExtAddr;
	sExtAddr.u32H = (uint32)(NODE_sData.u64Address>>32);
	sExtAddr.u32L = (uint32)NODE_sData.u64Address;
	MAC_vSetExtAddr(NODE_sData.pvMac, &sExtAddr);
#endif

	usrTimerInit();

	drvKey.open(0,0);

//	app_led_start_blink_1s();
	app_adc_start_loop_per10s();
	sleepSet();

	/* poll from parent nodes per  */
//	usrTimerOpen(&dataReqTimerId, ed_esl_usrPoll);
//	usrTimerStart(&dataReqTimerId, 10); /* period 500*10ms = 5s */

//	DBG_vPrintf(TRACE_ESL, "\r\nstart epd test.\r\n");
//	EPD_test();
//	epd_test();
//	DBG_vPrintf(TRACE_ESL, "\r\nepd test finished.\r\n");

//	DBG_vPrintf(TRACE_ESL, "\r\nRGB test.\r\n");
//	rgb_test();
}

uint8 pollDone= 1;
PUBLIC void esl_vHwIndHandle(AppQApiHwInd_s *psAHI_Ind)
{

	switch(psAHI_Ind->u32DeviceId)
	{
		/* Tick timer ? */
		case E_AHI_DEVICE_TICK_TIMER:
		{
			usrTimerLoop(); /* try per 10ms */

			if(NODE_sData.eNwkState == NODE_NWKSTATE_UP)
			{
				if(pollDone)
				{
					pollDone = 0;
					ed_esl_usrPoll();
				}

#ifdef END_DEV_SIMULATE
				nodeSimulate();/* simulate a lot nodes to add to the CRD net. */
#endif
			}else
			{
//				DBG_vPrintf(TRACE_ESL, "\r\n%d net state %d", NODE_sData.u32Timer, NODE_sData.eNwkState);
			}
		}break;
		/* System Ctrl ? */
		case E_AHI_DEVICE_SYSCTRL:
		{
		}break;
		default:
			DBG_vPrintf(TRACE_ESL, "\r\esl get psAHI_Ind->u32DeviceId=0x%x .\r\n", psAHI_Ind->u32DeviceId);
			break;
	}
	switch(psAHI_Ind->u32ItemBitmap)
	{
		case 1: break;
		default:
			DBG_vPrintf(TRACE_ESL, "\r\esl get psAHI_Ind->u32ItemBitmap=0x%x .\r\n", psAHI_Ind->u32ItemBitmap);
			break;
	}
}

PUBLIC void ed_esl_vMlmeDcfmInd(MAC_MlmeDcfmInd_s *psMlmeInd)
{
	DBG_vPrintf(TRACE_ESL, "\r\n%d ed_esl_vMlmeDcfmInd get cfm type %d", NODE_sData.u32Timer, psMlmeInd->u8Type);
}

/*{"tagID":123, "act":"poll","parm":"req"}*/
PUBLIC void ed_esl_usrPoll(void)
{
	char *pollCmd;

	return;
	if(pollMute())
		return;

	DBG_vPrintf(TRACE_ESL, "\r\n poll req...");
	if(NODE_sData.eNwkState == NODE_NWKSTATE_UP)
	{
		pollCmd = get_poll_json_cmd(NODE_sData.u64Address);
		NODE_vTx(NODE_sData.u16Parent, strlen(pollCmd), (uint8*)pollCmd, JSON_PKT);
	}
}

// find the xTh 1 position of data.
// 0x07FFF800UL offset: 11-26
PUBLIC uint8 getActiveCh(uint8 num)
{
	uint8 u8Bit = CHANNEL_MIN;
	uint8 count = 0;

	for(u8Bit = CHANNEL_MIN; u8Bit <= 26; u8Bit++)
	{
		if(SCAN_CHANNELS & (1 << u8Bit))
		{
			count += 1;
			if(count == num)break;
		}
	}

	if(count != num)
	{
		u8Bit = 0;
	}

	return u8Bit;
}

#define RX_BUF_SIZE 256
void usrUartRxLoop(void)
{
	uint16 rxLen;
	char rxBuf[RX_BUF_SIZE];

	if(ERR_OK == uart_read_sync(rxBuf, RX_BUF_SIZE, &rxLen))
	{
		uartMsgHandle(rxBuf, rxLen);
	}
}

void dbgUartRxLoop(void)
{
	uint16 rxLen;
	char rxBuf[RX_BUF_SIZE];

	dbg_uart_read_sync(rxBuf, &rxLen);

	if(rxLen > 0)
	{
		uartMsgHandle(rxBuf, rxLen);
	}
}
