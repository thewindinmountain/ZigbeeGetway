/*
 * setUpCmd.c
 *
 *  Created on: 2018Äê1ÔÂ2ÈÕ
 *      Author: xubov
 */
#include <jendefs.h>
#include "error.h"
#include "string.h"
#include "setUpCmd.h"
#include "dbg.h"
#include "app_flash.h"
#include "delay.h"

#define TRACE_CMD TRUE

sysParm sysConfigParm;
uint8   sysParmNeedWr = 0;

#define SPLIT_CMD_CHAR	'='
uint16_t SplitCmd(char *str, char ** cmd, char ** data)
{
	uint16_t result = ERR_OK;
	char * ch = str;
	while(*ch != SPLIT_CMD_CHAR && *ch)
		ch ++;
	if(*ch == SPLIT_CMD_CHAR)
	{
		*cmd=str;
		*ch ='\0';
		*data= ch + 1;
		result = ERR_OK;
	}
	else
	{
		result = ERR_VALUE;
	}
	return result;
}

void printCmdHelp(void)
{
	DBG_vPrintf(TRACE_CMD, "%s=<ip address>\r\n", USER_DATA_CMD_MQTT_SERVER_ADDR);
	DBG_vPrintf(TRACE_CMD, "%s=<ap name>\r\n", USER_DATA_WIFI_SSID_CMD);
	DBG_vPrintf(TRACE_CMD, "%s=<ap pass word>\r\n", USER_DATA_WIFI_PWD_CMD);
}

uint16_t ProcessSetupCmd(char * str)
{
	uint16_t result = ERR_OK;
	char * cmd, * data;

	if(!str)
	{
		DBG_vPrintf(TRACE_CMD, "In ProcessSetupCmd: cmd is null!\n");
		result = ERR_VALUE;
	}
	result = SplitCmd(str, &cmd, &data);
	if(result!=ERR_OK)
	{
		if(strstr(str, USER_DATA_HELP))
		{
			printCmdHelp();
		}else
			DBG_vPrintf(TRACE_CMD, "Split cmd failed!\n");
	}
	else
	{
		DBG_vPrintf(TRACE_CMD, "Start process command:%s data:%s\n", cmd, data);

		if(!strcmp(cmd, USER_DATA_CMD_MQTT_SERVER_ADDR))
		{
			if(strlen(data) > IP_ADDR_SIZE)
				DBG_vPrintf(TRACE_CMD, "ip data is too long");

			if(strlen(data) > USER_DATA_CMD_MQTT_SERVER_ADDR_LENGTH)
			{
				DBG_vPrintf(TRACE_CMD, "parm data is too long %d.\n", strlen(data));
			}else
			{
				memcpy(sysConfigParm.mqttServerIp, data, USER_DATA_CMD_MQTT_SERVER_ADDR_LENGTH);
				sysConfigParm.mqttServerIpActive = PARM_ACT;
				sysParmNeedWr = 1;
//				SysParmRewrite();
//				SysParmReload();
			}
		}

		if(!strcmp(cmd, USER_DATA_WIFI_SSID_CMD))
		{
			if(strlen(data) > USER_DATA_WIFI_SSID_LENGTH)
				DBG_vPrintf(TRACE_CMD, "parm data is too long.\n");
			else
			{
				memcpy(sysConfigParm.wifiSsid, data, USER_DATA_WIFI_SSID_LENGTH);
				sysConfigParm.wifiSsidActive = PARM_ACT;
				sysParmNeedWr = 1;
//				SysParmRewrite();
//				SysParmReload();
			}
		}

		if(!strcmp(cmd, USER_DATA_WIFI_PWD_CMD))
		{
			if(strlen(data) > USER_DATA_WIFI_PWD_LENGTH)
				DBG_vPrintf(TRACE_CMD, "parm data is too long.\n");
			else
			{
				memcpy(sysConfigParm.wifiPwd, data, USER_DATA_WIFI_PWD_LENGTH);
				sysConfigParm.wifiPwdActive = PARM_ACT;
				sysParmNeedWr = 1;
//				SysParmRewrite();
//				SysParmReload();
			}
		}
	}

	return result;
}


uint16_t SysParmRewrite(void)
{
	return WriteStringToFlash("parm rewrite", 0, (char*)&sysConfigParm, sizeof(sysParm));
}

uint16_t SysParmReload(void)
{
	return ReadStringFromFlash("parm reload", 0, (char*)&sysConfigParm, sizeof(sysParm));
}


int g_start_setting_mode=0;
PUBLIC void vDbgPortDataHandle(void)
{
	char   rxBuf[1024];
	uint16 rxDataLen;
	uint16 delayCount = STOP_WAIT_CMD_TIME;

	DBG_vPrintf(TRACE_CMD, "\r\n wait %ds for input \"StartSetE\" to go to set Mode...",STOP_WAIT_CMD_TIME);
	DBG_vPrintf(TRACE_CMD, "\r\n input \"HELP\" for help msg");
	DBG_vPrintf(TRACE_CMD, "\r\n input a signal char to skip the delay.");

	while(delayCount--)
	{
		delay_xms(1000);
		DBG_vPrintf(TRACE_CMD, "...%d ", delayCount);


		memset(rxBuf, 0, 1024);
		vDbgUartRx((uint8*)rxBuf, &rxDataLen);

		if(rxDataLen > 0)
		{
			if(strstr(rxBuf,START_SETTING_KEY))
			{
				g_start_setting_mode=1;
				DBG_vPrintf(TRACE_CMD, "\r\n >>>>>>>>>>>>>>>>>>>>>>>>>>>CRD enter set mode");
				break;
			}else
			if(strstr(rxBuf,USER_DATA_HELP))
			{
				printCmdHelp();
				delayCount = STOP_WAIT_CMD_TIME;
			}else
			{
				DBG_vPrintf(TRACE_CMD, "\r\n Err input \"HELP\" for help msg");
			}
		}

		if(rxDataLen == 1)
		{
			break;
		}
	}

	if(g_start_setting_mode==1)
	{
		while(1)
		{
			memset(rxBuf, 0, 1024);
			vDbgUartRx((uint8*)rxBuf, &rxDataLen);
			if(rxDataLen > 0)
			{
				if(strstr(rxBuf, END_SETTING_KEY))
				{
					g_start_setting_mode = 0;
					break;
				}

				if(ERR_OK != ProcessSetupCmd(rxBuf))
				{
					DBG_vPrintf(TRACE_CMD, "\r\n input \"EndSetE\" to leave set mode.");
					DBG_vPrintf(TRACE_CMD, "\r\n input \"HELP\" for help msg\r\n");
				}
			}
		}
	}

	DBG_vPrintf(TRACE_CMD, "\r\n leave setMode...");
}
