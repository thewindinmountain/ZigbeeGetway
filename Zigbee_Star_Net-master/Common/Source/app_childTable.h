/*
 * app_childTable.h
 *
 *  Created on: 2017-8-4
 *      Author: xubov
 */

#ifndef APP_CHILDTABLE_H_
#define APP_CHILDTABLE_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "config.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#pragma pack(push)
#pragma pack(2)
typedef struct
{
    uint32 u32ExtAddrL;
    uint32 u32ExtAddrH;
    uint16 u16ShortAddr;
} tsNodeAddr;

typedef struct
{
    /* Data related to associated end devices */
    uint32  		u32ScanChannels;
    uint16          u16Children;
    tsNodeAddr         asChild[MAX_CHILDREN];
} tsNodeAddrTable;
#pragma pack(pop)

typedef struct
{
//	uint8 u8TableStatus[3];
	uint8 u8ChildNum;
}subTableStatus;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC uint16 u16ChildTableReload(void);
PUBLIC bool_t bChildTableCheckFull(void);
PUBLIC bool_t bChildTableSearch(tsNodeAddr *, uint16 *);
PUBLIC bool_t bChildTableSearchByShort(uint16 , uint16 *);
PUBLIC uint16 u16ChildTableAdd(tsNodeAddr *, uint16 *);
PUBLIC bool_t bChildTableRewrite(tsNodeAddr *, uint16);
PUBLIC bool_t bChildTableRead(tsNodeAddr *, uint16);
PUBLIC bool_t bChildTableReadAndDeltetLastChild(tsNodeAddr *);
PUBLIC bool_t bChildTableDelete(tsNodeAddr *);
PUBLIC bool_t bChildTableDeleteAll(void);
PUBLIC uint16 u16GetChildShortAddr(uint16);
PUBLIC uint16 bChildTableNumRead(void);
PUBLIC uint16 bChildTableSerialize(uint8* data);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


#endif /* APP_CHILDTABLE_H_ */
