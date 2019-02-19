/*
 * usr_timer.c
 *
 *  Created on: 2017年10月4日
 *      Author: xubov
 */
#include <jendefs.h>
#include <AppHardwareApi.h>
#include <AppQueueApi.h>
#include "hw_config.h"
#include "usr_timer.h"
#include "dbg.h"

stUsrTimer usrTimer[USR_TIMERS_NUM];
#define TIMER_IDLE 0
#define TIMER_BUSY 1

#define TRACE_UST_TIMER TRUE

void usrTimerInit(void)
{
	uint8 i;
	for(i = 0; i < USR_TIMERS_NUM; i++ )
	{
		usrTimer[i].enable = FALSE;
		usrTimer[i].handle = NULL;
		usrTimer[i].status = TIMER_IDLE;
		usrTimer[i].timerCounter = 0;
		usrTimer[i].timerPeriod = 0;
		usrTimer[i].timerId = i;
	}
}

void usrTimerOpen(uint8 * timerId, void (*handle)(void))
{
	uint8 i;

	if(* timerId != 0) /* not NULL */
	{
		usrTimer[* timerId].status = TIMER_BUSY;
		usrTimer[* timerId].enable = FALSE;
		usrTimer[* timerId].handle = handle;
		usrTimer[* timerId].timerCounter = 0;
		return;
	}

	for(i = 1; i < USR_TIMERS_NUM; i++ ) // i = 0 not use.
	{
		if(TIMER_IDLE == usrTimer[i].status)
		{
			usrTimer[i].status = TIMER_BUSY;
			*timerId = usrTimer[i].timerId;
			usrTimer[i].enable = FALSE;
			usrTimer[i].handle = handle;
			usrTimer[i].timerCounter = 0;
			return;
		}
	}
}

void usrTimerClose(uint8 * timerId)
{
	usrTimer[*timerId].status = TIMER_IDLE;
	usrTimer[*timerId].enable = FALSE;
	usrTimer[*timerId].handle = NULL;
	usrTimer[*timerId].timerCounter = 0;
	* timerId = 0;
}


void usrTimerStop(uint8 * timerId)
{
	if(*timerId < USR_TIMERS_NUM)
	{
		usrTimer[*timerId].enable = FALSE;
	}
}

void usrTimerStart(uint8 * timerId, uint32 period)
{
	if(*timerId < USR_TIMERS_NUM && *timerId > 0)
	{
		usrTimer[*timerId].enable = TRUE;
		usrTimer[*timerId].timerCounter = period;
		usrTimer[*timerId].timerPeriod = period;
	}
}

void usrTimerLoop(void)
{
	uint8 i;
	for(i = 0; i < USR_TIMERS_NUM; i++ )
	{
		if(TRUE == usrTimer[i].enable)
		{
			if(0 == usrTimer[i].timerCounter--)
			{
				if(NULL != usrTimer[i].handle)
				{
					usrTimer[i].handle();
				}
				usrTimer[i].timerCounter = usrTimer[i].timerPeriod;
			}
		}
	}
}

