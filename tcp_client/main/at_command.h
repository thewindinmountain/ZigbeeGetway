#ifndef	AT_COMMAND_H
#define AT_COMMAND_H
#include "esp_system.h"
//查询帧
#define AT_SELECT_FRAME 0x09
//数据传送帧
#define AT_TRANSE_DATA_FRAME 0x10
//响应帧
#define AT_RESPONSE_FRAME 0x88
//硬件操作命令帧
#define AT_HARDWARE_FRAME 0x8A
//相应成功
#define AT_RESP_OK 0x00

#define AT_SELECT_CHILDREN "CL"
/**
 * common operation
 */
#define AT_FREAM_JUDGE(data) (*data==0x7E)
#define AT_FREAM_TYPE(data) (data[3])
#define AT_FREAM_LEN(data,len) \
	len=data[1];\
len=(len<<8)|data[2];
#define AT_FREAM_ID(data) (data[4])
#define AT_FREAM_CMD(data) (&data[5])

//TODO 计算checksum
#define AT_FREAM_CHECKSUM(data) (data[4])

/**
 * select data
 */


/**
 * Trans data
 */
#define AT_FREAM_ADDR64(data,addr,ind) \
	addr=data[5];\
for(ind=6;ind<=12;ind++)\
addr=(addr<<8)|data[ind];

#define AT_FREAM_ADDR16(data,addr) \
	addr=data[13];\
addr=(addr<<8)|data[14];

#define AT_FREAM_OPT(data) (data[16])
#define AT_FREAM_TRANS_DATA(data) (&data[17])
#define AT_FREAM_TRANS_DATA_LEN(len) \
	len=len-4;
/*
response frame
*/
#define AT_FREAM_RESP_DATA(data) (&data[8])
#define AT_FREAM_CHECHK_CMD(data,cmd) (data[5] == cmd[0]&& data[6] == cmd[1])

typedef void (*recieve_data_callback)(uint8_t *, unsigned short);

int send_cmd(uint8_t*,unsigned short);
int init_cmd_task(recieve_data_callback);

#endif
