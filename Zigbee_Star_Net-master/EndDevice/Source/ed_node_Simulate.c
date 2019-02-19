/*
 * ed_node_Simulate.c
 *
 *  Created on: 2018年1月22日
 *      Author: xubov
 */

#include <jendefs.h>
#include "dbg.h"
#include <AppHardwareApi.h>
#include <Appapi_JN516X.h>
#include "eeprom.h"
#include "node.h"
#include "sleep.h"

uint64 u64CurrentId;

void nodeResetNodeId(uint64 devId)
{
	NODE_sData.u64Address = devId;
	EEPROM_saveNodeParm(&NODE_sData);
}

void nodeSimulate(void)
{
	// reset NODE parm
	NODE_sData.u16Address = 0xffff;
	NODE_sData.eNwkState = NODE_NWKSTATE_NONE;

	// 64Bits address  plus 1.
	NODE_sData.u64Address ++;
	nodeResetNodeId(NODE_sData.u64Address);
	Sleep();
}
