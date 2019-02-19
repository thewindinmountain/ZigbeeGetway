
#ifndef _DISPLAY_EPD_H_
#define _DISPLAY_EPD_H_

extern void driver_delay_xms(unsigned long xms);

void EPD_Dis_Part(unsigned char xStart,unsigned char xEnd,unsigned long yStart,unsigned long yEnd,unsigned char *DisBuffer,unsigned char Label);
void EPD_Dis_Full(unsigned char *DisBuffer,unsigned char Label);
void EPD_init_Full(void);
void EPD_init_Part(void);
void EPD_Init(void);
void EPD_set_dot_range(uint16_t xDot, uint16_t yDot);
void EPD_POWEROFF(void);

#endif
/***********************************************************
						end file
***********************************************************/


