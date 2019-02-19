/*
 * drv_rgb.h
 *
 *  Created on: 2018年10月14日
 *      Author: xubov
 */

#ifndef DRV_RGB_H_
#define DRV_RGB_H_


/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "jendefs.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

// DIO line values



/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC bool_t bRGB_LED_Enable(void);
PUBLIC bool_t bRGB_LED_Disable(void);
PUBLIC bool_t bRGB_LED_SetLevel(uint8 u8RedLevel, uint8 u8GreenLevel, uint8 u8BlueLevel);
PUBLIC bool_t bRGB_LED_On(void);
PUBLIC bool_t bRGB_LED_Off(void);
PUBLIC bool_t bRGB_LED_SetGroupLevel(uint8 u8Level);
void rgb_test();

#endif /* DRV_RGB_H_ */
