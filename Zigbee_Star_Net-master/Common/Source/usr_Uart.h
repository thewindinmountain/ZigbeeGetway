/*
 * USR_UART.h
 *
 *  Created on: 2017-7-21
 *      Author: xubov
 */

#ifndef USR_UART_H_
#define USR_UART_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "queue.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/** USR_UART configuration */
#define USR_UART   		    E_AHI_UART_1		    /**< Uart to use */
#define USR_UART_BAUD		115200					/**< Baud rate */
#define USR_UART_EVEN		FALSE					/**< Even parity */
#define USR_UART_PARITY	    FALSE					/**< Enable parity */
#define USR_UART_WORDLEN	E_AHI_UART_WORD_LEN_8	/**< Word length */
#define USR_UART_ONESTOP	TRUE					/**< One stop bit */
#define USR_UART_RTSCTS	    FALSE					/**< RTS/CTS flow control */
#define USR_UART_XONXOFF	FALSE					/**< XON/XOFF flow control */

/* Queue indicies */
#define USR_UART_QUEUE_TX 	  		   0			/**< Transmit queue */
#define USR_UART_QUEUE_RX 	  		   1			/**< Receive queue */
#define USR_UART_QUEUES   	  		   2			/**< Number of queues */

/* Queue data size (use a power of two) */
#define USR_UART_QUEUE_DATA		 2048	/**< Size of queue */
#define USR_UART_QUEUE_DATA_LOW	 (USR_UART_QUEUE_DATA >> 2) /**< Low level of free space */
#define USR_UART_QUEUE_DATA_HIGH (USR_UART_QUEUE_DATA >> 1) /**< Hign level of free space */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void   vUsrUartInit(uint8, uint32);
PUBLIC void   vCatchUartPkt(QUEUE_tsData *, uint8);
PUBLIC void   vUsrUartTx(uint8 *, uint16);
PUBLIC void   vUsrUartRx(uint8 * pu8DataBuf, uint16 *u16RxSize);
PUBLIC uint16 u16UartRxBufUsed(void);
PUBLIC uint16_t uart_write_sync(char *buf, uint16_t bufsize, uint16_t *sent);
PUBLIC uint16_t uart_read_sync(char *buf, uint16_t bufsize, uint16_t *get);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#endif /* USR_UART_H_ */
