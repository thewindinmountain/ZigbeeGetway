/*
 * app_childTable.c
 *
 *  Created on: 2017-8-4
 *      Author: xubov
 */
#include <jendefs.h>
#include <AppHardwareApi_JN516X.h>
#include "app_childTable.h"
#include "dbg.h"
#include "config.h"


#ifdef DEBUG_APP_CHILDTABLE
#define CH_TABLE_TRACE TRUE
#else
#define CH_TABLE_TRACE FALSE
#endif

#define PER_CHILDREN_LEN sizeof(tsNodeAddr)
//#define PER_CHILDREN_LEN 10

/* Note */
/* u16AHI_InitialiseEEP() functin should be before calling in init function*/

#define EEP_SUCCESS       0
#define EEP_ERR_READ      1
#define EEP_ERR_WRITE     2
#define EEP_ERR_ERASE     3
#define EEP_ERR_OUT_RANGE 4

PRIVATE tsNodeAddrTable childTable;

PRIVATE bool_t bChildTableInit = FALSE;

PUBLIC uint16 bChildTableSerialize(uint8* data)
{
	if(data==NULL){
		DBG_kprintf("\r\ndata null\r\n");
		return 2;
	}
	if(childTable.u16Children==0){
		DBG_kprintf("\r\nChildtable null\r\n");
		return 1;
	}
	memcpy(data,(void*)&childTable,6+sizeof(tsNodeAddr)*childTable.u16Children);
	return 0;
}

PUBLIC uint16 bChildTableNumRead(void)
{
	return childTable.u16Children;
}

PUBLIC uint16 u16ChildTableReload(void)
{
	uint16 u16SegmentIndex;
	subTableStatus segStatus;
	uint16 errStatus = 0xFFFF;

	childTable.u16Children = 0;

	for(u16SegmentIndex = CHILD_TABLE_MIN_SEG; u16SegmentIndex <= CHILD_TABLE_MAX_SEG; u16SegmentIndex++)
	{
		errStatus = iAHI_ReadDataFromEEPROMsegment(u16SegmentIndex,
			        CHILD_TABLE_SEGMENT_STATUS_OFFSET,
			        (uint8*)&segStatus,
			        sizeof(subTableStatus));

		if(EEP_SUCCESS == errStatus)
		{
			if( segStatus.u8ChildNum == 0                                        ||
			    CHILD_TABLE_SEGMENT_LEN < (sizeof(tsNodeAddr) * segStatus.u8ChildNum) )
			{
				errStatus = u16SegmentIndex;
				DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableReload segment %d has no child, segStatus.u8ChildNum=%d.",
						u16SegmentIndex, segStatus.u8ChildNum);
				break;
			}

			if( (childTable.u16Children + segStatus.u8ChildNum) >= MAX_CHILDREN )
			{
				errStatus = EEP_ERR_OUT_RANGE;
				DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableReload out of range at seg %d, childTable.u16Children=%d, segStatus.u8ChildNum=%d.",
						u16SegmentIndex, childTable.u16Children, segStatus.u8ChildNum);
				break;
			}

			errStatus = iAHI_ReadDataFromEEPROMsegment(u16SegmentIndex,
				        CHILD_TABLE_SEGMENT_TABLE_START,
				        (uint8*)(&childTable.asChild[childTable.u16Children]),
				        sizeof(tsNodeAddr) * segStatus.u8ChildNum);
			if(EEP_SUCCESS == errStatus)
			{
				childTable.u16Children += segStatus.u8ChildNum;
				DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableReload segment %d has done.", u16SegmentIndex);
			}
		}else
		{
			break;
		}
	}

	DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableReload done childTable.u16Children = %d.", childTable.u16Children);

	bChildTableInit = TRUE;

	return errStatus;
}

PUBLIC bool_t bChildTableCheckFull(void)
{
	if(bChildTableInit == FALSE)
	{
		u16ChildTableReload();
	}

	if( childTable.u16Children >= (CHILD_TABLE_MAX_SEG - CHILD_TABLE_MIN_SEG + 1)*PER_CHILDREN_LEN)
	{
		return TRUE;
	}

	return FALSE;
}

bool_t bChildTableSearchByShort(uint16 shortAddr, uint16 *position)
{
	bool_t errValue = FALSE;
	uint16 u16Index;

	for(u16Index = 0; u16Index < childTable.u16Children; u16Index++)
	{
		if(shortAddr != 0xFFFF)
		{
			if(shortAddr == childTable.asChild[u16Index].u16ShortAddr)
			{
				*position = u16Index;
				errValue = TRUE;
				break;
			}
		}
	}

	return errValue;
}

PUBLIC bool_t bChildTableSearch(tsNodeAddr *child, uint16 *position)
{
	bool_t errValue = FALSE;
	uint16 u16Index;

	if(bChildTableInit == FALSE)
	{
		u16ChildTableReload();
	}

	for(u16Index = 0; u16Index < childTable.u16Children; u16Index++)
	{
		if(child->u16ShortAddr != 0xFFFF)
		{
			if(child->u16ShortAddr == childTable.asChild[u16Index].u16ShortAddr)
			{
				*position = u16Index;
				child->u16ShortAddr = childTable.asChild[u16Index].u16ShortAddr;
				child->u32ExtAddrL  = childTable.asChild[u16Index].u32ExtAddrL;
				child->u32ExtAddrH  = childTable.asChild[u16Index].u32ExtAddrH;
				errValue = TRUE;
				break;
			}
		}else
		{
			if(child->u32ExtAddrL  == childTable.asChild[u16Index].u32ExtAddrL  &&
			   child->u32ExtAddrH  == childTable.asChild[u16Index].u32ExtAddrH)
			{
				if(child->u32ExtAddrL != 0)
				{
					*position = u16Index;
					child->u16ShortAddr = childTable.asChild[u16Index].u16ShortAddr;
					child->u32ExtAddrL  = childTable.asChild[u16Index].u32ExtAddrL;
					child->u32ExtAddrH  = childTable.asChild[u16Index].u32ExtAddrH;
					errValue = TRUE;
					break;
				}
			}
		}
	}

	return errValue;
}

PUBLIC uint16 u16ChildTableAdd(tsNodeAddr *newChild, uint16 *index)
{
	uint16 u16Err = EEP_SUCCESS;
	uint16 u16SegmentIndex;
	uint8  u8SegmentByteAddress;
	subTableStatus segStatus;

	if(bChildTableInit == FALSE)
	{
		u16ChildTableReload();
	}

	if(childTable.u16Children >= MAX_CHILDREN)
	{
		u16Err = EEP_ERR_OUT_RANGE;
		DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableAdd: childTable full. childTable.u16Children=%d MAX_CHILDREN=%d", childTable.u16Children, MAX_CHILDREN);
		return u16Err;
	}

	u16SegmentIndex = CHILD_TABLE_MIN_SEG + (childTable.u16Children)/(CHILD_TABLE_SEGMENT_LEN/PER_CHILDREN_LEN);
	u8SegmentByteAddress  = PER_CHILDREN_LEN * ((childTable.u16Children)%(CHILD_TABLE_SEGMENT_LEN/PER_CHILDREN_LEN));

	if(u16SegmentIndex > CHILD_TABLE_MAX_SEG)
	{
		u16Err = EEP_ERR_OUT_RANGE;
		DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableAdd: u16ChildTableAdd, eeprom full u16SegmentIndex=%d", u16SegmentIndex);
		return u16Err;
	}

	if(CHILD_TABLE_SEGMENT_TABLE_START == u8SegmentByteAddress)
	{
		u16Err = iAHI_EraseEEPROMsegment(u16SegmentIndex);
		if(EEP_SUCCESS == u16Err)
		{
			segStatus.u8ChildNum = 1;

		}else
		{
			DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableAdd: iAHI_EraseEEPROMsegment err=%d at u16SegmentIndex = %d, CHILD_TABLE_MIN_SEG=%d.",
					u16Err, u16SegmentIndex, CHILD_TABLE_MIN_SEG);
			return u16Err;
		}
	}else
	{
		u16Err = iAHI_ReadDataFromEEPROMsegment(u16SegmentIndex,
				 CHILD_TABLE_SEGMENT_STATUS_OFFSET,
				 (uint8*)&segStatus,
				 sizeof(subTableStatus));
		if(EEP_SUCCESS == u16Err)
		{
			segStatus.u8ChildNum += 1;
		}else
		{
			DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableAdd: iAHI_ReadDataFromEEPROMsegment err=%d at u16SegmentIndex=%.", u16Err, u16SegmentIndex);
			return u16Err;
		}
	}

	u16Err = iAHI_WriteDataIntoEEPROMsegment(u16SegmentIndex,
			 u8SegmentByteAddress,
			 (uint8*)newChild,
			 sizeof(tsNodeAddr));
	if(EEP_SUCCESS != u16Err)
	{
		DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableAdd: iAHI_WriteDataIntoEEPROMsegment write newChild err=%d.", u16Err);
		DBG_vPrintf(CH_TABLE_TRACE, "u16SegmentIndex = %x; u8SegmentByteAddress=%x; dataLen=%d",
				u16SegmentIndex, u8SegmentByteAddress, sizeof(tsNodeAddr));
		return u16Err;
	}

	u16Err = iAHI_WriteDataIntoEEPROMsegment(u16SegmentIndex,
			 CHILD_TABLE_SEGMENT_STATUS_OFFSET,
			 (uint8*)&segStatus,
			 sizeof(subTableStatus));
	if(EEP_SUCCESS == u16Err)
	{
		*index = childTable.u16Children;
		childTable.asChild[*index].u16ShortAddr = newChild->u16ShortAddr;
		childTable.asChild[*index].u32ExtAddrH  = newChild->u32ExtAddrH;
		childTable.asChild[*index].u32ExtAddrL  = newChild->u32ExtAddrL;

		childTable.u16Children += 1;

		DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableAdd Done at childTable.asChild[%d].at u16SegmentIndex=%d, u8SegmentByteAddress=%d, segStatus.u8ChildNum=%d",
				*index, u16SegmentIndex, u8SegmentByteAddress, segStatus.u8ChildNum);
	}
	else
	{
		DBG_vPrintf(CH_TABLE_TRACE, "\r\n TableAdd: iAHI_WriteDataIntoEEPROMsegment write segStatus err=% at u16SegmentIndex=%d.", u16Err, u16SegmentIndex);
	}

	return u16Err;
}

PUBLIC bool_t bChildTableRewrite(tsNodeAddr *child, uint16 position)
{
	bool_t errValue = FALSE;

	if(position >= childTable.u16Children)
	{
		return errValue;
	}

	uint16 u16SegmentIndex = position/PER_CHILDREN_LEN;
	uint8 u8SegmentByteAddress = (position%PER_CHILDREN_LEN) * sizeof(tsNodeAddr);
	if(0 == iAHI_WriteDataIntoEEPROMsegment(u16SegmentIndex, u8SegmentByteAddress, (uint8*)child, sizeof(tsNodeAddr)))
	{
		childTable.asChild[position].u16ShortAddr = child->u16ShortAddr;
		childTable.asChild[position].u32ExtAddrH  = child->u32ExtAddrH;
		childTable.asChild[position].u32ExtAddrL  = child->u32ExtAddrL;
		errValue = TRUE;
	}

	return errValue;
}

PUBLIC bool_t bChildTableRead(tsNodeAddr *child, uint16 position)
{
	bool_t errValue = FALSE;

	child->u16ShortAddr = childTable.asChild[position].u16ShortAddr;
	child->u32ExtAddrH  = childTable.asChild[position].u32ExtAddrH;
	child->u32ExtAddrL  = childTable.asChild[position].u32ExtAddrL;

	return errValue;
}

PUBLIC bool_t bChildTableReadAndDeltetLastChild(tsNodeAddr *child)
{
	bool_t errValue = FALSE;

	if(childTable.u16Children == 0)
	{
		return errValue;
	}

	uint16 u16SegmentIndex = (childTable.u16Children-1)/PER_CHILDREN_LEN;
	uint8 u8SegmentByteAddress = ((childTable.u16Children-1)%PER_CHILDREN_LEN) * sizeof(tsNodeAddr);

	if(0 == iAHI_ReadDataFromEEPROMsegment(u16SegmentIndex,u8SegmentByteAddress, (uint8*)child, sizeof(tsNodeAddr)))
	{
		subTableStatus segStatus;
		iAHI_ReadDataFromEEPROMsegment(u16SegmentIndex,
									   CHILD_TABLE_SEGMENT_STATUS_OFFSET,
									   (uint8*)&segStatus,
									   sizeof(subTableStatus));
		segStatus.u8ChildNum -= 1;
		iAHI_WriteDataIntoEEPROMsegment(u16SegmentIndex,
										CHILD_TABLE_SEGMENT_STATUS_OFFSET,
										(uint8*)&segStatus,
										sizeof(subTableStatus));
		childTable.u16Children -= 1;

		errValue = TRUE;

	}

	return errValue;
}

PUBLIC bool_t bChildTableDelete(tsNodeAddr *child)
{
	bool_t errValue = FALSE;

	uint16 position = 0;

	// check the child was in the table.
	// if not return as true;
	if( bChildTableSearch(child, &position) == FALSE)
	{
		errValue = TRUE;
		return errValue;
	}

	tsNodeAddr tmpChild;
	//read and delete the last child
	if( bChildTableReadAndDeltetLastChild(&tmpChild) == FALSE)
	{
		return errValue;
	}
	if( position == 0) //if the last one.
	{
		errValue = TRUE;
		return errValue;
	}

	//write the last child into the position of the child should be deleted.
	if( bChildTableRewrite(&tmpChild, position) == TRUE )
	{
		errValue = TRUE;
		return errValue;
	}

	return errValue;
}

PUBLIC bool_t bChildTableDeleteAll(void)
{
	bool_t errValue = FALSE;
	uint16 u16SegmentIndex = 0;

	subTableStatus segStatus;
	segStatus.u8ChildNum = 0;

	for(u16SegmentIndex = CHILD_TABLE_MIN_SEG; u16SegmentIndex <= CHILD_TABLE_MAX_SEG; u16SegmentIndex ++)
	{
		iAHI_WriteDataIntoEEPROMsegment(u16SegmentIndex,
									    CHILD_TABLE_SEGMENT_STATUS_OFFSET,
									    (uint8*)&segStatus,
									    sizeof(subTableStatus));
	}

	childTable.u16Children = 0;

	return errValue;
}

PUBLIC uint16 u16GetChildShortAddr(uint16 index)
{
	if(index < MAX_CHILDREN)
	{
		return childTable.asChild[index].u16ShortAddr;
	}
	else
		return 0xFFFF;
}
