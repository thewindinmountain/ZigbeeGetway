/*
 * common_init.c
 *
 *  Created on: 2018/10/31
 *      Author: xubov
 */

#include <jendefs.h>
#include <AppQueueApi.h>
#include <AppHardwareApi.h>
#include "eeprom.h"
#include "node.h"

void common_init(void)
{
	/* Setup interface to MAC */
    (void)u32AppQApiInit(NULL, NULL, NULL);
    (void)u32AHI_Init();

	EEPROM_init();

	/* clean parm when debug */
//	EEPROM_cleanNodeParm();
	/* Read back node parm */
	EEPROM_readNodeParm(&NODE_sData); //it should follow EEPROM_init

}
