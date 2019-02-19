/*****************************************************************************/
/*!
 *\MODULE              Debug (Application Layer)
 *
 *\COMPONENT           $HeadURL: https://www.collabnet.nxp.com/svn/lprf_apps/Application_Notes/JN-AN-1069-IEEE-802.15.4-Serial-Cable-Replacement/Trunk/Common/Source/dbg.c $
 *
 *\VERSION			   $Revision: 16555 $
 *
 *\REVISION            $Id: dbg.c 16555 2015-11-20 12:14:37Z nxa04494 $
 *
 *\DATED               $Date: 2015-11-20 12:14:37 +0000 (Fri, 20 Nov 2015) $
 *
 *\AUTHOR              $Author: nxa04494 $
 *
 *\DESCRIPTION         Debug (Application Layer) - implementation.
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
/* Standard includes */
#include <string.h>
/* Jennic includes */
#include <jendefs.h>
#include <AppHardwareApi.h>

/* Application includes */
#include "dbg.h"
#include "uart.h"
#include "delay.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE QUEUE_tsData		asQueue[DBG_QUEUES];					 /**< Queues */
PRIVATE uint8              au8QueueData[DBG_QUEUES][DBG_QUEUE_DATA]; /**< Queue data buffers */
PRIVATE uint8 				u8Uart = 0xff;							 /**< Uart for debugging */

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: DBG_vUartInit
 *
 * DESCRIPTION:
 * Initialises debugging uart.
 *
 ****************************************************************************/
PUBLIC void DBG_vUartInit(uint8 u8UartInit, uint32 u32BaudRate)
{
	/* Initialise the queues */
	QUEUE_bInit(&asQueue[DBG_QUEUE_TX], DBG_QUEUE_DATA, DBG_QUEUE_DATA_LOW, DBG_QUEUE_DATA_HIGH, au8QueueData[DBG_QUEUE_TX]);
	QUEUE_bInit(&asQueue[DBG_QUEUE_RX], DBG_QUEUE_DATA, DBG_QUEUE_DATA_LOW, DBG_QUEUE_DATA_HIGH, au8QueueData[DBG_QUEUE_RX]);

	/* Try to open the uart */
	if (UART_bOpen (u8UartInit,
					u32BaudRate,
					DBG_EVEN,
					DBG_PARITY,
					DBG_WORDLEN,
					DBG_ONESTOP,
					DBG_RTSCTS,
					DBG_XONXOFF,
					&asQueue[DBG_QUEUE_TX],
					&asQueue[DBG_QUEUE_RX]))
	{
		/* Note uart being used */
		u8Uart = u8UartInit;
	}
}

/****************************************************************************
 *
 * NAME: DBG_vPrintf
 *
 * DESCRIPTION:
 * Output debug message using printf
 *
 ****************************************************************************/
PUBLIC void	DBG_vPrintf(bool_t bStream, const char *pcFormat, ...)
{
	/* Stream is anabled and uart is opened ? */
	if (bStream == TRUE && u8Uart < 2)
	{
		va_list ap;

		/* Initialise argument pointer */
		va_start(ap, pcFormat);
		/* Call the worker function to do the work */
		UART_bPrintf_ap(u8Uart, pcFormat, ap);
		while((u8AHI_UartReadLineStatus(u8Uart) & E_AHI_UART_LS_THRE ) == 0);
	}
}


PUBLIC void	DBG_kprintf(const char *pcFormat, ...)
{
	va_list ap;

	/* Initialise argument pointer */
	va_start(ap, pcFormat);
	/* Call the worker function to do the work */
	UART_bPrintf_ap(u8Uart, pcFormat, ap);
	while((u8AHI_UartReadLineStatus(u8Uart) & E_AHI_UART_LS_THRE ) == 0);
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
PUBLIC void vDbgUartRx(uint8 * pu8DataBuf, uint16 *u16RxSize)
{
	uint16 u16Pos;

	do
	{
		*u16RxSize = QUEUE_u16Used(&asQueue[DBG_QUEUE_RX]);
		delay_xms(1);
	}while(*u16RxSize != QUEUE_u16Used(&asQueue[DBG_QUEUE_RX]));

	/* Loop through received data */
	for (u16Pos = 0; u16Pos < *u16RxSize; u16Pos++)
	{
		/* Get uart to transmit this byte */
		UART_bGetChar(DBG_UART, (char*)&pu8DataBuf[u16Pos]);
	}
}

void dbg_uart_read_sync(char *buf, uint16_t *len)
{
	vDbgUartRx((uint8 *)buf, len);
}


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
