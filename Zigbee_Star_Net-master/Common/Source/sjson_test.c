/*
 * sjson_app.h
 *
 *  Created on: 2018楠烇拷閺堬拷閺冿拷
 *      Author: xubov
 */

#ifndef SJSON_APP_H_
#define SJSON_APP_H_

#include "jendefs.h"

char* get_poll_json_cmd(uint64 tagID);
char* get_poll_bak_with_nodata_json_cmd(uint64 tagID);
char* get_poll_bak_with_data_json_cmd(uint64 tagID);
char* get_miss_msg_json_cmd(uint64 tagID,uint16 msgNum);
char* get_msg_done_json_cmd(uint64 tagID,uint16 msgNum);
int getStrFromSjsonMsg(char *strData, char *data, char *ret);
int getNumFromSjsonMsg(char *strData, char *data, double *ret);


#endif /* SJSON_APP_H_ */
 * 			"array":[{"param1":1080.000230,"param2":768.555550},{"param1":234210.235225,"param2":-2453443.289232,"param3":"p3_string"}]
 * 		},
 * 	"esc\"a\"ped":"\tstring\t"}
 * */
int sjson_test(void)
{
    char *end, *out, *call, *par;
    char *aa, *ret, *ptr;
    int type;
    double number = 0.0;
    char name[64];

    // JSON string creation test
    DPRINTF("\nJSON string creation test:\n");
    end = g_buf + FILE_BUF_SIZE;
    ret = SJSON_CreateRootObj(g_buf, end);

    call = SJSON_ObjAddObj(g_buf, end, "call");
    ret = SJSON_ObjAddString(call, end, "function", "test2");

	ret = SJSON_ObjAddString(g_buf, end, "esc\"a\"ped", "\tstring\t");
    par = SJSON_ObjAddObj(call, end, "params");
    ret = SJSON_ObjAddNum(par, end, "param1", 1080);
    ret = SJSON_ObjAddNum(par, end, "param2", 1080);

    aa = SJSON_ObjAddArray(call, end, "array");
    par = SJSON_ArrayAddObj(aa, end);
    ret = SJSON_ObjAddNum(par, end, "param1", 1080.00023);
    ret = SJSON_ObjAddNum(par, end, "param2", 768.55555);
    par = SJSON_ArrayAddObj(aa, end);
    ret = SJSON_ObjAddNum(par, end, "param1", 234210.235225673567);
    ret = SJSON_ObjAddNum(par, end, "param2", -2453443.28923234435);
    ret = SJSON_ObjAddString(par, end, "param3", "p3_string");

    DPRINTF("%s\n", g_buf);

    // JSON string parse test
    DPRINTF("\nJSON string parse test:\n");
    if (!SJSON_IsValid(g_buf))
    {
        DPRINTF("Invalid JSON format!\n");
        return -1;
    }

    out = SJSON_ToObjValue(g_buf, "call", NULL, NULL);
    if (!out)
    {
        DPRINTF("parse error!\n");
        return -1;
    }
    out = SJSON_ToObjValue(out, "array", NULL, &end);
    if (!out)
    {
        DPRINTF("parse error!\n");
        return -1;
    }
    ptr = out;
    while (ptr < end)
    {
        DPRINTF("%c", *ptr++);
    }
    DPRINTF("\n");

    par = SJSON_ToArrayValue(out, 1, &type, &end);
    if (!par)
    {
        DPRINTF("parse error!\n");
        return -1;
    }
    out = SJSON_ToObjValue(par, "param2", &type, &end);
    if (!out)
    {
        DPRINTF("parse error!\n");
        return -1;
    }
    if (type == SJSON_NUMBER)
    {
        out = SJSON_ParseNum(out, &number);
//        DPRINTF("%lf\n", number);// not suport.
        if(number < 0.0)
        {
        	DPRINTF("-");
        	number = -number;
        }
        DPRINTF("%d.", (int)number);
        DPRINTF("%d\n", (int)((number - (int)number)*1000000));
    }

    out = SJSON_ToObjValue(par, "param3", &type, &end);
    if (!out)
    {
        DPRINTF("parse error!\n");
        return -1;
    }
    if (type == SJSON_STRING)
    {
        out = SJSON_ParseString(out, name, 64);
        DPRINTF("%s\n", name);
    }

    out = SJSON_ToPathValue(g_buf, "*/*/param1", &type, &end);
    out = SJSON_ToPathValue(g_buf, "*/*[0]/param1", &type, &end);
    out = SJSON_ToPathValue(g_buf, "*/array[0]/param2", &type, &end);
    out = SJSON_ToPathValue(g_buf, "*/*/param1", &type, &end);
    if (!out)
    {
        DPRINTF("parse error!\n");
        return -1;
    }

    out = SJSON_FindObjValue(g_buf, "array", &type, &end);
    out = SJSON_FindObjValue(g_buf, "param1", &type, &end);
	out = SJSON_FindObjValue(g_buf, "esc\"a\"ped", &type, &end);
    if (!out)
    {
        DPRINTF("parse error!\n");
        return -1;
    }

    DPRINTF("\n%s\n", g_buf);

    return 0;
}

//int main(void)
//{
//    sjson_test();
//    return 0;
//}
