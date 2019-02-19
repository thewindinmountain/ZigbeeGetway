/*
 * setUpCmd.h
 *
 *  Created on: 201812
 *      Author: xubov
 */

#ifndef SETUPCMD_H_
#define SETUPCMD_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define IP_ADDR_SIZE	16
#define PARM_ACT        1

#define START_SETTING_KEY	"StartSetE"
#define END_SETTING_KEY	    "EndSetE"
#define STOP_WAIT_CMD_TIME  10

//#define USER_DATA_CMD_MQTT_SERVER_ADDR_OFFSET	0
#define USER_DATA_CMD_MQTT_SERVER_ADDR "MQTTSERVERIP"
#define USER_DATA_CMD_MQTT_SERVER_ADDR_LENGTH 128

//#define USER_DATA_WIFI_SSID_OFFSET	(USER_DATA_CMD_MQTT_SERVER_ADDR_OFFSET + USER_DATA_CMD_MQTT_SERVER_ADDR_LENGTH)
#define USER_DATA_WIFI_SSID_CMD	"WIFISSID"
#define USER_DATA_WIFI_SSID_LENGTH 64

//#define USER_DATA_WIFI_PWD_OFFSET	(USER_DATA_WIFI_SSID_OFFSET + USER_DATA_WIFI_SSID_LENGTH)
#define USER_DATA_WIFI_PWD_CMD	"WIFIPWD"
#define USER_DATA_WIFI_PWD_LENGTH 64

#define USER_DATA_HELP	"HELP"
#define USER_DATA_HELP_LENGTH 5


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct
{
	char mqttServerIp[USER_DATA_CMD_MQTT_SERVER_ADDR_LENGTH];
	char wifiSsid[USER_DATA_WIFI_SSID_LENGTH];
	char wifiPwd[USER_DATA_WIFI_PWD_LENGTH];
	char wifiSsidActive;
	char mqttServerIpActive;
	char wifiPwdActive;
	char notUsed[13];
}sysParm; //struct len should be multiple of 16.

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
uint16_t ProcessSetupCmd(char * str);
uint16_t SysParmRewrite(void);
uint16_t SysParmReload(void);
uint16_t ProcessSetupCmd(char * str);
void printCmdHelp(void);
PUBLIC void vDbgPortDataHandle(void);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


#endif /* SETUPCMD_H_ */
