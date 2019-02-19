/*
 * sleep.c
 *
 *  Created on: 2017年8月19日
 *      Author: xubov
 */
#include "dbg.h"
#include <jendefs.h>
//#include "usr_Uart.h"
#include "ledControl.h"
#include <AppHardwareApi.h>
#include "config.h"
#include "node.h"
#include "usr_Timer.h"

#define TRACE

#ifdef TRACE
#define SLEEP_TRACE TRUE
#else
#define SLEEP_TRACE FALSE
#endif

PRIVATE uint64 u64KeepAwake = KEEP_AWAKE_LONG_TIMER;
PRIVATE uint64 u64KeepSleep = NORMAL_SLEEP;

PUBLIC bool bSleepAwakeTimeDec(uint32 time)
{
	bool bZero = FALSE;

	if(u64KeepAwake > time)
	{
		u64KeepAwake -= time;
	}else
	{
		u64KeepAwake = 0;
		bZero = TRUE;
	}

	return bZero;
}

PUBLIC void vSleepAwakeTimeAdd(uint64 time)
{
	DBG_vPrintf(SLEEP_TRACE, "\r\n%d vSleepAwakeTimeAdd %d", NODE_sData.u32Timer, time);
	u64KeepAwake += time;
}

PUBLIC void vSleepAwakeTimeSet(uint64 time)
{
	DBG_vPrintf(SLEEP_TRACE, "\r\n%d vSleepAwakeTimeSet %d", NODE_sData.u32Timer, time);
	u64KeepAwake = time;
}

PUBLIC void vSleepKeepTimeSet(uint64 time)
{
	DBG_vPrintf(SLEEP_TRACE, "\r\n%d vSleepKeepTimeSet %d", NODE_sData.u32Timer, time);
	u64KeepSleep = time;
}

void Sleep(void)
{
	uint32 time = (uint32)(u64KeepSleep/32);
	DBG_vPrintf(SLEEP_TRACE, "\r\n%d Sleep %d ms ", NODE_sData.u32Timer, time);

	/* SPI disabling needs to happen first */
	vAHI_SpiDisable();

	vAHI_UartDisable(DBG_UART);
//	vAHI_UartDisable(USR_UART);

//	/* Turn off LEDs */
//	vLedControl(0, FALSE);
//	vLedControl(1, FALSE);

	/* Enable and start wake timer */
	vAHI_WakeTimerEnable(E_AHI_WAKE_TIMER_0, TRUE);
	vAHI_WakeTimerStartLarge(E_AHI_WAKE_TIMER_0, u64KeepSleep);
	/* Sleep without RAM forcing a cold start and full scan when we wake */
	vAHI_Sleep(E_AHI_SLEEP_OSCON_RAMOFF);

	/* Loop until we sleep */
	while(1);
}

void goToSleep(void)
{
	Sleep();
}

uint8 sleepTimerId = 0;
void sleepSet(void)
{
	usrTimerOpen(&sleepTimerId, goToSleep);
	usrTimerStart(&sleepTimerId, 600);
}

void sleepReconfig(uint32 period)
{
	usrTimerOpen(&sleepTimerId, goToSleep);
	usrTimerStart(&sleepTimerId, period);
}

PUBLIC void TASK_vSleepCtrl(void)
{
	if(bSleepAwakeTimeDec(1))
	{
		/* Debug */
		DBG_vPrintf(SLEEP_TRACE, "\r\n%d TASK_vSleepCtrl Go to sleep", NODE_sData.u32Timer);
		Sleep();
	}
}
