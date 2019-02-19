/*
 * crd_netGate.c
 *
 *  Created on: 2017年10月7日
 *      Author: xubov
 */

#include <jendefs.h>
#include "dbg.h"
#include <AppHardwareApi.h>
#include <AppQueueApi.h>
#include "hw_config.h"

#include "usr_timer.h"
#include "config.h"
#include "delay.h"
#include "error.h"
#include "usr_Uart.h"
#include "crd_netGate.h"
#include "app_childTable.h"
#include "eeprom.h"
#include "node.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef DEBUG_START_UP
    #define TRACE_GATE FALSE
#else
    #define TRACE_GATE TRUE
#endif

PUBLIC void crd_vIndAssociate(MAC_MlmeDcfmInd_s *psMlmeInd)
{
    tsNodeAddr child;
    uint16 index = 0;
    uint16 u16Err;

    MAC_MlmeReqRsp_s   sMlmeReqRsp;
    MAC_MlmeSyncCfm_s  sMlmeSyncCfm;

	child.u32ExtAddrH  = psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32H;
	child.u32ExtAddrL  = psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32L;
	child.u16ShortAddr = 0xFFFF;

	/* Debug */
	DBG_vPrintf(TRACE_GATE, "\r\n%d CRD < gate_vIndAssociate() Ext=%#x:%x  ",
		NODE_sData.u32Timer,
		child.u32ExtAddrH,
		child.u32ExtAddrL);

	// default status.
	sMlmeReqRsp.uParam.sRspAssociate.u8Status = 0; /* Access granted */

	if(bChildTableSearch(&child, &index))
	{
		DBG_vPrintf(TRACE_GATE, "\r\nREJOIN: index=%d Short=%#x.", index, child.u16ShortAddr);
	}else
	{
		child.u16ShortAddr = (uint16)(psMlmeInd->uParam.sIndAssociate.sDeviceAddr.u32L & 0xffff);

		u16Err = u16ChildTableAdd(&child, &index);
		if(0 == u16Err)// success
		{
			DBG_vPrintf(TRACE_GATE, "\r\nJOIN: index = %d Short = %#x.", index, child.u16ShortAddr);
		}else
		{
			sMlmeReqRsp.uParam.sRspAssociate.u8Status = 2; /* Denied */
			DBG_vPrintf(TRACE_GATE, "\r\nDENIED: short = %#x Err = %d.", child.u16ShortAddr, u16Err);
		}
	}

    /* Create association response */
    sMlmeReqRsp.u8Type = MAC_MLME_RSP_ASSOCIATE;
    sMlmeReqRsp.u8ParamLength = sizeof(MAC_MlmeRspAssociate_s);
    sMlmeReqRsp.uParam.sRspAssociate.sDeviceAddr.u32H = child.u32ExtAddrH;
    sMlmeReqRsp.uParam.sRspAssociate.sDeviceAddr.u32L = child.u32ExtAddrL;
    sMlmeReqRsp.uParam.sRspAssociate.u16AssocShortAddr = child.u16ShortAddr;
    sMlmeReqRsp.uParam.sRspAssociate.u8SecurityEnable = FALSE;

    /* Send association response. There is no confirmation for an association
       response, hence no need to check */
    vAppApiMlmeRequest(&sMlmeReqRsp, &sMlmeSyncCfm);
}


PUBLIC void crd_vReqStartCoordinator(uint8 panSet)
{
    /* Structures used to hold data for MLME request and response */
    MAC_MlmeReqRsp_s   sMlmeReqRsp;
    MAC_MlmeSyncCfm_s  sMlmeSyncCfm;

    if(panSet == 0) // pan crd has done when init.
    {
		/* Set short address in pib */
		MAC_vPibSetShortAddr(NODE_sData.pvMac, NODE_sData.u16Address);
		/* Set pan id in pib */
		MAC_vPibSetPanId(NODE_sData.pvMac, NODE_sData.u16PanId);
		/* Enable receiver to be on when idle */
		MAC_vPibSetRxOnWhenIdle(NODE_sData.pvMac, TRUE, FALSE );
		/* Allow nodes to associate */
		NODE_sData.psPib->bAssociationPermit = 1;
    }

	/* Network is now up */
    NODE_sData.eNwkState = NODE_NWKSTATE_UP;

	/* Debug */
	DBG_vPrintf(TRACE_GATE, "\r\n%d CRD < crd_vReqStartCoordinator() ch %d as %d(0- NOT PAN, 1- PAN)",
		NODE_sData.u32Timer, NODE_sData.u8Channel, panSet);

    /* Start Pan */
    sMlmeReqRsp.u8Type = MAC_MLME_REQ_START;
    sMlmeReqRsp.u8ParamLength = sizeof(MAC_MlmeReqStart_s);
    sMlmeReqRsp.uParam.sReqStart.u16PanId = NODE_sData.u16PanId;
    sMlmeReqRsp.uParam.sReqStart.u8Channel = NODE_sData.u8Channel;
    sMlmeReqRsp.uParam.sReqStart.u8BeaconOrder = 0x0F; // not use
    sMlmeReqRsp.uParam.sReqStart.u8SuperframeOrder = 0x0F; // not use
    if(panSet != 0)
    {
    	sMlmeReqRsp.uParam.sReqStart.u8PanCoordinator = TRUE;
    }else
    {
    	sMlmeReqRsp.uParam.sReqStart.u8PanCoordinator = FALSE;
    }
    sMlmeReqRsp.uParam.sReqStart.u8BatteryLifeExt = FALSE;
    sMlmeReqRsp.uParam.sReqStart.u8Realignment = FALSE;
    sMlmeReqRsp.uParam.sReqStart.u8SecurityEnable = FALSE;

    vAppApiMlmeRequest(&sMlmeReqRsp, &sMlmeSyncCfm);
}
