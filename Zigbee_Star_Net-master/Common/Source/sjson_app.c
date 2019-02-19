/*
 * sjson_app.c
 *
 *  Created on: 2018楠烇拷閺堬拷閺冿拷
 *      Author: xubov
 */


#include <stddef.h> // for NULL
#include "sjson.h"
#include "sjson_app.h"
#include "dbg.h"
#include "jendefs.h"
#include "string.h"

#define DPRINTF DBG_kprintf

#define FILE_BUF_SIZE	1024
static char g_buf[FILE_BUF_SIZE];

#define TMP_BUF_LEN 100
char cmd[TMP_BUF_LEN];
PUBLIC char* get_poll_json_cmd(uint64 tagID)
{
	char *start, *end;//, *ret;
	start = cmd;
	end = cmd + TMP_BUF_LEN;
	memset(start, 0, TMP_BUF_LEN);
	SJSON_CreateRootObj(start, end);
	SJSON_ObjAddNum(start, end, "tagID", tagID);
	SJSON_ObjAddString(start, end, "act", "poll");
	SJSON_ObjAddString(start, end, "parm", "req");
	return start;
}

PUBLIC char* get_poll_bak_with_nodata_json_cmd(uint64 tagID)
{
	char *start, *end;//, *ret;
	start = cmd;
	end = cmd + TMP_BUF_LEN;
	memset(start, 0, TMP_BUF_LEN);
	start = SJSON_CreateRootObj(start, end);
	SJSON_ObjAddNum(start, end, "tagID", tagID);
	SJSON_ObjAddString(start, end, "act", "poll");
	 SJSON_ObjAddString(start, end, "parm", "none");
	return start;
}

PUBLIC char* get_poll_bak_with_data_json_cmd(uint64 tagID)
{
	char *start, *end;//, *ret;
	start = cmd;
	end = cmd + TMP_BUF_LEN;
	memset(start, 0, TMP_BUF_LEN);
	start = SJSON_CreateRootObj(start, end);
	SJSON_ObjAddNum(start, end, "tagID", tagID);
	SJSON_ObjAddString(start, end, "act", "poll");
	 SJSON_ObjAddString(start, end, "parm", "yes");
	return start;
}

PUBLIC char* get_miss_msg_json_cmd(uint64 tagID,uint16 msgNum)
{
	char *start, *end;//, *ret;
	start = cmd;
	end = cmd + TMP_BUF_LEN;
	memset(start, 0, TMP_BUF_LEN);
	start = SJSON_CreateRootObj(start, end);
	SJSON_ObjAddNum(start, end, "tagID", tagID);
	SJSON_ObjAddString(start, end, "act", "miss");
//	SJSON_ObjAddString(start, end, "parm", "none");
	SJSON_ObjAddNum(start, end, "parm", msgNum);
	return start;
}

PUBLIC char* get_msg_done_json_cmd(uint64 tagID,uint16 msgNum)
{
	char *start, *end;//, *ret;
	start = cmd;
	end = cmd + TMP_BUF_LEN;
	memset(start, 0, TMP_BUF_LEN);
	start = SJSON_CreateRootObj(start, end);
	SJSON_ObjAddNum(start, end, "tagID", tagID);
	SJSON_ObjAddString(start, end, "act", "done");
//	SJSON_ObjAddString(start, end, "parm", "none");
	SJSON_ObjAddNum(start, end, "parm", msgNum);
	return start;
}


void structToSjson()
{

}

void sjsonToStruct()
{

}

int getNumFromSjsonMsg(char *strData, char *data, double *ret)
{
	char *end, *out;
	int type;

	if (!SJSON_IsValid(g_buf))
	{
		return -1;
	}
	out = SJSON_ToObjValue(data, strData, &type, &end);

	if (type != SJSON_NUMBER)
	{
		return -1;
	}

	out = SJSON_ParseNum(out, ret);

	return 0;
}

int getStrFromSjsonMsg(char *strData, char *data, char *ret)
{
	char *end, *out;
	int type;

	if(data == NULL || strData == NULL || ret == NULL)
	{
		return -1;
	}

	if (!SJSON_IsValid(data))
	{
		return -2;
	}
	out = SJSON_ToObjValue(data, strData, &type, &end);

	out = SJSON_ParseString(out, ret, 100);

	return 0;
}
