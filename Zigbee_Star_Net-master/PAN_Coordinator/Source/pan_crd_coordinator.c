/****************************************************************************/
/*!
 *\MODULE              Coordinator (Application Layer)
 *
 *\COMPONENT           $HeadURL: https://www.collabnet.nxp.com/svn/lprf_apps/Application_Notes/JN-AN-1069-IEEE-802.15.4-Serial-Cable-Replacement/Trunk/Coordinator/Source/PAN_CRD_coordinator.c $
 *
 *\VERSION			   $Revision: 18318 $
 *
 *\REVISION            $Id: PAN_CRD_coordinator.c 18318 2017-02-09 11:36:46Z nxp29759 $
 *
 *\DATED               $Date: 2017-02-09 11:36:46 +0000 (Thu, 09 Feb 2017) $
 *
 *\AUTHOR              $Author: nxp29759 $
 *
 *\DESCRIPTION         Coordinator (Application Layer) - implementation.
 */
/*****************************************************************************
 *
 * This software is owned by Jennic and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on Jennic products. You, and any third parties must reproduce
 * the copyright and warranty notice and any other legend of ownership on each
 * copy or partial copy of the software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". JENNIC MAKES NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
 * ACCURACY OR LACK OF NEGLIGENCE. JENNIC SHALL NOT, IN ANY CIRCUMSTANCES,
 * BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER.
 *
 * Copyright Jennic Ltd 2009. All rights reserved
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
/* Jennic defines */
#include <jendefs.h>
#include <AppHardwareApi.h>
#include <AppQueueApi.h>
#include <LedControl.h>
#include <mac_sap.h>
#include <mac_pib.h>
/* Common defines */
#include "config.h"
#include "dbg.h"
#include "node.h"
#ifdef SWDEBUG
    #include <gdb.h>
#endif

#include "pan_crd_esl.h"
#include "msg.h"
#include "eeprom.h"
#include "common_init.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DEBUG_CRD
    #define PAN_CRD_TRACE FALSE
#else
    #define PAN_CRD_TRACE TRUE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
///* Data type for storing data related to all end devices that have associated */


/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void PAN_CRD_vMain(void);
PRIVATE void PAN_CRD_vInit(void);
PRIVATE void PAN_CRD_vMlmeDcfmInd(MAC_MlmeDcfmInd_s *psMlmeInd);
PRIVATE void PAN_CRD_vMcpsDcfmInd(MAC_McpsDcfmInd_s *psMcpsInd);
PRIVATE void PAN_CRD_vHwInd(AppQApiHwInd_s *psAHI_Ind);
PRIVATE void PAN_CRD_vIndData(MAC_McpsDcfmInd_s *psMcpsInd);
PRIVATE void PAN_CRD_vDcfmData(MAC_McpsDcfmInd_s *psMcpsInd);
PRIVATE void PAN_CRD_vReqEnergyScan(void);
PRIVATE void PAN_CRD_vDcfmScanEnergy(MAC_MlmeDcfmInd_s *psMlmeInd);
PRIVATE void PAN_CRD_vReqStartCoordinator(void);
//PRIVATE void PAN_CRD_vIndAssociate(MAC_MlmeDcfmInd_s *psMlmeInd);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
//PRIVATE PAN_CRD_tsData PAN_CRD_sData;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: AppColdStart
 *
 * DESCRIPTION:
 * Entry point for application from boot loader. Initialises system and runs
 * main loop.
 *
 * RETURNS:
 * Never returns.
 *
 ****************************************************************************/
PUBLIC void AppColdStart(void)
{
	/* Disable watchdog if enabled by default */
	#ifdef WATCHDOG_ENABLED
		vAHI_WatchdogStop();
	#endif

	/* Perform main processing */
	PAN_CRD_vMain();
}

/****************************************************************************
 *
 * NAME: AppWarmStart
 *
 * DESCRIPTION:
 * Entry point for application from boot loader. Simply jumps to AppColdStart
 * as, in this instance, application will never warm start.
 *
 * RETURNS:
 * Never returns.
 *
 ****************************************************************************/
PUBLIC void AppWarmStart(void)
{
	/* Disable watchdog if enabled by default */
	#ifdef WATCHDOG_ENABLED
		vAHI_WatchdogStop();
	#endif

	/* Perform main processing */
	PAN_CRD_vMain();
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: PAN_CRD_vMain
 *
 * DESCRIPTION:
 * Check each of the three event queues and process and items found.
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
PRIVATE void PAN_CRD_vMain(void)
{
    MAC_MlmeDcfmInd_s *psMlmeInd;
	MAC_McpsDcfmInd_s *psMcpsInd;
    AppQApiHwInd_s    *psAHI_Ind;

	/* Initialise */
    common_init();
    PAN_CRD_vInit();

    vUsrPanCrdInit();

    if(NODE_sData.eNwkState == NODE_NWKSTATE_UP)
	{
    	DBG_vPrintf(PAN_CRD_TRACE, "\r\n PAN_CRD < ReStartCrd.");
    	PAN_CRD_vReqStartCoordinator();
	}
    else
    {
    	DBG_vPrintf(PAN_CRD_TRACE, "\r\n PAN_CRD < ReEnergyScan.");
    	/* Start state machine by requesting an energy scan */
		PAN_CRD_vReqEnergyScan();
    }

	/* Start state machine by requesting an energy scan */
#if 0 /* not use */
    PAN_CRD_vReqEnergyScan();
#endif

	/* Loop processing events */
    while (1)
    {
		/* Check for anything on the MLME upward queue */
		do
		{
			psMlmeInd = psAppQApiReadMlmeInd();
			if (psMlmeInd != NULL)
			{
				PAN_CRD_vMlmeDcfmInd(psMlmeInd);
				vAppQApiReturnMlmeIndBuffer(psMlmeInd);
			}
		} while (psMlmeInd != NULL);

		/* Check for anything on the MCPS upward queue */
		do
		{
			psMcpsInd = psAppQApiReadMcpsInd();
			if (psMcpsInd != NULL)
			{
				PAN_CRD_vMcpsDcfmInd(psMcpsInd);
				vAppQApiReturnMcpsIndBuffer(psMcpsInd);
			}
		} while (psMcpsInd != NULL);

		/* Check for anything on the AHI upward queue */
		do
		{
			psAHI_Ind = psAppQApiReadHwInd();
			if (psAHI_Ind != NULL)
			{
				PAN_CRD_vHwInd(psAHI_Ind);
				vAppQApiReturnHwIndBuffer(psAHI_Ind);
			}
		} while (psAHI_Ind != NULL);
	}
}

/****************************************************************************
 *
 * NAME: PAN_CRD_vInit
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void PAN_CRD_vInit(void)
{
    /* Setup interface to MAC */
//    (void)u32AHI_Init();
//    (void)u32AppQApiInit(NULL, NULL, NULL);

	/* Initialise debugging */
	DBG_vUartInit(DBG_UART, DBG_BAUD);
	/* Debug */
	DBG_vPrintf(PAN_CRD_TRACE, "\r\n%d PAN_CRD < vInitSystem()", NODE_sData.u32Timer);

    /* Initialise coordinator data */
//    PAN_CRD_sData.u16Children = 0;

	/* Initialise common 15.4 node data */
	NODE_vInit();

	/* Initialise the random number generator */
//	RND_vInit();

	/* We are the co-ordinator so choose our own short address */
	NODE_sData.u16Address = (uint16) (NODE_sData.u64Address & 0xffff);
	/* We are the co-ordinator so choose our own pan id which is fixed */
	NODE_sData.u16PanId = PAN_ID;
	MAC_ExtAddr_s sExtAddr;
	sExtAddr.u32L = (uint32)(NODE_sData.u64Address & 0xFFFFFFFF);
	sExtAddr.u32H = (uint32)(NODE_sData.u64Address>>32);

	/* Set short address in pib */
    MAC_vPibSetShortAddr(NODE_sData.pvMac, NODE_sData.u16Address);
    MAC_vSetExtAddr(NODE_sData.pvMac, &sExtAddr);
	/* Set pan id in pib */
    MAC_vPibSetPanId(NODE_sData.pvMac, NODE_sData.u16PanId);
    /* Enable receiver to be on when idle */
    MAC_vPibSetRxOnWhenIdle(NODE_sData.pvMac, TRUE, FALSE );
    /* Allow nodes to associate */
    NODE_sData.psPib->bAssociationPermit = 1;

	/* Debug */
	DBG_vPrintf(PAN_CRD_TRACE, " Sht=%#x Pan=%#x", NODE_sData.u16Address, NODE_sData.u16PanId);

//	/* Initialise wuart module */
//	WUART_vInit();

	/* Start tick timer 10ms repeating */
	vAHI_TickTimerInterval(10 * TICK_TIMER_MS);
#ifdef JENNIC_CHIP_FAMILY_JN517x
	vAHI_TickTimerClear();
#else
    vAHI_TickTimerWrite(0);
#endif

	vAHI_TickTimerIntEnable(TRUE);
	vAHI_TickTimerConfigure(E_AHI_TICK_TIMER_RESTART);
}

/****************************************************************************
 *
 * NAME: PAN_CRD_vMlmeDcfmInd
 *
 * DESCRIPTION:
 * Process any incoming managment events from the stack.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  psMlmeInd
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
PRIVATE void PAN_CRD_vMlmeDcfmInd(MAC_MlmeDcfmInd_s *psMlmeInd)
{
	/* Which event type ? */
    switch (psMlmeInd->u8Type)
    {
		/* Incoming scan results ? */
		case MAC_MLME_DCFM_SCAN:
		{
			/* Waiting to create network ? */
			if (NODE_sData.eNwkState == NODE_NWKSTATE_CREATE)
			{
				/* Energy scan results ? */
				if (psMlmeInd->uParam.sDcfmScan.u8ScanType == MAC_MLME_SCAN_TYPE_ENERGY_DETECT)
				{
					/* Process energy scan results and start device as coordinator */
					PAN_CRD_vDcfmScanEnergy(psMlmeInd);
				}
			}
		}
		break;

		/* Incoming association request ? */
		case MAC_MLME_IND_ASSOCIATE:
		{
			/* Network is up ? */
			if (NODE_sData.eNwkState == NODE_NWKSTATE_UP)
			{
				/* Handle incoming association request */
				vPanCrdIndAssociate(psMlmeInd);
			}
		}
		break;

		/* Others ? */
		default:
		{
			/* Do nothing */
//			gate_vMlmeDcfmInd(psMlmeInd);
		}
		break;
    }
}

/****************************************************************************
 *
 * NAME: PAN_CRD_vMcpsDcfmInd
 *
 * DESCRIPTION:
 * Process incoming data events from the stack.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  psMcpsInd
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
PRIVATE void PAN_CRD_vMcpsDcfmInd(MAC_McpsDcfmInd_s *psMcpsInd)
{
	/* Which event type ? */
	switch(psMcpsInd->u8Type)
	{
		/* Incoming data frame ? */
        case MAC_MCPS_IND_DATA:
		{
    		/* Network is up ? */
    		if (NODE_sData.eNwkState == NODE_NWKSTATE_UP)
    		{
				/* Handle incoming data */
            	PAN_CRD_vIndData(psMcpsInd);
			}
		}
        break;

		/* Incoming acknowledgement or ack timeout ? */
        case MAC_MCPS_DCFM_DATA:
		{
    		/* Network is up ? */
    		if (NODE_sData.eNwkState == NODE_NWKSTATE_UP)
    		{
				/* Handle incoming delayed confirm data */
	            PAN_CRD_vDcfmData(psMcpsInd);
			}
		}
        break;

		/* Others ? */
        default:
		{
			/* Do nothing */
			;
		}
        break;
    }
}

/****************************************************************************
 *
 * NAME: PAN_CRD_vHwInd
 *
 * DESCRIPTION:
 * Process any hardware events.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  psAHI_Ind
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
PRIVATE void PAN_CRD_vHwInd(AppQApiHwInd_s *psAHI_Ind)
{
	/* Tick timer ? */
	if (psAHI_Ind->u32DeviceId == E_AHI_DEVICE_TICK_TIMER)
	{
		/* Call tick function for node */
		NODE_vTick();
	}
	vPanCrdEslHwIndHandle(psAHI_Ind);
}

/****************************************************************************
 *
 * NAME: PAN_CRD_vIndData
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 *
 * NOTES:
 ****************************************************************************/
PRIVATE void PAN_CRD_vIndData(MAC_McpsDcfmInd_s *psMcpsInd)
{
    MAC_RxFrameData_s *psFrame;

    psFrame = &psMcpsInd->uParam.sIndData.sFrame;
    uint64 u64SrcAddr;

	/* Short addressing used ? */
	if (psFrame->sSrcAddr.u8AddrMode == 2)
	{
		/* Debug */
		DBG_vPrintf(PAN_CRD_TRACE, "\r\n%d PAN_CRD < PAN_CRD_vIndData() Mode=%d Short=%#x",
			NODE_sData.u32Timer, psFrame->sSrcAddr.u8AddrMode, psFrame->sSrcAddr.uAddr.u16Short);
		u64SrcAddr = (uint64) psFrame->sSrcAddr.uAddr.u16Short;

		/* Pass data to wuart for handling */
//		WUART_vRx((uint64) psFrame->sSrcAddr.uAddr.u16Short, psFrame->u8SduLength, psFrame->au8Sdu);
		rfMsgHandle(CRD_DEVICE, u64SrcAddr, psFrame->u8SduLength, psFrame->au8Sdu);
	}
	/* Extended addressing used ? */
	else if (psFrame->sSrcAddr.u8AddrMode == 3)
	{
		/* Debug */
		DBG_vPrintf(PAN_CRD_TRACE, "\r\n%d PAN_CRD < PAN_CRD_vIndData() Mode=%d Ext=%#x:%x",
			NODE_sData.u32Timer, psFrame->sSrcAddr.u8AddrMode,
			psFrame->sSrcAddr.uAddr.sExt.u32H, psFrame->sSrcAddr.uAddr.sExt.u32L);

		/* Construct extended address */
		u64SrcAddr = ((uint64) psFrame->sSrcAddr.uAddr.sExt.u32H << 32) | (uint64) psFrame->sSrcAddr.uAddr.sExt.u32L;

		/* Pass data to wuart for handling */
//		WUART_vRx(u64SrcAddr, psFrame->u8SduLength, psFrame->au8Sdu);
		rfMsgHandle(CRD_DEVICE, u64SrcAddr, psFrame->u8SduLength, psFrame->au8Sdu);
	}
}

/****************************************************************************
 *
 * NAME: PAN_CRD_vDcfmData
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 *
 * NOTES:
 ****************************************************************************/
//533094 PAN_CRD < PAN_CRD_vDcfmData() StatusSuccess=0
PRIVATE void PAN_CRD_vDcfmData(MAC_McpsDcfmInd_s *psMcpsInd)
{
    if (psMcpsInd->uParam.sDcfmData.u8Status == MAC_ENUM_SUCCESS)
    {
        /* Data frame transmission successful */
    	/* Debug */
    	DBG_vPrintf(PAN_CRD_TRACE, "\r\n%d PAN_CRD < PAN_CRD_vDcfmData() StatusSuccess=%d",
    		NODE_sData.u32Timer, psMcpsInd->uParam.sDcfmData.u8Status);
//    	setrfTxStatus(RF_SEND_OK);

    	msgSendCallback(RF_SEND_OK);
    }
    else
    {
        /* Data transmission falied after 3 retries at MAC layer. */
    	/* Debug */
    	DBG_vPrintf(PAN_CRD_TRACE, "\r\n%d PAN_CRD < PAN_CRD_vDcfmData() StatusError=%d",
    		NODE_sData.u32Timer, psMcpsInd->uParam.sDcfmData.u8Status);
//    	setrfTxStatus(RF_SEND_ERR);
    	msgSendCallback(RF_SEND_ERR);
    }
}

/****************************************************************************
 *
 * NAME: PAN_CRD_vReqEnergyScan
 *
 * DESCRIPTION:
 * Starts an enery sacn on the channels specified.
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
PRIVATE void PAN_CRD_vReqEnergyScan(void)
{
	/* Debug */
	DBG_vPrintf(PAN_CRD_TRACE, "\r\n%d PAN_CRD < PAN_CRD_vReqEnergyScan()",
		NODE_sData.u32Timer);

    /* Structures used to hold data for MLME request and response */
    MAC_MlmeReqRsp_s   sMlmeReqRsp;
    MAC_MlmeSyncCfm_s  sMlmeSyncCfm;

    NODE_sData.eNwkState = NODE_NWKSTATE_CREATE;

    /* Start energy detect scan */
    sMlmeReqRsp.u8Type = MAC_MLME_REQ_SCAN;
    sMlmeReqRsp.u8ParamLength = sizeof(MAC_MlmeReqStart_s);
    sMlmeReqRsp.uParam.sReqScan.u8ScanType = MAC_MLME_SCAN_TYPE_ENERGY_DETECT;
    sMlmeReqRsp.uParam.sReqScan.u32ScanChannels = SCAN_CHANNELS;
    sMlmeReqRsp.uParam.sReqScan.u8ScanDuration = ENERGY_SCAN_DURATION;

    vAppApiMlmeRequest(&sMlmeReqRsp, &sMlmeSyncCfm);
}

/****************************************************************************
 *
 * NAME: PAN_CRD_vDcfmScanEnergy
 *
 * DESCRIPTION:
 * Selects a channel with low enery content for use by the wireless UART.
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
PRIVATE void PAN_CRD_vDcfmScanEnergy(MAC_MlmeDcfmInd_s *psMlmeInd)
{
    uint8 i = 0;
    uint8 u8MinEnergy;
    uint8 ch;

	u8MinEnergy = (psMlmeInd->uParam.sDcfmScan.uList.au8EnergyDetect[0]);
	NODE_sData.u8Channel = getActiveCh(1);
	if(0 == NODE_sData.u8Channel)
	{
		NODE_sData.u8Channel = CHANNEL_MIN;
	}

	/* Debug */
	DBG_vPrintf(PAN_CRD_TRACE, "\r\n%d PAN_CRD < PAN_CRD_vDcfmScanEnergy() resultListSize = %d",
		NODE_sData.u32Timer, psMlmeInd->uParam.sDcfmScan.u8ResultListSize);

	/* Search list to find quietest channel */
    while (i < psMlmeInd->uParam.sDcfmScan.u8ResultListSize)
    {
        if ((psMlmeInd->uParam.sDcfmScan.uList.au8EnergyDetect[i]) < u8MinEnergy)
        {
			u8MinEnergy = (psMlmeInd->uParam.sDcfmScan.uList.au8EnergyDetect[i]);
			ch = getActiveCh(i+1);
			if(ch != 0)
			{
				NODE_sData.u8Channel = ch;
			}
		}
		i++;
    }

	/* Debug */
	DBG_vPrintf(PAN_CRD_TRACE, "\r\n Ch=%d Energy=%d", NODE_sData.u8Channel, u8MinEnergy);

	PAN_CRD_vReqStartCoordinator();
}


/****************************************************************************
 *
 * NAME: PAN_CRD_vReqStartCoordinator
 *
 * DESCRIPTION:
 * Starts the network by configuring the controller board to act as the PAN
 * coordinator.
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * TRUE if network was started successfully otherwise FALSE
 *
 * NOTES:
 * None.
 ****************************************************************************/
PRIVATE void PAN_CRD_vReqStartCoordinator(void)
{
    /* Structures used to hold data for MLME request and response */
    MAC_MlmeReqRsp_s   sMlmeReqRsp;
    MAC_MlmeSyncCfm_s  sMlmeSyncCfm;

	/* Network is now up */
    NODE_sData.eNwkState = NODE_NWKSTATE_UP;

	/* Debug */
	DBG_vPrintf(PAN_CRD_TRACE, "\r\n%d PAN_CRD < PAN_CRD_vReqStartCoordinator() ch %d",
		NODE_sData.u32Timer, NODE_sData.u8Channel);

    /* Start Pan */
    sMlmeReqRsp.u8Type = MAC_MLME_REQ_START;
    sMlmeReqRsp.u8ParamLength = sizeof(MAC_MlmeReqStart_s);
    sMlmeReqRsp.uParam.sReqStart.u16PanId = NODE_sData.u16PanId;
    sMlmeReqRsp.uParam.sReqStart.u8Channel = NODE_sData.u8Channel;
    sMlmeReqRsp.uParam.sReqStart.u8BeaconOrder = 0x0F; // not use
    sMlmeReqRsp.uParam.sReqStart.u8SuperframeOrder = 0x0F; // not use
    sMlmeReqRsp.uParam.sReqStart.u8PanCoordinator = TRUE;
    sMlmeReqRsp.uParam.sReqStart.u8BatteryLifeExt = FALSE;
    sMlmeReqRsp.uParam.sReqStart.u8Realignment = FALSE;
    sMlmeReqRsp.uParam.sReqStart.u8SecurityEnable = FALSE;

    vAppApiMlmeRequest(&sMlmeReqRsp, &sMlmeSyncCfm);

    EEPROM_saveNodeParm(&NODE_sData);

	// /* Wuart not running ? */
	// if (WUART_sData.u8State == WUART_STATE_NONE)
	// {
	// 	/* Put wuart into idle state */
	// 	WUART_vState(WUART_STATE_IDLE);
	// }
}


// redefine the function at PAN_CRD_netgate.c
#if 0
/****************************************************************************
 *
 * NAME: PAN_CRD_vIndAssociate
 *
 * DESCRIPTION:
 * Handle request by node to join the network.
 *
 * PARAMETERS:      Name            RW  Usage
 *                  psMlmeInd
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * None.
 ****************************************************************************/
PRIVATE void PAN_CRD_vIndAssociate(MAC_MlmeDcfmInd_s *psMlmeInd)
{
    uint16 u16ShortAddr = 0xffff;
    uint16 u16EndDeviceIndex;

    MAC_MlmeReqRsp_s   sMlmeReqRsp;
    MAC_MlmeSyncCfm_s  sMlmeSyncCfm;

	/* Debug */
	DBG_vPrintf(PAN_CRD_TRACE, "\r\n%d PAN_CRD < PAN_CRD_vIndAssociate() Ext=%#x:%x",
		NODE_sData.u32Timer,
		psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32H,
		psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32L);

	/* Loop through current end devices */
	for (u16EndDeviceIndex = 0;
		 u16EndDeviceIndex < MAX_CHILDREN && u16ShortAddr == 0xffff;
		 u16EndDeviceIndex++)
	{
		/* Already got this end device ? */
		if (PAN_CRD_sData.asChild[u16EndDeviceIndex].u32ExtAddrL 	==
            psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32L				&&
		    PAN_CRD_sData.asChild[u16EndDeviceIndex].u32ExtAddrH 	==
            psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32H)
		{
			/* Note the short address of this entry */
			u16ShortAddr = PAN_CRD_sData.asChild[u16EndDeviceIndex].u16ShortAddr;

			sMlmeReqRsp.uParam.sRspAssociate.u8Status = 0; /* Access granted */

			/* Debug */
			DBG_vPrintf(PAN_CRD_TRACE, "%d Short=%#x REJOIN @ %d of child table.",
				NODE_sData.u32Timer, u16ShortAddr, u16EndDeviceIndex);
		}
	}

	/* Not an existing end device and have room for more ? */
    if (u16ShortAddr                       == 0xffff &&
        PAN_CRD_sData.u16Children <  MAX_CHILDREN)
    {
        /* Store end device address data */
        u16EndDeviceIndex = PAN_CRD_sData.u16Children;
        u16ShortAddr = (uint16)(psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32L & 0xffff);

        PAN_CRD_sData.asChild[u16EndDeviceIndex].u16ShortAddr = u16ShortAddr;

        PAN_CRD_sData.asChild[u16EndDeviceIndex].u32ExtAddrL  =
        psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32L;

        PAN_CRD_sData.asChild[u16EndDeviceIndex].u32ExtAddrH  =
        psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32H;

        PAN_CRD_sData.u16Children++;

        sMlmeReqRsp.uParam.sRspAssociate.u8Status = 0; /* Access granted */

		/* Debug */
		DBG_vPrintf(PAN_CRD_TRACE, " Short=%#x JOIN",
			NODE_sData.u32Timer, u16ShortAddr);
    }
    else if (u16ShortAddr == 0xffff)
    {
        sMlmeReqRsp.uParam.sRspAssociate.u8Status = 2; /* Denied */

		/* Debug */
		DBG_vPrintf(PAN_CRD_TRACE, " DENIED");
    }

    /* Create association response */
    sMlmeReqRsp.u8Type = MAC_MLME_RSP_ASSOCIATE;
    sMlmeReqRsp.u8ParamLength = sizeof(MAC_MlmeRspAssociate_s);
    sMlmeReqRsp.uParam.sRspAssociate.sDeviceAddr.u32H = psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32H;
    sMlmeReqRsp.uParam.sRspAssociate.sDeviceAddr.u32L = psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32L;
    sMlmeReqRsp.uParam.sRspAssociate.u16AssocShortAddr = u16ShortAddr;
    sMlmeReqRsp.uParam.sRspAssociate.u8SecurityEnable = FALSE;

    /* Send association response. There is no confirmation for an association
       response, hence no need to check */
    vAppApiMlmeRequest(&sMlmeReqRsp, &sMlmeSyncCfm);
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
