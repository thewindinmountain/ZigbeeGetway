/*
 * app_flash.c
 *
 *  Created on: 2017楠烇拷0閺堬拷閺冿拷
 *      Author: xubov
 */
#include "jendefs.h"
#include "AppHardwareApi.h"
#include "hw_config.h"
#include "drv_flash.h"
#include "drv_type.h"
#include "dbg.h"
#include "error.h"
#include "delay.h"

#ifdef DEBUG_APP_FLASH
#define APP_FLASH_TRACE TRUE
#else
#define APP_FLASH_TRACE FALSE
#endif
/***********************************************
 *  0xF000 8000 - 0xFFFF FFFF: Unpopulated
 *  0x0400 0000 - 0xF000 7FFF: RAM
 *  0x0200 0000 - 0x03FF FFFF: Perpherals
 *  0x0100 0000 - 0x01FF FFFF: Flash & EEPROM Registers
 *  ...Unpopulated
 *  0x0008 0000 - 0x000B FFFF: Flash ApPlications Code 256KB
 *  ...Unpopulated
 *  0x0000 0000 - 0x0000 FFFF: Flash Boot Code 8K
 *
 *
 *  OTP(One Time Programmable) memory as part of the embedded Flash(Index Sector)
 *  64Bit for MAC and 384bits for 3 128-bit AES
 * ***********************************************/

/*
 *	JN516X Chip	| Number 	 | Sector Size | Total Size
 *				| of Sectors | KB		   | KB
 * 	JN5169		| 16		 | 32		   | 512
 * 	JN5168		| 8 		 | 32		   | 256
 * 	JN5164		| 5 		 | 32		   | 160
 * 	JN5161		| 2 		 | 32		   | 64
 *
 */

/*
 * Note
 * 1. Sector Erase Time: 100ms
 * 2. Write Time: 1ms
 * 2. Application is stored from Sector 0
 * 3. 10000 Write/Erase cycles per sector
 * 4. All Flash address specified in the Flash Memory functions are offsets from the
 *    start of Flash memory and not absolute address.
 */

void appFlashInit(void)
{
	drvFlash.open(0,0);
}

uint16_t ReadStringFromFlash(char *feature_name, int offset, char *data, int size)
{
	uint16_t result = ERR_OK;
	uint8  status;

	drvFlash.read(USR_FLASH_START_ADDR + offset, (uint8*)data, size,  &status);

	result = status;

	if(result == ERR_OK)
		DBG_vPrintf(APP_FLASH_TRACE, "Read %s, data size %d success.\n",feature_name, size, data);
	else
	{
		DBG_vPrintf(APP_FLASH_TRACE, "Read %s, data size %d failed, err num: %d\n",feature_name, size, data, result);
	}
	return result;
}

uint16_t WriteStringToFlash(char *feature_name, int offset, char * data, int size)
{
	uint16_t result = ERR_OK;
	uint8  status;

	drvFlash.write(USR_FLASH_START_ADDR + offset,  (uint8*)data, size, &status);

	result = status;

	if(result != ERR_OK)
		DBG_vPrintf(APP_FLASH_TRACE, "%s failed, data size %d , err num: %d\n",feature_name, size, result);

	delay_xms(100);

	return result;
}


void appFlashTest(void)
{
	uint8  dataBuf[128];
	uint8  dataBufRead[128];
	uint8  status;
	uint16 i;

	appFlashInit();

	// write 0-128 and read it.
	DBG_vPrintf(APP_FLASH_TRACE, "\r\nwrite 0-128 and read it. ");
	for(i=0; i<128; i++)
	{
		dataBuf[i] = i;
	}
	drvFlash.write(USR_FLASH_START_ADDR, dataBuf,     sizeof(dataBuf),     &status);
	drvFlash.read (USR_FLASH_START_ADDR, dataBufRead, sizeof(dataBufRead), &status);
	DBG_vPrintf(APP_FLASH_TRACE, "\r\ndataBufRead[] ");
	for(i=0; i<128; i++)
	{
		DBG_vPrintf(APP_FLASH_TRACE, "%d, ", dataBufRead[i]);
	}

	// write all 0xA5 and read it.
	DBG_vPrintf(APP_FLASH_TRACE, "\r\nwrite all 0xA5 and read it. ");
	for(i=0; i<128; i++)
	{
		dataBuf[i] = 0xA5;
	}
	drvFlash.write(USR_FLASH_START_ADDR, dataBuf,     sizeof(dataBuf),     &status);
	drvFlash.read (USR_FLASH_START_ADDR, dataBufRead, sizeof(dataBufRead), &status);
	DBG_vPrintf(APP_FLASH_TRACE, "\r\ndataBufRead[] ");
	for(i=0; i<128; i++)
	{
		DBG_vPrintf(APP_FLASH_TRACE, "%d, ", dataBufRead[i]);
	}
}
