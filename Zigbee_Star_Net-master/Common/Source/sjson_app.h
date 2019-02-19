/*
 * sjson_app.h
 *
 *  Created on: 2018骞�鏈�鏃�
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
