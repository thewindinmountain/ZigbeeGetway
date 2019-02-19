/*
 * at_command.h
 *
 *  Created on: 2019年1月24日
 *      Author: Administrator
 */

#ifndef AT_COMMAND_H_
#define AT_COMMAND_H_
#include <jendefs.h>
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

/**
 * 基类
 */
typedef struct {
	uint8 start_delimiter;
	uint8 len_l;
	uint8 len_h;
	uint8 type;
	uint8 id;
}at_command;

typedef struct {
//	uint8 start_delimiter;
//	uint16 len;
//	uint8 type;
//	uint8 id;
	at_command head;
	char cmd[2];
	uint8 checksum;
} select_req;

typedef struct {
	at_command head;
	uint64 dest_addr64;
	uint16 dest_addr16;
	uint8 max_hops; //需要传播的最多跳数，建议删除
	uint8 data[1024]; //需要传输的数据
	uint8 checksum;
} trans_data_req;

//回应数据
typedef struct  {
	uint8 start_delimiter;
	uint8 len_l;
	uint8 len_h;
	uint8 type;
	uint8 id;
	char cmd[2];
	uint8 status;
	uint8* data;
	uint8 checksum;
}response_data;

/*
 网络操作，例如重置网络，查询是否有新的节点入网
 */
typedef struct  {
//	uint8 start_delimiter;
//	uint16 len;
//	uint8 type;
//	uint8 status;
	at_command head;
	uint8 status;
	uint8 checksum;
}hdopt_req;

/*
 * 执行一条AT指令
 * */
int exec_request(uint8* data);

/**
 * 解析命令
 */


#endif /* AT_COMMAND_H_ */
