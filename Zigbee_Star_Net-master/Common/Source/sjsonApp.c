/*
 * sjsonApp.c
 *
 *  Created on: 2018年3月18日
 *      Author: xubov
 */
#include <stddef.h> // for NULL
#include "sjson.h"
#include "dbg.h"
#include "node.h"
#include "string.h"

#define DPRINTF DBG_kprintf

//#define FILE_BUF_SIZE	1024
//static char g_buf[FILE_BUF_SIZE];

int sjson_msgHandle(char *head, char *end);

typedef struct
{
	int devID;
	char *actString;
}jsMsg;

typedef struct
{
	int devID;
	char *actString;
	int batVolt;
}batStatus;

uint16 sjson_PktVoltage(int vol, char *bufStart, char *bufEnd)
{
	char *end, *id, *act, *bat;//, *ret;
	batStatus testBatVol;

	end = bufEnd;

	// NODE_sData.u16PanId != testBatVol.devID
	testBatVol.devID = NODE_sData.u16PanId;
	testBatVol.actString = "update";
	testBatVol.batVolt = 3300;

	/* {"tagID":XXXXXXXX, "act":"update", "bat":3300} */
	id = SJSON_CreateRootObj(bufStart, end);
	act = SJSON_ObjAddNum(id, end, "tagID", testBatVol.devID );
	bat = SJSON_ObjAddString(act, end, "act", testBatVol.actString);
	SJSON_ObjAddNum(bat, end, "bat", testBatVol.batVolt);
	DPRINTF("%s\n", bufStart);

    return (uint16)strlen(bufStart);
}

int sjson_msgHandle(char *head, char *end)
{
	char *out;
	int type;
	double dataTmp;
	char cString[100];
	jsMsg msg;
	msg.actString = cString;

	if(head == NULL)
	{
		DPRINTF("Invalid data addr = NULL!\n");
		return -1;
	}

	// check JSON
	if (!SJSON_IsValid(head))
	{
		DPRINTF("Invalid JSON format!\n");
		return -1;
	}

	// check tagID
	out = SJSON_ToObjValue(head, "tagID", &type, &end);
	if (!out)
	{
		DPRINTF("parse error!\n");
		return -1;
	}
    if (type != SJSON_NUMBER)
    {
    	DPRINTF("parse error!\n");
    	return -1;
    }
	out = SJSON_ParseNum(out, &dataTmp);
	msg.devID = (int)dataTmp;


	// check act
	out = SJSON_ToObjValue(head, "act", &type, &end);
	if (!out)
	{
		DPRINTF("parse error!\n");
		return -1;
	}
    if (type != SJSON_STRING)
    {
		DPRINTF("parse error!\n");
		return -1;
    }
	out = SJSON_ParseString(out, msg.actString, 100);

	DPRINTF("msg.devID = %d.\n", msg.devID);
	DPRINTF("msgactString = %s.\n", msg.actString);

	if(NODE_sData.u16PanId == msg.devID)
	{
		//do it,
		// handle the msg here.
	}else
	{
		// do it,
		// forward to other node or hold it for children.
	}

	return 0;
}
