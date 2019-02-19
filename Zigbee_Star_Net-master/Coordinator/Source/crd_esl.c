/*
 *  crd_esl.c
 *
 *  Created on: 2017骞�0鏈�鏃�
 *      Author: xubov
 */

#include <jendefs.h>
#include "dbg.h"
#include <AppHardwareApi.h>
#include <Appapi_JN516X.h>
#include <AppQueueApi.h>
#include <string.h>
#include "hw_config.h"

#include "usr_timer.h"
#include "eeprom.h"
//#include "cmd.h"
#include "usr_uart.h"
#include "app_adc.h"
#include "app_flash.h"
#include "hw_config.h"
#include "button.h"
#include "app_key.h"
#include "app_led.h"
#include "usr_timer.h"
#include "app_childTable.h"
#include "crd_netGate.h"
#include "delay.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define DEBUG_START_UP
#ifndef DEBUG_START_UP
    #define TRACE_CRD_ESL FALSE
#else
    #define TRACE_CRD_ESL TRUE
#endif

extern uint8 u8CrdEn;

uint8 dataReqTimerId = 0;

void printfNode_sData(NODE_tsData *NODE_sData)
{
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->u64Address %#x:%#x", NODE_sData->u64Address >> 32, NODE_sData->u64Address);
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->u64Parent %#x:%#x", NODE_sData->u64Parent >> 32, NODE_sData->u64Parent);
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->u64ExtendId %#x:%#x", NODE_sData->u64ExtendId >> 32, NODE_sData->u64ExtendId);
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->u32Timer %d", NODE_sData->u32Timer);
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->u32TimerSync %d", NODE_sData->u32TimerSync);
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->u16Address %#x", NODE_sData->u16Address);
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->u16Parent %#x", NODE_sData->u16Parent);
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->u16PanId %#x", NODE_sData->u16PanId);
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->u8Channel %d", NODE_sData->u8Channel);
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->eNwkState %d", NODE_sData->eNwkState);
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n NODE_sData->u8Channel %d", NODE_sData->u8Channel);
}

void crd_esl_Init(void)
{
	DBG_vPrintf(TRACE_CRD_ESL, VERSION);
	DBG_vPrintf(TRACE_CRD_ESL, CRD_DEV);
	delay_xms(100);

	NODE_sData.u16MaxChildren = MAX_CHLDR;
	NODE_sData.u16MaxDeepth   = MAX_DEEPTH;
	NODE_sData.u16MaxRouter   = MAX_ROUTER;
	NODE_sData.u16NodeDeepth  = 0;

//	eAppApiPlmeGet(PHY_PIB_ATTR_TX_POWER, &txPower); //-32,-20,-9,0dBm
//	DBG_vPrintf(TRACE_GATE, "\r\n%d gate_Init read txPower= %d dBm", NODE_sData.u32Timer, txPower);
//	txPower = -32;
//	eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, txPower);
//	DBG_vPrintf(TRACE_GATE, "\r\n%d gate_Init reset txPower= %d dBm", NODE_sData.u32Timer, txPower);

    EEPROM_init();
    bChildTableDeleteAll();
//	/* Read network message*/
//	EEPROM_readNodeParm(&NODE_sData);
//    /* Initialise coordinator data */
//	u16ChildTableReload();

	/* Initialise user uart*/
	vUsrUartInit(USR_UART, USR_UART_BAUD);

#ifdef REMOTE_CTRL
	vSleepKeepTimeSet(CRD_NORMAL_SLEEP);
	if(NODE_sData.eNwkState == NODE_NWKSTATE_UP)
	{
		vSleepAwakeTimeSet(KEEP_AWAKE_SHORT_TIMER);
	}else
	{
		vSleepAwakeTimeSet(KEEP_AWAKE_LONG_TIMER);
	}
#endif

	/* Debug */
	DBG_vPrintf(TRACE_CRD_ESL, "\r\n CRD < Sht=%#x Pan=%#x", NODE_sData.u16Address, NODE_sData.u16PanId);

	usrTimerInit();

	app_led_test();
}

PUBLIC void vCrdEslHwIndHandle(AppQApiHwInd_s *psAHI_Ind)
{
	switch(psAHI_Ind->u32DeviceId)
	{
		/* Tick timer ? */
		case E_AHI_DEVICE_TICK_TIMER:
		{
			usrTimerLoop();
			if(u8CrdEn == 1)
			{
				crd_vReqStartCoordinator(0);
				u8CrdEn = 0;
			}
		}break;
		default: break;
	}
}

void crd_esl_vMlmeDcfmInd(MAC_MlmeDcfmInd_s *psMlmeInd)
{
	switch(psMlmeInd->u8Type)
	{
		case MAC_MLME_DCFM_POLL:
		{
			DBG_vPrintf(TRACE_CRD_ESL, "\r\n%d poll u8Status = %d",
					NODE_sData.u32Timer,
					psMlmeInd->uParam.sDcfmPoll.u8Status);
//			MAC_ENUM_SUCCESS
		}break;
	default:
		DBG_vPrintf(TRACE_CRD_ESL, "\r\n%d ed_crd_esl_vMlmeDcfmInd get cfm type %d", NODE_sData.u32Timer, psMlmeInd->u8Type);
	}
}

PUBLIC void crd_esl_deepthReq(void)
{
	uint8 data[] = "reqDeepth:req";
	if(NODE_sData.eNwkState == NODE_NWKSTATE_UP)
	{
		NODE_vTx(NODE_sData.u16Parent, strlen("reqDeepth:req"), data);
	}
}
