/*
 * msg.h
 *
 *  Created on: 2017楠烇拷2閺堬拷5閺冿拷
 *      Author: xubov
 */

#ifndef MSG_H_
#define MSG_H_



/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define MSG_HEAD_LEN 8

#define JSON_PKT 1
#define DATA_PKT 2
#define ACK_PKT 3

#define RF_SEND_OK  0
#define RF_SEND_ERR 2
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct
{
	//uint16 msgType;
	uint16 destAddr;
	uint16 srcAddr;
	uint16 msgNum;
	uint16 pkgTotalLen;
}msgHead;

typedef struct
{
	msgHead head;
	uint8  data[130];
}msg;

typedef struct
{
	uint16 cmdId;
	uint16 parmLen;
	uint8 *parm;
}usrcmd;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void rfMsgHandle(uint8 u8DeviceType, uint64 u64RxAddr, uint16 u16RxSize, uint8 *pu8RxPayload);
PUBLIC void uartMsgHandle(char *data, uint16 len);
PUBLIC void msgSendCallback(uint8 parm);
void sendDataToMsgBuf(uint64 addr, uint8 cmdNo, uint8 *data, uint16 dataLen);
uint16 getMsgSendStatus(void);
uint8 pollMute(void);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


#endif /* MSG_H_ */
