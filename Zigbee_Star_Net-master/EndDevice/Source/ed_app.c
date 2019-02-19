/*
 * ed_app.c
 *
 *  Created on: 2019年1月29日
 *      Author: Administrator
 */

#include "ed_app.h"
#include "dbg.h"
#include "node.h"
#include "msg.h"
/**
 * 处理终端接收到的数据帧
 *
 * 1，判断接收到的数据的数据类型
 * 2，根据类型对硬件进行相关的操作
 * 3，返回一个应答帧
 *
 */
uint8 ed_buf[32];

int halopt(uint8* data,uint16 len){
	return 0;
}

/**
 * 构建响应帧
 */
int buildresp(uint64 destaddr)
{
	ed_buf[0]=3;
	memcpy(ed_buf+1,"DTOK",4);
	NODE_vTx(destaddr, 5, ed_buf, ACK_PKT);
}


PUBLIC int data2label(uint64 srcaddr,uint8* data,uint16 len)
{
	int i,optres;
	//打印内容
	DBG_kprintf("\r\n incoming data: ");
	for(i=0;i<len;i++){
		DBG_kprintf("%x ",data[i]);
	}
	DBG_kprintf("\r\n");
	DBG_kprintf("content %s\r\n",data);

	//TODO 解析数据，同时对硬件进行操作，这个是后话 ，这里的操作有
	optres =halopt(data,len);
	if(!optres){
		//硬件操作成功
		buildresp(srcaddr);
	}
	//组织应答帧
	return 0;
}
