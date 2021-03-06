/*
 * epd_api.h
 *
 *  Created on: 2017-12-5
 *      Author: xubov
 */

#ifndef APP_LCD_H_
#define APP_LCD_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "stdint.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
void epd_init(uint16_t xDot, uint16_t yDot);
void epd_test(void);

void epd_lcd_clear(void);
void epd_lcd_parm_set(uint8 parm, uint8 *data);
void app_lcd_style_set(uint8 table);
void epd_test();
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


#endif /* APP_LCD_H_ */
