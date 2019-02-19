/*
 * usr_Uart.c
 *
 *  Created on: 2017-7-21
 *      Author: xubov
 */

#include <jendefs.h>
#include "node.h"
#include "queue.h"
#include "dbg.h"
#include "dbg_config.h"
#include "uart.h"
#include "usr_Uart.h"
#include "error.h"
#include "delay.h"

#ifndef DEBUG_USR_UART
#define TRACE_USR_UART FALSE
#else
#define TRACE_USR_UART TRUE
#endif

//#pragma pack(16)
//uint8 uartRxDataBuf[NODE_PAYLOAD];
//#pragma pack()

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE QUEUE_tsData  asQueue[USR_UART_QUEUES];					 	 /**< Queues */
PRIVATE uint8         au8QueueData[USR_UART_QUEUES][USR_UART_QUEUE_DATA]; /**< Queue data buffers */
PRIVATE uint8 		  u8UartPort = 	0xFF;							 /**< Uart0 for debugging, Uart1 for usr */

PUBLIC void vUsrUartInit(uint8 u8UartInit, uint32 u32BaudRate)
{
	/* Initialise the queues */
	QUEUE_bInit(&asQueue[USR_UART_QUEUE_TX], USR_UART_QUEUE_DATA, USR_UART_QUEUE_DATA_LOW, USR_UART_QUEUE_DATA_HIGH, au8QueueData[USR_UART_QUEUE_TX]);
	QUEUE_bInit(&asQueue[USR_UART_QUEUE_RX], USR_UART_QUEUE_DATA, USR_UART_QUEUE_DATA_LOW, USR_UART_QUEUE_DATA_HIGH, au8QueueData[USR_UART_QUEUE_RX]);

	/* Try to open the uart */
	if (UART_bOpen (u8UartInit,
					  u32BaudRate,
					  USR_UART_EVEN,
					  USR_UART_PARITY,
					  USR_UART_WORDLEN,
					  USR_UART_ONESTOP,
					  USR_UART_RTSCTS,
					  USR_UART_XONXOFF,
					  &asQueue[USR_UART_QUEUE_TX],
					  &asQueue[USR_UART_QUEUE_RX]))
	{
		/* Note uart being used */
		u8UartPort = u8UartInit;
	}
}

PUBLIC void vUsrUartTx(uint8 * pu8RxPayload, uint16 u16RxSize)
{
	  /* Is there space in the UART transmit buffer for this data */
  if (QUEUE_u16Free(&asQueue[USR_UART_QUEUE_TX]) >= u16RxSize - 1)
  {
    uint16 u16Pos;

    /* Loop through received data */
    for (u16Pos = 0; u16Pos < u16RxSize; u16Pos++)
    {
      /* Get uart to transmit this byte */
      UART_bPutChar(u8UartPort, pu8RxPayload[u16Pos]);
    }

    /* Start transmit */
    UART_vStartTx(u8UartPort);
  }
}

PUBLIC void vUsrUartRx(uint8 * pu8DataBuf, uint16 *u16RxSize)
{
	uint16 u16Pos;

	if(*u16RxSize == 0)
	{
		DBG_vPrintf(TRACE_USR_UART, "\r\n%d vUsrUartRx Err: *u16RxSize = %!",*u16RxSize);
		return;
	}

	/* Loop through received data */
	for (u16Pos = 0; u16Pos < *u16RxSize; u16Pos++)
	{
		/* Get uart to transmit this byte */
		if (FALSE == UART_bGetChar(u8UartPort, (char*)(pu8DataBuf + u16Pos)))
		{
			DBG_vPrintf(TRACE_USR_UART, "\r\n%d vUsrUartRx Err: UART_bGetChar return err!");
			*u16RxSize = u16Pos;
			break;
		}
	}
}

PUBLIC void vUartClearRxBuf(void)
{
	uint16 rxLen = QUEUE_u16Used(&asQueue[USR_UART_QUEUE_RX]);
	char  rxBuf;

	while(rxLen)
	{
		UART_bGetChar(u8UartPort, &rxBuf);
	}
}

PUBLIC uint16 u16UartRxBufUsed(void)
{
	return QUEUE_u16Used(&asQueue[USR_UART_QUEUE_RX]);
}

//PUBLIC uint32 vGetUsrUartRxDataLen(void)
//{
//	return ZQ_u32QueueGetQueueSize(&asQueue[USR_UART_QUEUE_RX]);
//}

#define MAX_WAIT_SEND_UART_COUNT	100000
uint16_t uart_write_sync(char *buf, uint16_t bufsize, uint16_t *sent)
{
	uint16_t result=ERR_OK;
//	long wait_complete=0;
//	int needCancelIfFailed = 1;

	vUsrUartTx((uint8*)buf, bufsize);
	* sent = bufsize;
	return result;
}

#define MAX_GET_UART_COUNT 500
uint16_t uart_read_sync(char *buf, uint16_t bufsize, uint16_t *get)
{
	uint16_t result = ERR_OK;
	uint16_t rxBufUsed;

	do
	{
		rxBufUsed = u16UartRxBufUsed();
		delay_xms(1);
	}while(rxBufUsed != u16UartRxBufUsed());

	if(rxBufUsed > 0)
	{
		if(rxBufUsed <= bufsize)
		{
			vUsrUartRx((uint8 *)buf, &rxBufUsed);
			*get = rxBufUsed;
		}else
		{
			vUsrUartRx((uint8 *)buf, &bufsize);
			*get = bufsize;
		}
	}else
	{
		result = ERR_IDLE;
		*get = 0;
	}

	return result;
}

