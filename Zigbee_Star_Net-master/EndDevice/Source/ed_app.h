/*
 * ed_app.h
 *
 *  Created on: 2019年1月29日
 *      Author: Administrator
 */

#ifndef ED_APP_H_
#define ED_APP_H_

#include <jendefs.h>

typedef struct{
	uint16 price;
	char name[128];
}label;


PUBLIC int data2label(uint64 srcaddr ,uint8* data,uint16 len);

#endif /* ED_APP_H_ */
