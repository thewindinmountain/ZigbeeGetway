/*
 * pan_crd_esl.c
 *
 *  Created on: 2017年12月27日
 *      Author: xubov
 */
#include <jendefs.h>
#include "dbg.h"
#include <AppHardwareApi.h>
#include <Appapi_JN516X.h>
#include <AppQueueApi.h>
#include "hw_config.h"

#include "eeprom.h"
#include "usr_timer.h"
#include "usr_Uart.h"
#include "usr_timer.h"

//#include "sleep.h"
#include "config.h"
//#include "button.h"

//#include "app_key.h"
#include "app_led.h"
#include "app_adc.h"
#include "app_flash.h"
#include "app_childTable.h"

#include "crd_netGate.h"
#include "delay.h"
#include "setUpCmd.h"
#include "string.h"
#include "error.h"
#include "msg.h"
#include "at_command.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define DEBUG_START_UP
#ifndef DEBUG_START_UP
    #define TRACE_PAN_CRD_ESL FALSE
#else
    #define TRACE_PAN_CRD_ESL TRUE
#endif

void usrUartRxLoop(void);
void dbgUartRxLoop(void);

PUBLIC void vUsrPanCrdInit(void)
{
	DBG_vPrintf(TRACE_PAN_CRD_ESL, VERSION);
	DBG_vPrintf(TRACE_PAN_CRD_ESL, PAN_DEV);
	delay_xms(100);

	NODE_sData.u16MaxChildren = MAX_CHLDR;
	NODE_sData.u16MaxDeepth   = MAX_DEEPTH;
	NODE_sData.u16MaxRouter   = MAX_ROUTER;
	NODE_sData.u16NodeDeepth  = 0;

//  //not useful
//	eAppApiPlmeGet(PHY_PIB_ATTR_TX_POWER, &txPower); //-32,-20,-9,0dBm
//	DBG_vPrintf(TRACE_PAN_CRD_ESL, "\r\n%d gate_Init read txPower= %d dBm", NODE_sData.u32Timer, txPower);
//	txPower = -32;
//	eAppApiPlmeSet(PHY_PIB_ATTR_TX_POWER, txPower);
//	DBG_vPrintf(TRACE_PAN_CRD_ESL, "\r\n%d gate_Init reset txPower= %d dBm", NODE_sData.u32Timer, txPower);

    EEPROM_init();
//	/* Read network message*/
	EEPROM_readNodeParm(&NODE_sData);
//    /* Initialise coordinator data */
	u16ChildTableReload();

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
	DBG_vPrintf(TRACE_PAN_CRD_ESL, "\r\n CRD < Sht=%#x Pan=%#x", NODE_sData.u16Address, NODE_sData.u16PanId);

	usrTimerInit();
	app_led_start_blink_1s();
	appFlashInit();
	SysParmReload();
	vDbgPortDataHandle();
}


PUBLIC void vPanCrdEslHwIndHandle(AppQApiHwInd_s *psAHI_Ind)
{
	switch(psAHI_Ind->u32DeviceId)
	{
		/* Tick timer ? */
		case E_AHI_DEVICE_TICK_TIMER:
		{
			usrTimerLoop();

			usrUartRxLoop();

			dbgUartRxLoop();
		}break;
//		/* System Ctrl ? */
//		case E_AHI_DEVICE_SYSCTRL:
//		{
//			vISR_SystemController(psAHI_Ind->u32DeviceId, psAHI_Ind->u32ItemBitmap);
//		}break;
		default: break;
	}
}

PUBLIC void vPanCrdIndAssociate(MAC_MlmeDcfmInd_s *psMlmeInd)
{
	crd_vIndAssociate(psMlmeInd);
}


// find the xTh 1 position of data.
// 0x07FFF800UL offset: 11-26
//PUBLIC uint8 getOnePosition(uint32 data, uint8 num)
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

#define RX_BUF_SIZE 2048
void usrUartRxLoop(void)
{
	uint16 rxLen;
	char rxBuf[RX_BUF_SIZE];

	if(ERR_OK == uart_read_sync(rxBuf, RX_BUF_SIZE, &rxLen))
	{
		//uartMsgHandle(rxBuf, rxLen);
		exec_request((uint8*)rxBuf);
	}
}

void dbgUartRxLoop(void)
{
	uint16 rxLen;
	char rxBuf[RX_BUF_SIZE];

	dbg_uart_read_sync(rxBuf, &rxLen);

	if(rxLen > 0)
	{
		exec_request(rxBuf);
//		uartMsgHandle(rxBuf, rxLen);
	}
}

