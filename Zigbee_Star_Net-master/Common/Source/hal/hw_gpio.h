/*
 * hw_gpio.h
 *
 *  Created on: 2017年9月26日
 *      Author: xubov
 */

#ifndef HW_GPIO_H_
#define HW_GPIO_H_

#include "jendefs.h"

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#define HW_DIO0                  (0x00000001)
#define HW_DIO1                  (0x00000002)
#define HW_DIO2                  (0x00000004)
#define HW_DIO3                  (0x00000008)
#define HW_DIO4                  (0x00000010)
#define HW_DIO5                  (0x00000020)
#define HW_DIO6                  (0x00000040)
#define HW_DIO7                  (0x00000080)
#define HW_DIO8                  (0x00000100)
#define HW_DIO9                  (0x00000200)
#define HW_DIO10                 (0x00000400)
#define HW_DIO11                 (0x00000800)
#define HW_DIO12                 (0x00001000)
#define HW_DIO13                 (0x00002000)
#define HW_DIO14                 (0x00004000)
#define HW_DIO15                 (0x00008000)
#define HW_DIO16                 (0x00010000)
#define HW_DIO17                 (0x00020000)
#define HW_DIO18                 (0x00040000)
#define HW_DIO19                 (0x00080000)
#define HW_DIOMAX                (0x00100000)

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
void initGpio(uint32 input, uint32 output);
void cfgGpioInput(uint32 mask);
void cfgGpioOutput(uint32 mask);
void setGpio(uint32 lowMask, uint32 highMask);
void getGpio(uint32 mask, uint32 *value);
uint32  u32GetGpio(uint32 mask);
void setGpioPull(uint32 on, uint32 off);

void setDO(uint32 lowMask, uint32 highMask);
void cfgGpioInputWithIrq(uint32 mask, char type);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


#endif /* HW_GPIO_H_ */
