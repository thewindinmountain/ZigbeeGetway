/*
 * app_flash.h
 *
 *  Created on: 2017年10月3日
 *      Author: xubov
 */

#ifndef APP_FLASH_H_
#define APP_FLASH_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
void appFlashInit(void);
void appFlashTest(void);
uint16_t WriteStringToFlash(char *feature_name, int offset, char * data, int size);
uint16_t ReadStringFromFlash(char *feature_name, int offset, char *addr, int size);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


#endif /* APP_FLASH_H_ */
