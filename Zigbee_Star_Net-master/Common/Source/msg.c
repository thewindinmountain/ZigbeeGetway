/*
 * msg.c
 *
 *  Created on: 2017��12��25��
 *      Author: xubov
 */
#include <string.h>
#include <jendefs.h>
#include <mac_sap_CombinedMac.h>
#include <dbg.h>
#include "dbg_config.h"
#include "node.h"
#include "app_childTable.h"
#include "msg.h"
#include "sjson_app.h"
#include "uart.h"

#ifdef DEBUG_MSG
#define MSG_TRACE TRUE
#else
#define MSG_TRACE FALSE
#endif


#define PKT_HEAD_LEN 2
#define DATA_LEN_PER_PKT (MAC_MAX_DATA_PAYLOAD_LEN - PKT_HEAD_LEN - 14) // 14-too long will not send out

uint8  msgSendBuf[4096];
uint16 msgLen = 0;
uint64 txAddr = 0;
uint16 pktCurNo = 0;
uint16 pktLastNo = 0;

uint8  msgSendStatus = 0;
uint8  msgSendCmdNo=0;

uint8  msgRecvBuf[4096];
uint8  msgRecvCurNo;
uint16 msgRecvLen;

uint8 uartBuf[128];

uint8 poll_mute = 0;
uint8 pollMute(void)
{
	if(poll_mute !=0)
		poll_mute-=1;
	return poll_mute;
}

uint16 getMsgSendStatus(void)
{
	return msgSendStatus;
}

void sendDataToMsgBuf(uint64 u64Addr, uint8 cmdNo, uint8 *data, uint16 dataLen)
{
	if(msgSendStatus)
		return;

	msgSendBuf[0] = dataLen&0xFF;
	msgSendBuf[1] = (dataLen>>8)&0xFF;
	msgSendBuf[2] = cmdNo;
	msgSendCmdNo  = cmdNo;
	memcpy(msgSendBuf+3, data, dataLen);
	msgLen = dataLen+3;
	DBG_vPrintf(MSG_TRACE, "\r\n ready2 send msg2 0x%x, msgLen=%d.", (uint32)u64Addr, msgLen);

	msgSendStatus = 1;

	txAddr = u64Addr;
	pktCurNo = 0;
	pktLastNo = (uint8)(msgLen/DATA_LEN_PER_PKT);
	if(msgLen%DATA_LEN_PER_PKT)
	{
		pktLastNo += 1;
	}
	DBG_vPrintf(MSG_TRACE, "\r\n >>>pktLastNo=%d.",pktLastNo);
}

void msgSendCallback(uint8 parm)//parm 1-sending, 2-ok, 3-send err
{
	static uint8 *data = NULL;
	static uint16 len;
	static uint8 retryCnt = 0;
	static uint8 dataBuf[MAC_MAX_DATA_PAYLOAD_LEN];

	if(msgSendStatus == 0)// no data or sending...
		return;

//	DBG_vPrintf(MSG_TRACE, "\r\n >>>pktCurNo=%d, parm=%d.",pktCurNo,parm);
	switch(parm)
	{
		case RF_SEND_OK: // send ok, then check send done or send next
		{
			pktCurNo += 1;
			if(pktCurNo > pktLastNo)
			{
				DBG_vPrintf(MSG_TRACE, "\r\n >>> cmdNo=%d, msgLen= %d, send none.", msgSendCmdNo, msgLen);
				data = NULL;
				msgLen = 0;
				retryCnt = 0;
				msgSendStatus = 0;
			}else if(pktCurNo == pktLastNo)// last pkt
			{
				data = msgSendBuf + DATA_LEN_PER_PKT*(pktCurNo-1);
				len = (msgLen%DATA_LEN_PER_PKT);
				dataBuf[0] = pktCurNo;
				memcpy(dataBuf+1, data, len);
				NODE_vTx(txAddr, len+1, dataBuf, DATA_PKT);
			}else // send next pkt
			{
				retryCnt = 0;
				data = msgSendBuf + DATA_LEN_PER_PKT*(pktCurNo-1);
				len = DATA_LEN_PER_PKT;
				dataBuf[0] = pktCurNo;
				memcpy(dataBuf+1, data, len);
				NODE_vTx(txAddr, len+1, dataBuf, DATA_PKT);
			}
		}break;
		case RF_SEND_ERR:// send err, check
		{
			if(retryCnt++ > 3)
			{
				retryCnt = 0;
				msgSendStatus = 0;
				msgLen = 0;
				data = NULL;
				DBG_vPrintf(MSG_TRACE, "\r\n >>> cmdNo=%d, send time out, please waring server.", msgSendCmdNo);
				// send err to server.
				// do it.
			}else
			{
				DBG_vPrintf(MSG_TRACE, "\r\n >>> send err retry.");
				if(data != NULL)// retry
					NODE_vTx(txAddr, len+1, dataBuf, DATA_PKT);
			}
		}break;
		default:break;
	}
}


// curNo+payload
void msgRecv(uint64 u64Addr, uint8 *data, uint16 len)
{
	uint16 curRecvLen;

	msgRecvCurNo = *data;
	data += PKT_HEAD_LEN;// skip curNo.
	curRecvLen = len - PKT_HEAD_LEN;

	if(msgRecvCurNo == 0)
		return;

	msgRecvCurNo -= 1;
	memcpy(msgRecvBuf+msgRecvCurNo*PKT_HEAD_LEN, data, curRecvLen);

	if(msgRecvCurNo == 0)
	{
		//get pkt len;
		msgRecvLen = msgRecvBuf[0] + (msgRecvBuf[1]<<8);
	}

	DBG_vPrintf(MSG_TRACE, "\r\n curNo=%d, msgLen=%d,cmdNo=%d.",msgRecvCurNo, msgRecvLen, msgRecvBuf[2]);
	if((curRecvLen + DATA_LEN_PER_PKT*msgRecvCurNo) >= msgRecvLen)
	{
		//msg handle
		DBG_vPrintf(MSG_TRACE, "\r\n msgRecvLen=%d,curRecvLen=%d rx a msg pkt please handle it.",msgRecvLen,curRecvLen);
	}
}

//
//void endUsrMsgHandle(uint64 u64Addr, uint8 *data, uint16 len)
//{
//	msg *pMsg;
//	uint16 dataLen;
//	usrcmd newCmd;
//	char * pollCmd = NULL;
//
//	pMsg = (msg*)data;
//
//	dataLen = len - sizeof(msgHead);
//	if(pMsg->head.msgNum == curMsg.head.msgNum)
//	{
//		// rx done
//		get_msg_done_json_cmd(u64Addr, pMsg->head.msgNum);
//	}else if(pMsg->head.msgNum == curMsg.head.msgNum+1)
//	{
//		curMsg.head.msgNum += 1;
//		memcpy(pCurBuf, pMsg->data, dataLen);
//		curMsg.head.pkgTotalLen += dataLen;
//
//		if(curMsg.head.pkgTotalLen == pMsg->head.pkgTotalLen)
//		{
//			// cmd handle
//			newCmd.cmdId = usrmsgbuf[0];
//			newCmd.parmLen = curMsg.head.pkgTotalLen - 2;
//			newCmd.parm = (uint8*)(&usrmsgbuf[1]);
//			DBG_vPrintf(MSG_TRACE, "\r\n rx cmdId=%d parmLen=%d.",newCmd.cmdId, newCmd.parmLen);
//		}
//		// rx done
//		get_msg_done_json_cmd(u64Addr, pMsg->head.msgNum);
//	}else
//	{
//		DBG_vPrintf(MSG_TRACE, "\r\n miss msg num %d.", curMsg.head.msgNum+1);
//		pollCmd = get_miss_msg_json_cmd(u64Addr, curMsg.head.msgNum+1);
//	}
//	NODE_vTx(u64Addr, strlen(pollCmd), (uint8*)pollCmd, JSON_PKT);
//}

void endJsonMsgHandle(uint64 u64Addr, uint8 *data, uint16 len)
{
	char cmd[20];
	char parm[100];

	/* msgHandle */
	if(0 == getStrFromSjsonMsg("act", (char*)data, cmd))
	{
		if(0 == strcmp("poll", cmd))
		{
			if(0 == getStrFromSjsonMsg("parm", (char*)data, parm))
			{
				if(0 == strcmp("none", parm))
				{
					/* go to sleep */
					DBG_vPrintf(MSG_TRACE, "\r\n >>> poll nothing, go to sleep.");
				}else
				{
					poll_mute = 10;
					DBG_vPrintf(MSG_TRACE, "\r\n >>> poll get msg.");
				}
			}else
			{
				DBG_vPrintf(MSG_TRACE, "\r\n >>> Err: get parm");
			}
			return;
		}else
		{
			DBG_vPrintf(MSG_TRACE, "\r\n >>> Err: get none cmd");
		}
		/* do something here */
		/* do it*/
	}else
	{
		DBG_vPrintf(MSG_TRACE, "\r\n >>> not support cmd: %s", (char*)cmd);
	}
}

PUBLIC void uartMsgHandle(char *data, uint16 len)
{
	uint16 u16tagID;
	double dTagID;
	char actCmd[20];

	if(0 == getNumFromSjsonMsg("tagID", (char*)data, &dTagID))
	{
		u16tagID = (uint16)(dTagID);
		DBG_vPrintf(MSG_TRACE, "\r\n >>> get sjson cmd for 0x%x.", u16tagID);
	}

	if(0 == getStrFromSjsonMsg("act", (char*)data, actCmd))
	{
		if(0 == strcmp("setTab", actCmd))
		{
			DBG_vPrintf(MSG_TRACE, "\r\n >>> get setTable cmd. support soon.");
		}else if(0 == strcmp("setPrice", actCmd))
		{
			DBG_vPrintf(MSG_TRACE, "\r\n >>> get setPrice cmd. support soon.");
		}else if(0 == strcmp("setNum", actCmd))
		{
			DBG_vPrintf(MSG_TRACE, "\r\n >>> get setNum cmd. support soon.");
		}else if(0 == strcmp("disp", actCmd))
		{
			DBG_vPrintf(MSG_TRACE, "\r\n >>> get disp cmd. support soon.");
		}else if(0 == strcmp("setLed0", actCmd))
		{
			DBG_vPrintf(MSG_TRACE, "\r\n >>> get setLed0 cmd. support soon.");
		}else if(0 == strcmp("setLed1", actCmd))
		{
			DBG_vPrintf(MSG_TRACE, "\r\n >>> get setLed1 cmd. support soon.");
		}
		else
		{
			DBG_vPrintf(MSG_TRACE, "\r\n >>> Err: actCmd \"%s\" not support.", actCmd);
		}
	}
}

void crdJsonMsgHandle(uint64 u64Addr, uint8 *data, uint16 len)
{
	char cmd[20];
	char parm[100];
	char * pollCmd;
	uint32 i;
	static uint8 pollCnt =0 ;

	/* msgHandle */
	if(0 == getStrFromSjsonMsg("act", (char*)data, cmd))
	{
		if(0 == strcmp("poll", cmd))
		{
			if(0 == getStrFromSjsonMsg("parm", (char*)data, parm))
			{
				if(0 == strcmp("req", parm))
				{
					DBG_vPrintf(MSG_TRACE, "\r\n >>> get poll req");
					if(msgSendStatus == 0 && pollCnt++ > 10)
					{
						pollCnt = 0;
						pollCmd = get_poll_bak_with_data_json_cmd(u64Addr);
						DBG_vPrintf(MSG_TRACE, "\r\n poll return: %s", pollCmd);
						NODE_vTx(u64Addr, strlen(pollCmd), (uint8*)pollCmd, JSON_PKT);
						i=sizeof(msgSendBuf);
						while(i--) msgSendBuf[i]='0'+i%10;
						sendDataToMsgBuf(u64Addr,0x01, msgSendBuf, sizeof(msgSendBuf));
					}else
					{
						pollCmd = get_poll_bak_with_nodata_json_cmd(u64Addr);
						DBG_vPrintf(MSG_TRACE, "\r\n poll return: %s", pollCmd);
						NODE_vTx(u64Addr, strlen(pollCmd), (uint8*)pollCmd, JSON_PKT);
					}
				}
			}else
			{
				DBG_vPrintf(MSG_TRACE, "\r\n >>> waring parm: %s", parm);
			}
			return;
		}else
		{
			DBG_vPrintf(MSG_TRACE, "\r\n >>> not support cmd: %s", (char*)cmd);
		}

		/* do something here */
		/* do it*/
	}
}
/**
 * 协调器处理数据帧
 */
PRIVATE void crdDataMsgHnadle(uint64 u64RxAddr,uint8* pu8RxPayload,uint16 u16RxSize)
{

}

/**
 * 协调器处理应答帧
 */
PRIVATE void crdAckMsgHnadle(uint64 u64RxAddr,uint8* pu8RxPayload,uint16 u16RxSize)
{
	//TODO 此处有可能还需要处理
	vUsrUartTx(pu8RxPayload, (uint16) u16RxSize);
}
/**
 * 终端处理数据帧，处理接收到的数据，同时返回应答帧
 */
PRIVATE void endDataMsgHnadle(uint64 u64RxAddr,uint8* pu8RxPayload,uint16 u16RxSize)
{


}

PUBLIC  uint8 u8CrdEn = 0;
PUBLIC void rfMsgHandle(uint8 u8DeviceType, uint64 u64RxAddr, uint16 u16RxSize, uint8 *pu8RxPayload)
{
	tsNodeAddr node;
	uint16 postion;

	/*  */
	if(u64RxAddr & 0xFFFF)
	{
		node.u32ExtAddrL = (uint32)(u64RxAddr & 0xffffffff);
		node.u32ExtAddrH = (uint32)(u64RxAddr >> 32);
		node.u16ShortAddr = (uint16)node.u32ExtAddrL & 0xFFFF;
		DBG_vPrintf(MSG_TRACE, "\r\n%d Radio appRadio_vRx(Rx from %x:%x %d)",NODE_sData.u32Timer,node.u32ExtAddrH,node.u32ExtAddrL,u16RxSize);
	}else
	{
		node.u32ExtAddrL = 0;
		node.u32ExtAddrH = 0;
		node.u16ShortAddr = (uint16)u64RxAddr;
		DBG_vPrintf(MSG_TRACE, "\r\n%d Radio appRadio_vRx(Rx from %x %d)",NODE_sData.u32Timer,node.u16ShortAddr,u16RxSize);
	}

	/* for End device
	 * 1. check msg from parent, if not return
	 * 2. handle msg
	 * 2.1 check msg timerStamp active, if not return
	 * */
	if(END_DEVICE == u8DeviceType)
	{
		if(node.u16ShortAddr != NODE_sData.u16Parent   &&
		   node.u32ExtAddrL  != (uint32)NODE_sData.u64Parent)
		{
			return ;
		}

		switch(*pu8RxPayload)
		{
			case JSON_PKT:
				endJsonMsgHandle(u64RxAddr, pu8RxPayload+1, u16RxSize-1);
			break;
			case DATA_PKT:
				endDataMsgHnadle(u64RxAddr, pu8RxPayload+1, u16RxSize-1);
//				msgRecv(u64RxAddr, pu8RxPayload+1, u16RxSize-1);
			break;
		}
	}

	/* for crd device
	 * 1. check msg form children, if not return
	 * 2. handle msg
	 * 2.1 check msg timerStamp active, if not return
	 * */
	if(CRD_DEVICE == u8DeviceType)
	{
		if(FALSE == bChildTableSearch(&node, &postion))
		{
			return;
		}

		switch(*pu8RxPayload)
		{
			case JSON_PKT:
				crdJsonMsgHandle(u64RxAddr, pu8RxPayload+1, u16RxSize-1);
			break;
			case DATA_PKT:
//				crdDataMsgHnadle(u64RxAddr, pu8RxPayload+1, u16RxSize-1);
			break;
			case ACK_PKT:
				crdAckMsgHnadle(u64RxAddr, pu8RxPayload+1, u16RxSize-1);
			break;
		}
	}

	memset(pu8RxPayload, 0, u16RxSize);
}



