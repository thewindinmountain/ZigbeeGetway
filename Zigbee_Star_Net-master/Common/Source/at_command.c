/*
 * at_command.c
 *
 *  Created on: 2019年1月26日
 *      Author: Administrator
 */
#include "at_command.h"
#include "dbg.h"
#include <jendefs.h>
#include "app_childTable.h"
#include "uart.h"
#include "usr_Uart.h"
#include "node.h"

#ifndef DEBUG_CRD
#define AT_CRD_TRACE FALSE
#else
#define AT_CRD_TRACE TRUE
#endif

#define DATA_TRANS_OK "DT"
/**
 * 将数据打包成一个response
 *
 *
 */
PRIVATE int data_resp(char* cmd, uint8* data, uint16 len, uint8 fid) {
	response_data resp;
	resp.start_delimiter = 0x7E;

	resp.len_l = len;
	resp.len_h = len>>8;
	resp.type = AT_RESPONSE_FRAME;
	resp.id = fid;
	resp.cmd[0] = cmd[0];
	resp.cmd[1] = cmd[1];
	resp.status = AT_RESP_OK;
	resp.data = data;
	//TODO 这个地方在后期应该算出来
	resp.checksum = 0xF0;
	vUsrUartTx(&resp, (uint16) 8);
	if (data != NULL && len != 0) {
		vUsrUartTx(data, len);
	}
	vUsrUartTx((uint8*) (&resp.checksum), 1);
	return 0;
}

PRIVATE int cmd_resp(char* cmd, uint8 fid) {
	return data_resp(cmd, NULL, 0, fid);
}

/**
 * 查询所有的EndDevice，并封装到一个response中
 */
PRIVATE int childList2response() {

	uint16 childnum, ret;
	uint8 data[3006];

	childnum = bChildTableNumRead();
	if (childnum == 0) {
		DBG_vPrintf(AT_CRD_TRACE, "\r\n child num is 0\r\n");
		return 1;
	}
	ret = bChildTableSerialize(data);
	//len=childnum*10+6
	data_resp("CL", data, childnum * 10 + 6, 0x01);

	return ret;
}

PRIVATE int transdata2child(uint8* data) {
	uint64 addr;
	uint16 len;

	AT_FREAM_LEN(data, len);
	AT_FREAM_TRANS_DATA_LEN(len);
	AT_FREAM_ADDR16(data, addr);

	NODE_vTx(addr, len, AT_FREAM_TRANS_DATA(data), 0xFF);
	return 0;
}

PRIVATE int cmpcmd(uint8* cmd1, uint8* cmd2) {
	return cmd1[0] == cmd2[0] && cmd1[1] == cmd2[1];
}

/**
 * 根据查询的命令，将查询结果通过串口输出
 */
PRIVATE int exec_select(uint8* cmd) {

	if(cmpcmd(cmd,AT_SELECT_CHILDREN)){
		return childList2response();
	}



	DBG_vPrintf(AT_CRD_TRACE, "\r\nAT:select command err\r\n");
	return 0;
}

PUBLIC int exec_request(uint8* data) {

	if (!AT_FREAM_JUDGE(data)) {
		DBG_vPrintf(AT_CRD_TRACE, "\r\nAT:not command frame\r\n");
		return 1;
	}

	switch (AT_FREAM_TYPE(data)) {

	case AT_SELECT_FRAME:
		DBG_vPrintf(AT_CRD_TRACE, "\r\nAT:select end device list\r\n");
//		childList2response();
		exec_select(AT_FREAM_CMD(data));
		break;
	case AT_TRANSE_DATA_FRAME:
		DBG_vPrintf(AT_CRD_TRACE, "\r\nAT:start trans data\r\n");
		transdata2child(data);
		break;
	case AT_HARDWARE_FRAME:
		DBG_vPrintf(AT_CRD_TRACE, "\r\nAT:hardware opt\r\n");
		break;
	default:
		DBG_vPrintf(AT_CRD_TRACE, "\r\nAT:frame type erro\r\n");
	}

	return 0;
}
