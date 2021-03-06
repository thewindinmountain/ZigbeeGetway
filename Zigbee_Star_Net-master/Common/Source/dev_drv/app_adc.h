/*
 * app_adc.h
 *
 *  Created on: 2017年10月3日
 *      Author: xubov
 */

#ifndef APP_ADC_H_
#define APP_ADC_H_

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
int32 i32AppGetVdd(void);
int16 i16AppGetTemp(void);
void app_adc_start_loop_per10s(void);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


#endif /* APP_ADC_H_ */
