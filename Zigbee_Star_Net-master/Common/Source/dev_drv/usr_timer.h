/*
 * usr_timer.h
 *
 *  Created on: 2017年10月4日
 *      Author: xubov
 */

#ifndef USR_TIMER_H_
#define USR_TIMER_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <app_timer_driver.h>
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define USR_TIME_MS  APP_TIME_MS
#define USR_TIME_SEC APP_TIME_SEC

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct
{
	uint8  timerId;
	uint8  status;
	uint32 timerCounter;
	uint32 timerPeriod;
	uint8  enable;
	void (*handle)(void);
}stUsrTimer;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
void usrTimerInit(void);
void usrTimerOpen(uint8 * timerId, void (*handle)(void));
void usrTimerClose(uint8 * timerId);
void usrTimerStop(uint8 * timerId);
void usrTimerStart(uint8 * timerId, uint32 timerCounter);
void usrTimerLoop(void);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#endif /* USR_TIMER_H_ */
