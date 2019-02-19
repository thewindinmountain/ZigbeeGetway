/*
 * dbg_config.h
 *
 *  Created on: 2017Äê12ÔÂ7ÈÕ
 *      Author: xubov
 */

void delay_xms(unsigned long xms)
{
    unsigned long i = 0, j = 0;

	for(j = 0; j < xms; j++)
	{
		for(i = 0; i<2800; i++) asm volatile("");
	}
}

void delay_us(unsigned int xus)

{
	for(;xus>4;xus--) asm volatile("");
}
