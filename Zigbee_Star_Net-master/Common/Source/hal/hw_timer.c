/*
 * hw_timer.c
 *
 *  Created on: 2017楠烇拷閺堬拷6閺冿拷
 *      Author: xubov
 */
#include "AppHardwareApi_JN516x.h"
#include "hw_config.h"

void initPwm(void)
{
	vAHI_TimerConfigureOutputs(E_AHI_TIMER_1, TRUE, TRUE); //GPIO11
	vAHI_TimerConfigureOutputs(E_AHI_TIMER_2, TRUE, TRUE); //GPIO12
	vAHI_TimerConfigureOutputs(E_AHI_TIMER_3, TRUE, TRUE); //GPIO13

}

void pwmSet(uint8 port, uint16 Hi, uint16 Lo)
{
	switch(port)
	{
	case 0: vAHI_TimerStartRepeat(E_AHI_TIMER_0, Hi, Lo);break;
	case 1: vAHI_TimerStartRepeat(E_AHI_TIMER_1, Hi, Lo);break;
	case 2: vAHI_TimerStartRepeat(E_AHI_TIMER_2, Hi, Lo);break;
	case 3: vAHI_TimerStartRepeat(E_AHI_TIMER_3, Hi, Lo);break;
	case 4: vAHI_TimerStartRepeat(E_AHI_TIMER_4, Hi, Lo);break;
	default: break;
	}
}

void pwmStop(uint8 port)
{
	switch(port)
	{
	case 0: vAHI_TimerStop(E_AHI_TIMER_0); break;
	case 1: vAHI_TimerStop(E_AHI_TIMER_1); break;
	case 2: vAHI_TimerStop(E_AHI_TIMER_2); break;
	case 3: vAHI_TimerStop(E_AHI_TIMER_3); break;
	case 4: vAHI_TimerStop(E_AHI_TIMER_4); break;
	default: break;
	}
}

void pwmEn(uint8 port)
{
	switch(port)
	{
	case 0: vAHI_TimerEnable(E_AHI_TIMER_0,2,FALSE,FALSE,TRUE); break;
	case 1: vAHI_TimerEnable(E_AHI_TIMER_1,2,FALSE,FALSE,TRUE); break;
	case 2: vAHI_TimerEnable(E_AHI_TIMER_2,2,FALSE,FALSE,TRUE); break;
	case 3: vAHI_TimerEnable(E_AHI_TIMER_3,2,FALSE,FALSE,TRUE); break;
	case 4: vAHI_TimerEnable(E_AHI_TIMER_4,2,FALSE,FALSE,TRUE); break;
	default: break;
	}
}

