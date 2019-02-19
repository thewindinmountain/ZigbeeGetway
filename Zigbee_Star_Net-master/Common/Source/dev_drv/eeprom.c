/*
 * eeprom.c
 *
 *  Created on: 2017-7-13
 *      Author: xubov
 */
#include <jendefs.h>
#include <AppHardwareApi.h>
#include "eeprom.h"
#include "node.h"
#include "dbg.h"
#include "config.h"
#include "string.h"

#ifdef DEBUG_EEPROM
#define EEPROM_TRACE TRUE
#else
#define EEPROM_TRACE FALSE
#endif

#define SUCCESS 0
#define EEPROM_ERR 1

typedef struct
{
	uint64 	  		u64Address;		/**< Address of node */
	uint64	  		u64Parent;		/**< Address of node's parent */
	uint64          u64ExtendId;    /**< Extend PAN ID of network add by xubh @2017-7-21*/
	uint32 	  		u32Timer;		/**< Internal timer */
	uint32 	  		u32TimerSync;	/**< Synchronised timer */
	uint16    		u16Address;		/**< Address of node */
	uint16    		u16Parent;		/**< Address of node's parent */
	uint16 	  		u16PanId;		/**< PAN ID of network */
	uint8      	 	u8Channel;		/**< Channel of network */
	uint8			u8SeqTx;		/**< Transmit sequence number */
	uint8			u8DefPibMinBe;	/**< Default minimum be */
	uint8			u8NodType;		/**< define node type END or PAN */
	bool_t	      	bHighPower;		/**< High Power module flag */
	NODE_teNwkState eNwkState;		/**< Network state */

	uint8			u8CurImageSector;
}node_tsParm;

//0 - success
//1 - failure
int EEPROM_deleteAllRecord(void)
{
  uint16 i;
  uint16 resault = 0;

  for(i = 0; i < MAX_SECTION; i ++)
  {
	  resault = iAHI_EraseEEPROMsegment(i);
  }
  return resault;
}

int EEPROM_deleteRecord(uint8 section)
{
  uint16 resault = 0;

  if(section < MAX_SECTION)
  {
	  resault = iAHI_EraseEEPROMsegment(section);
  }
  return resault;
}

int EEPROM_read(uint8* data, uint16 len, uint8 section, uint16 offset)
{
	return iAHI_ReadDataFromEEPROMsegment(section, (uint8)offset, data, len);
}

int EEPROM_write(uint8* data, uint16 len, uint8 section, uint16 offset)
{
	return iAHI_WriteDataIntoEEPROMsegment(section, (uint8)offset, data, len);
}

#define PARM_ACTIVE 0xA5
int EEPROM_saveNodeParm(NODE_tsData *NODE_sData)
{
	uint8 parmActive = PARM_ACTIVE;
	int returnValue = SUCCESS;
	node_tsParm nodeDataBuf;

//	nodeDataBuf.u64Address = NODE_sData->u64Address;
	nodeDataBuf.u16Address = NODE_sData->u16Address;
	nodeDataBuf.u16PanId   = NODE_sData->u16PanId;
	nodeDataBuf.u16Parent  = NODE_sData->u16Parent;
	nodeDataBuf.u8Channel  = NODE_sData->u8Channel;
	nodeDataBuf.bHighPower = NODE_sData->bHighPower;
	nodeDataBuf.eNwkState  = NODE_sData->eNwkState;
	nodeDataBuf.u8NodType  = NODE_sData->u8NodType;

	if (SUCCESS != EEPROM_write((uint8*)&nodeDataBuf,
	                            sizeof(node_tsParm),
	                            PDM_ID_NODE_PARM,
								2) )
	{
		DBG_vPrintf(EEPROM_TRACE, "\r\n EEPROM write PDM_ID_NODE_CFG parm ERR");
	}else if (SUCCESS != EEPROM_write(&parmActive, 
	                            1,
	                            PDM_ID_NODE_PARM,
								0) )
	{
		DBG_vPrintf(EEPROM_TRACE, "\r\n EEPROM write PDM_ID_NODE_CFG act ERR");
	}

	return returnValue;
}

int EEPROM_readNodeParm(NODE_tsData *NODE_sData)
{
	uint8 parmActive;
	node_tsParm nodeDataBuf;
	int returnValue = SUCCESS;

	if (SUCCESS != EEPROM_read(&parmActive, 1, PDM_ID_NODE_PARM, 0))
	{
		returnValue = EEPROM_ERR; 
		DBG_vPrintf(EEPROM_TRACE, "\r\n EEPROM read PDM_ID_NODE_CFG active ERR");
	}
	
	if (PARM_ACTIVE == parmActive)
	{
		if(SUCCESS != EEPROM_read((uint8 *)&nodeDataBuf,
		                          sizeof(node_tsParm),
		                          PDM_ID_NODE_PARM,
								  2))
		{
			DBG_vPrintf(EEPROM_TRACE, "\r\n EEPROM read PDM_ID_NODE_CFG parm ERR");
		}else
		{
//			NODE_sData->u64Address = nodeDataBuf.u64Address;
			NODE_sData->u16Address = nodeDataBuf.u16Address;
			NODE_sData->u16PanId   = nodeDataBuf.u16PanId;
			NODE_sData->u16Parent  = nodeDataBuf.u16Parent;
			NODE_sData->u8Channel  = nodeDataBuf.u8Channel;
			NODE_sData->bHighPower = nodeDataBuf.bHighPower;
			NODE_sData->eNwkState  = nodeDataBuf.eNwkState;
			NODE_sData->u8NodType  = nodeDataBuf.u8NodType;
		}
	}else
	{
		DBG_vPrintf(EEPROM_TRACE, "\r\n EEPROM PDM_ID_NODE_CFG unactive");
	}

	return returnValue;//success
}

int EEPROM_cleanNodeParm(void)
{
	return EEPROM_deleteRecord(PDM_ID_NODE_PARM);
}

void EEPROM_init(void)
{
	uint8 p_u8EepromSegDataLen[64];

	u16AHI_InitialiseEEP(p_u8EepromSegDataLen);
	//iAHI_ReadDataFromEEPROMsegment(0, EEPROM_DEV_CFG,p_u8EepromSegDataLen,10);
}


void EEPROM_WR_test(void)
{
	uint8 dataBuf[64];
	uint8 dataBuf1[64];
	uint8 i;
	for(i = 0; i < 64; i++)
	{
		dataBuf[i] = i;
		dataBuf1[i] = 0;
	}

	EEPROM_write(dataBuf, 10, PDM_ID_APP_ROUTER, 0);
	EEPROM_read(dataBuf1, 10, PDM_ID_APP_ROUTER, 0);

	for(i = 0; i < 10; i++)
	{
		DBG_vPrintf(EEPROM_TRACE, "\r\n N[%d] %x =? %x", i, dataBuf[i], dataBuf1[i]);
	}
}



