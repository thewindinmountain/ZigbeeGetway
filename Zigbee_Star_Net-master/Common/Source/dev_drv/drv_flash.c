/*
 * drv_flash.c
 *
 *  Created on: 2017Äê9ÔÂ29ÈÕ
 *      Author: xubov
 */

#include "jendefs.h"
#include "AppHardwareApi.h"
#include "drv_type.h"
#include "hw_config.h"
#include "dbg.h"
#include "error.h"

#define DRV_FLASH_TRACE TRUE

teFlashChipType        flashType = E_FL_CHIP_INTERNAL;
tSPIflashFncTable      *pCustomFncTable;
uint8 drvInitDone = 0;

void drvDelay(void)
{
	uint32 i;

	for(i = 1000; i > 0; i--);
}

void drvFlashConfig(int32 mask1, int32 mask2)
{
	if(bAHI_FlashInit(flashType, pCustomFncTable))
	{
		drvInitDone = 1;
	}
}

void drvFlashRead(uint32 u32Addr, uint8 *pu8Data, uint16 u16Len, uint8* status)
{
	if(drvInitDone!=1)
	{
		drvFlashConfig(0,0);
	}

	if(drvInitDone!=1)
	{
		*status = ERR_IDLE;
		return;
	}

	if((u32Addr % 0x02) != 0)
	{
		*status = ERR_FAULT;
	}else
	{
		if(bAHI_FullFlashRead(u32Addr, u16Len, pu8Data))
		{
			*status = ERR_OK;
		}
	}
}

void drvFlashWrite(uint32 u32Addr, uint8 *pu8Data, uint16 u16Len, uint8* status)
{
	uint8  sector;

	if(drvInitDone!=1)
	{
		drvFlashConfig(0,0);
	}

	if(drvInitDone!=1)
	{
		*status = ERR_IDLE;
		return;
	}

	if(u16Len%16)
	{
		DBG_vPrintf(DRV_FLASH_TRACE, "\r\nlen %d err, should multiple of 16.", u16Len);
		*status = ERR_FAULT;
		return;
	}

	if((u32Addr % 0x02) != 0)
	{
		*status = ERR_FAULT;
	}else
	{
		sector = u32Addr>>15;
		DBG_vPrintf(DRV_FLASH_TRACE, "\r\nFlashEraseSector %d.", sector);
		if( bAHI_FlashEraseSector(sector))
			DBG_vPrintf(DRV_FLASH_TRACE, "\r\nbAHI_FlashEraseSector success.\r\n");
		else
		{
			DBG_vPrintf(DRV_FLASH_TRACE, "\r\nbAHI_FlashEraseSector failed.\r\n");
			*status = ERR_FAULT;
			return;
		}

		drvDelay();
		if(bAHI_FullFlashProgram(u32Addr, u16Len, pu8Data))
		{
			*status = ERR_OK;
		}else
		{
			DBG_vPrintf(DRV_FLASH_TRACE, "\r\bAHI_FullFlashProgram failed.\r\n");
			*status = ERR_FAULT;
		}
	}
}

memDrvStruct drvFlash =
{
	.open = drvFlashConfig,
	.read = drvFlashRead,
	.write = drvFlashWrite,
};


