/****************************************************************************/
/*!
 *\MODULE              Crd Device (Application Layer)
 *
 *\COMPONENT           $HeadURL: https://www.collabnet.nxp.com/svn/lprf_apps/Application_Notes/JN-AN-1069-IEEE-802.15.4-Serial-Cable-Replacement/Trunk/EndDevice/Source/CRD_enddevice.c $
 *
 *\VERSION			   $Revision: 18318 $
 *
 *\REVISION            $Id: CRD_enddevice.c 18318 2017-02-09 11:36:46Z nxp29759 $
 *
 *\DATED               $Date: 2017-02-09 11:36:46 +0000 (Thu, 09 Feb 2017) $
 *
 *\AUTHOR              $Author: nxp29759 $
 *
 *\DESCRIPTION         End Device (Application Layer) - implementation.
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
#include <mac_sap.h>
#include <mac_pib.h>
/* Common defines */
#include "config.h"
#include "dbg.h"
#include "node.h"
//#include "rnd.h"
#include "uart.h"
#ifdef SWDEBUG
    #include <gdb.h>
#endif

#include "msg.h"
#include "crd_esl.h"
#include "eeprom.h"
#include "crd_netGate.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define CRD_TRACE TRUE

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct
{
	uint32  		u32ScanChannels;
} CRD_tsData;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void CRD_vMain(void);
PRIVATE void CRD_vInit(void);
PRIVATE void CRD_vMlmeDcfmInd(MAC_MlmeDcfmInd_s *psMlmeInd);
PRIVATE void CRD_vMcpsDcfmInd(MAC_McpsDcfmInd_s *psMcpsInd);
PRIVATE void CRD_vHwInd(AppQApiHwInd_s *psAHI_Ind);
PRIVATE void CRD_vIndData(MAC_McpsDcfmInd_s *psMcpsInd);
PRIVATE void CRD_vDcfmData(MAC_McpsDcfmInd_s *psMcpsInd);
PRIVATE void CRD_vReqActiveScan(void);
PRIVATE void CRD_vDcfmScanActive(MAC_MlmeDcfmInd_s *psMlmeInd);
PRIVATE void CRD_vReqAssociate(void);
PRIVATE void CRD_vDcfmAssociate(MAC_MlmeDcfmInd_s *psMlmeInd);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE CRD_tsData CRD_sData;
PRIVATE uint8 u8Bit;
PRIVATE uint8 u8Channel;

PRIVATE bool  bMoreChannels;
PRIVATE uint8 u8BeaconCounter = 0;
PRIVATE uint8 u8Beacon;
PRIVATE uint16 au8RejectedBeacons[500]; /* 500 potential short MAC addresses */
PRIVATE bool  bAlreadySaved = FALSE;
PRIVATE uint8 u8SavedBeacons = 0;
PRIVATE bool  bAssociate = FALSE;

//PRIVATE uint16 u16ScanCounter = 0;
//PRIVATE uint8 u8RejoinCounter = 0;
//PRIVATE bool  bRejoinMode = FALSE;
//PRIVATE uint8 u8IoWakeValue;
//extern PUBLIC uint8 u8ScanCh[];

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

	#ifdef SWDEBUG
       HAL_GDB_INIT();
       HAL_BREAKPOINT();
	#endif

	/* Perform main processing */
	CRD_vMain();
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

	#ifdef SWDEBUG
       HAL_GDB_INIT();
       HAL_BREAKPOINT();
	#endif

	/* Perform main processing */
	CRD_vMain();
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: CRD_vMain
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
PRIVATE void CRD_vMain(void)
{
    MAC_MlmeDcfmInd_s *psMlmeInd;
	MAC_McpsDcfmInd_s *psMcpsInd;
    AppQApiHwInd_s    *psAHI_Ind;

	/* Initialise */
    CRD_vInit();
    crd_esl_Init();

//	/* Start state machine by requesting an active scan */
//	CRD_vReqActiveScan();

	if(NODE_sData.eNwkState == NODE_NWKSTATE_JOIN)
	{
		DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < Scan and Rejoin", NODE_sData.u32Timer);
		CRD_vReqAssociate();
	}else// if(NODE_sData.eNwkState < NODE_NWKSTATE_JOIN)
	{
		DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < eNwkState  %d", NODE_sData.u32Timer, NODE_sData.eNwkState);
		DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < Scan and join", NODE_sData.u32Timer);
		/* Start state machine by requesting an active scan */
		CRD_vReqActiveScan();
	}//else have joined.

	/* Loop processing events */
    while (1)
    {
		/* Check for anything on the MLME upward queue */
		do
		{
			psMlmeInd = psAppQApiReadMlmeInd();
			if (psMlmeInd != NULL)
			{
				CRD_vMlmeDcfmInd(psMlmeInd);
				vAppQApiReturnMlmeIndBuffer(psMlmeInd);
			}
		} while (psMlmeInd != NULL);

		/* Check for anything on the MCPS upward queue */
		do
		{
			psMcpsInd = psAppQApiReadMcpsInd();
			if (psMcpsInd != NULL)
			{
				CRD_vMcpsDcfmInd(psMcpsInd);
				vAppQApiReturnMcpsIndBuffer(psMcpsInd);
			}
		} while (psMcpsInd != NULL);

		/* Check for anything on the AHI upward queue */
		do
		{
			psAHI_Ind = psAppQApiReadHwInd();
			if (psAHI_Ind != NULL)
			{
				CRD_vHwInd(psAHI_Ind);
				vAppQApiReturnHwIndBuffer(psAHI_Ind);
			}
		} while (psAHI_Ind != NULL);
	}
}

/****************************************************************************
 *
 * NAME: CRD_vInit
 *
 * DESCRIPTION:
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void CRD_vInit(void)
{
	/* Setup interface to MAC */
    (void)u32AppQApiInit(NULL, NULL, NULL);
    (void)u32AHI_Init();

	/* Initialise debugging */
	DBG_vUartInit(DBG_UART, DBG_BAUD);
	/* Debug */
	DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < CRD_vInit()", NODE_sData.u32Timer);

	/* Initialise the scan channels */
	for(u8Bit = CHANNEL_MIN; u8Bit <= 26; u8Bit++)
	{
		if(SCAN_CHANNELS & (1 << u8Bit))
		{
			u8Channel = u8Bit;
			break;
		}
	}
	CRD_sData.u32ScanChannels = (1 << u8Channel);

	/* Going to use a fixed pan so set it now */
	NODE_sData.u16PanId = PAN_ID;

	/* Initialise common 15.4 node data */
	NODE_vInit();

	/* Initialise the random number generator */
//	RND_vInit();

	/* Set pan id in pib */
	MAC_vPibSetPanId(NODE_sData.pvMac, NODE_sData.u16PanId);
	/* Enable receiver to be on when idle */
	MAC_vPibSetRxOnWhenIdle(NODE_sData.pvMac, TRUE, FALSE);

	/* Debug */
	DBG_vPrintf(CRD_TRACE, " Scan=%#x Pan=%#x", CRD_sData.u32ScanChannels, NODE_sData.u16PanId);

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
 * NAME: CRD_vMlmeDcfmInd
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
PRIVATE void CRD_vMlmeDcfmInd(MAC_MlmeDcfmInd_s *psMlmeInd)
{
	/* Which event type ? */
    switch (psMlmeInd->u8Type)
    {
		/* Deferred confirmation that the scan is complete */
		case MAC_MLME_DCFM_SCAN:
		{
			/* Discovering networks ? */
			if (NODE_sData.eNwkState == NODE_NWKSTATE_DISCOVER)
			{
				/* Process scan delayed confirm */
				CRD_vDcfmScanActive(psMlmeInd);
			}
		}
		break;

		/* Deferred confirmation that the association process is complete */
		case MAC_MLME_DCFM_ASSOCIATE:
		{
			/* Attempting to join network ? */
			if (NODE_sData.eNwkState == NODE_NWKSTATE_JOIN)
			{
				/* Process assoicate delayed confirm */
				CRD_vDcfmAssociate(psMlmeInd);
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
//				CRD_vIndAssociate(psMlmeInd);
				crd_vIndAssociate(psMlmeInd);
			}
		}
		break;

		/* Others ? */
		default:
		{
			/* Do nothing */
//			CRD_esl_vMlmeDcfmInd(psMlmeInd);
		}
		break;
    }
}

/****************************************************************************
 *
 * NAME: CRD_vMcpsDcfmInd
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
PRIVATE void CRD_vMcpsDcfmInd(MAC_McpsDcfmInd_s *psMcpsInd)
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
            	CRD_vIndData(psMcpsInd);
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
	            CRD_vDcfmData(psMcpsInd);
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
 * NAME: CRD_vHwInd
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
PRIVATE void CRD_vHwInd(AppQApiHwInd_s *psAHI_Ind)
{
	/* Tick timer ? */
	if (psAHI_Ind->u32DeviceId == E_AHI_DEVICE_TICK_TIMER)
	{
		/* Call tick function for node */
		NODE_vTick();
	}

	vCrdEslHwIndHandle(psAHI_Ind);
}

/****************************************************************************
 *
 * NAME: CRD_vIndData
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 *
 * NOTES:
 ****************************************************************************/
PRIVATE void CRD_vIndData(MAC_McpsDcfmInd_s *psMcpsInd)
{
    MAC_RxFrameData_s *psFrame;

    psFrame = &psMcpsInd->uParam.sIndData.sFrame;
    uint64 u64SrcAddr;

	/* Short addressing used ? */
	if (psFrame->sSrcAddr.u8AddrMode == 2)
	{
		/* Debug */
		DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < CRD_vIndData() Mode=%d Short=%#x",
			NODE_sData.u32Timer, psFrame->sSrcAddr.u8AddrMode, psFrame->sSrcAddr.uAddr.u16Short);

		/* Data handling */
		rfMsgHandle(CRD_DEVICE, (uint64) psFrame->sSrcAddr.uAddr.u16Short, psFrame->u8SduLength, psFrame->au8Sdu);
	}
	/* Extended addressing used ? */
	else if (psFrame->sSrcAddr.u8AddrMode == 3)
	{
		/* Debug */
		DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < CRD_vIndData() Mode=%d Ext=%#x:%x",
			NODE_sData.u32Timer, psFrame->sSrcAddr.u8AddrMode,
			psFrame->sSrcAddr.uAddr.sExt.u32H, psFrame->sSrcAddr.uAddr.sExt.u32L);

		/* Construct extended address */
		u64SrcAddr = ((uint64) psFrame->sSrcAddr.uAddr.sExt.u32H << 32) | (uint64) psFrame->sSrcAddr.uAddr.sExt.u32L;
		/* Data handling */
		rfMsgHandle(CRD_DEVICE, u64SrcAddr, psFrame->u8SduLength, psFrame->au8Sdu);
	}
}

/****************************************************************************
 *
 * NAME: CRD_vDcfmData
 *
 * DESCRIPTION:
 *
 * PARAMETERS:      Name            RW  Usage
 *
 * RETURNS:
 *
 * NOTES:
 ****************************************************************************/
PRIVATE void CRD_vDcfmData(MAC_McpsDcfmInd_s *psMcpsInd)
{
    if (psMcpsInd->uParam.sDcfmData.u8Status == MAC_ENUM_SUCCESS)
    {
        /* Data frame transmission successful */

    	/* Debug */
    	DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < CRD_vDcfmData() StatusSuccess=%d",
    		NODE_sData.u32Timer, psMcpsInd->uParam.sDcfmData.u8Status);
    }
    else
    {
        /* Data transmission falied after 3 retries at MAC layer. */

    	/* Debug */
    	DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < CRD_vDcfmData() StatusError=%d",
    		NODE_sData.u32Timer, psMcpsInd->uParam.sDcfmData.u8Status);
    }
}

/****************************************************************************
 *
 * NAME: CRD_vReqActiveScan
 *
 * DESCRIPTION:
 * Start a scan to search for a network to join.
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
PRIVATE void CRD_vReqActiveScan(void)
{
    MAC_MlmeReqRsp_s  sMlmeReqRsp;
    MAC_MlmeSyncCfm_s sMlmeSyncCfm;

	/* Debug */
	DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < CRD_vReqActiveScan() Scan=%#x",
		NODE_sData.u32Timer,
		CRD_sData.u32ScanChannels);

   	/* Invalidate channel and addresses */
    NODE_sData.u8Channel  = 0;
    NODE_sData.u16Address =	0xffff;
    NODE_sData.u16Parent  =	0xffff;

    NODE_sData.eNwkState = NODE_NWKSTATE_DISCOVER;

    /* Request scan */
    sMlmeReqRsp.u8Type = MAC_MLME_REQ_SCAN;
    sMlmeReqRsp.u8ParamLength = sizeof(MAC_MlmeReqScan_s);
    sMlmeReqRsp.uParam.sReqScan.u8ScanType = MAC_MLME_SCAN_TYPE_ACTIVE;
    sMlmeReqRsp.uParam.sReqScan.u32ScanChannels = CRD_sData.u32ScanChannels;
    sMlmeReqRsp.uParam.sReqScan.u8ScanDuration = ACTIVE_SCAN_DURATION;

    vAppApiMlmeRequest(&sMlmeReqRsp, &sMlmeSyncCfm);
}

/****************************************************************************
 *
 * NAME: CRD_vDcfmScanActive
 *
 * DESCRIPTION:
 * Handle the reponse generated by the stack as a result of the network scan.
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
PRIVATE void CRD_vDcfmScanActive(MAC_MlmeDcfmInd_s *psMlmeInd)
{
	/* Debug */
	DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < CRD_vDcfmScanActive()",
		NODE_sData.u32Timer);

    if (psMlmeInd->uParam.sDcfmScan.u8ScanType == MAC_MLME_SCAN_TYPE_ACTIVE)
    {
		/* Debug */
		DBG_vPrintf(CRD_TRACE, " Status=%d Unscanned=%#x",
			psMlmeInd->uParam.sDcfmScan.u8Status,
			psMlmeInd->uParam.sDcfmScan.u32UnscannedChannels);

        if (psMlmeInd->uParam.sDcfmScan.u8Status == MAC_ENUM_SUCCESS)
        {
		    uint8 u8Result;
	    	MAC_PanDescr_s *psPanDesc;

			/* Loop through results */
			for (u8Result = 0; u8Result < psMlmeInd->uParam.sDcfmScan.u8ResultListSize; u8Result++)
            {
            	/* Get pointer to this result */
                psPanDesc = &psMlmeInd->uParam.sDcfmScan.uList.asPanDescr[u8Result];

                /* Reset saved flag */
				bAlreadySaved = FALSE;

				if(u8BeaconCounter == 0) /* first beacon ? */
				{
					/* Matching PAN ID and
					   short addressing used and
					   accepting association requests ? */
	                if ((psPanDesc->sCoord.u16PanId   == NODE_sData.u16PanId) &&
	                    (psPanDesc->sCoord.u8AddrMode == 2) &&
	                    (psPanDesc->u16SuperframeSpec & 0x8000))
	                {
	                	/* Attempt association */
	                    NODE_sData.u8Channel = psPanDesc->u8LogicalChan;
	                    NODE_sData.u16Parent = psPanDesc->sCoord.uAddr.u16Short;
	                    /* Debug */
						DBG_vPrintf(CRD_TRACE, " Chan=%d Coord=%#x",
							psPanDesc->u8LogicalChan, psPanDesc->sCoord.uAddr.u16Short);
						bAssociate = TRUE;
						CRD_vReqAssociate();
	                }
	                else
	                {
	                	if(psPanDesc->sCoord.u8AddrMode == 2) /* short address */
						{
							au8RejectedBeacons[u8BeaconCounter] = psPanDesc->sCoord.uAddr.u16Short;
						}
						else
						{
							au8RejectedBeacons[u8BeaconCounter] = (uint16) psPanDesc->sCoord.uAddr.sExt.u32L;
						}
						u8BeaconCounter++;
						u8SavedBeacons++;
	                }
				}
				else
				{
					/* check if already saved */
					for(u8Beacon = 0; u8Beacon < u8BeaconCounter; u8Beacon++)
					{
						if(psPanDesc->sCoord.u8AddrMode == 2) /* short address */
						{
							if(psPanDesc->sCoord.uAddr.u16Short == au8RejectedBeacons[u8Beacon])
							{
								bAlreadySaved = TRUE;
								break;
							}
						}
						else if(psPanDesc->sCoord.u8AddrMode == 3) /* ext address */
						{
							if(((uint16) psPanDesc->sCoord.uAddr.sExt.u32L) == au8RejectedBeacons[u8Beacon])
							{
								bAlreadySaved = TRUE;
								break;
							}
						}
					}

					if(!bAlreadySaved)
					{
						/* Matching PAN ID and
						   short addressing used and
						   accepting association requests ? */
						if ((psPanDesc->sCoord.u16PanId   == NODE_sData.u16PanId) &&
							(psPanDesc->sCoord.u8AddrMode == 2) &&
							(psPanDesc->u16SuperframeSpec & 0x8000))
						{
							/* Attempt association */
							NODE_sData.u8Channel = psPanDesc->u8LogicalChan;
							NODE_sData.u16Parent = psPanDesc->sCoord.uAddr.u16Short;
							/* Debug */
							DBG_vPrintf(CRD_TRACE, " Chan=%d Coord=%#x",
								psPanDesc->u8LogicalChan, psPanDesc->sCoord.uAddr.u16Short);
							bAssociate = TRUE;
							CRD_vReqAssociate();
						}
						else /* save beacon's addr */
						{
							if(psPanDesc->sCoord.u8AddrMode == 2) /* short address */
							{
								au8RejectedBeacons[u8BeaconCounter] = psPanDesc->sCoord.uAddr.u16Short;
							}
							else
							{
								au8RejectedBeacons[u8BeaconCounter] = (uint16) psPanDesc->sCoord.uAddr.sExt.u32L;
							}
							u8BeaconCounter++;
							u8SavedBeacons++;
						}
					}
				}
            }

			if(bAssociate)
			{
				bAssociate = FALSE;
			}
			else if(u8SavedBeacons > 0) /* unique beacons ? */
			{
				u8SavedBeacons = 0;
				/* Do another scan of the same channel */
				CRD_vReqActiveScan();
			}
			else /* no unique beacons */
			{
				CRD_sData.u32ScanChannels &= ~(1 << u8Channel); /* clear bit */
				for(u8Bit = (u8Channel+1); u8Bit <= 26; u8Bit++)
				{
					if(SCAN_CHANNELS & (1 << u8Bit))
					{
						u8Channel = u8Bit;
						bMoreChannels = TRUE;
						break;
					}
				}
				if(bMoreChannels)
				{
					bMoreChannels = FALSE;
					CRD_sData.u32ScanChannels |= (1 << u8Channel);
					/* Start the next scan */
					CRD_vReqActiveScan();
				}
				else
				{
					u8SavedBeacons = 0;
					u8BeaconCounter = 0;
					/* Go to sleep */
					NODE_sData.eNwkState = NODE_NWKSTATE_RESCAN;
				}
			}
        }
        else if (psMlmeInd->uParam.sDcfmScan.u8Status == MAC_ENUM_NO_BEACON)
        {
        	/* No beacons found on this channel, go to next one */
			CRD_sData.u32ScanChannels &= ~(1 << u8Channel); /* clear bit */
			for(u8Bit = (u8Channel+1); u8Bit <= 26; u8Bit++)
			{
				if(SCAN_CHANNELS & (1 << u8Bit))
				{
					u8Channel = u8Bit;
					bMoreChannels = TRUE;
					break;
				}
			}
			if(bMoreChannels)
			{
				bMoreChannels = FALSE;
				CRD_sData.u32ScanChannels |= (1 << u8Channel);
				/* Start the next scan */
				CRD_vReqActiveScan();
			}
			else
			{
				u8SavedBeacons = 0;
				u8BeaconCounter = 0;
				/* Go to sleep */
				NODE_sData.eNwkState = NODE_NWKSTATE_RESCAN;
			}
        }
        else
        {
        	/* Something went wrong */
			/* Attempt another active scan */
			CRD_vReqActiveScan();
        }
    }
}

/****************************************************************************
 *
 * NAME: CRD_vReqAssociate
 *
 * DESCRIPTION:
 * Start the association process with the network coordinator.
 *
 * PARAMETERS:      Name            RW  Usage
 * None.
 *
 * RETURNS:
 * None.
 *
 * NOTES:
 * Assumes that a network has been found during the network scan.
 ****************************************************************************/
PRIVATE void CRD_vReqAssociate(void)
{
    MAC_MlmeReqRsp_s  sMlmeReqRsp;
    MAC_MlmeSyncCfm_s sMlmeSyncCfm;

    NODE_sData.eNwkState = NODE_NWKSTATE_JOIN;

	/* Debug */
	DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < CRD_vReqAssociate()",
		NODE_sData.u32Timer);

    /* Create associate request. We know short address and PAN ID of
       coordinator as this is preset and we have checked that received
       beacon matched this */

    sMlmeReqRsp.u8Type = MAC_MLME_REQ_ASSOCIATE;
    sMlmeReqRsp.u8ParamLength = sizeof(MAC_MlmeReqAssociate_s);
    sMlmeReqRsp.uParam.sReqAssociate.u8LogicalChan = NODE_sData.u8Channel;
    sMlmeReqRsp.uParam.sReqAssociate.u8Capability = 0x80; /* We want short address, other features off */
    sMlmeReqRsp.uParam.sReqAssociate.u8Capability |= FFD; /* set as FFD type */
    sMlmeReqRsp.uParam.sReqAssociate.u8SecurityEnable = FALSE;
    sMlmeReqRsp.uParam.sReqAssociate.sCoord.u8AddrMode = 2;
    sMlmeReqRsp.uParam.sReqAssociate.sCoord.u16PanId = NODE_sData.u16PanId;
    sMlmeReqRsp.uParam.sReqAssociate.sCoord.uAddr.u16Short = NODE_sData.u16Parent;

    /* Put in associate request and check immediate confirm. Should be
       deferred, in which case response is handled by event handler */
    vAppApiMlmeRequest(&sMlmeReqRsp, &sMlmeSyncCfm);
}

/****************************************************************************
 *
 * NAME: CRD_vDcfmAssociate
 *
 * DESCRIPTION:
 * Handle the response generated by the stack as a result of the associate
 * start request.
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
PRIVATE void CRD_vDcfmAssociate(MAC_MlmeDcfmInd_s *psMlmeInd)
{
	/* Debug */
	DBG_vPrintf(CRD_TRACE, "\r\n%d CRD < CRD_vDcfmAssociate() Status=%d",
		NODE_sData.u32Timer,
		psMlmeInd->uParam.sDcfmAssociate.u8Status);

    /* If successfully associated with network coordinator */
    if (psMlmeInd->uParam.sDcfmAssociate.u8Status == MAC_ENUM_SUCCESS)
    {
    	/* Note our short address */
        NODE_sData.u16Address = psMlmeInd->uParam.sDcfmAssociate.u16AssocShortAddr;

        EEPROM_saveNodeParm(&NODE_sData);

        /* Network is now up */
        NODE_sData.eNwkState = NODE_NWKSTATE_UP;

		/* Debug */
		DBG_vPrintf(CRD_TRACE, " Short=%#x",
			psMlmeInd->uParam.sDcfmAssociate.u16AssocShortAddr);

//		crd_vReqStartCoordinator(); // start when get network deep.
		crd_esl_deepthReq();
   	}
	/* Got some channels left to scan ? */
	else if (CRD_sData.u32ScanChannels)
	{
		DBG_vPrintf(CRD_TRACE, " u16Parent=%#x",
				NODE_sData.u16Parent);
		/* Start the next scan */
		CRD_vReqActiveScan();
	}
	/* No channels left to scan ? */
	else
	{
		/* Go into rescan state */
		NODE_sData.eNwkState = NODE_NWKSTATE_RESCAN;
	}
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
