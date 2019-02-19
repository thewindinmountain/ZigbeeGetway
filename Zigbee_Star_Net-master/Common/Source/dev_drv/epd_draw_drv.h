#ifndef __EPD_DRAW_DRV_H__
#define __EPD_DRAW_DRV_H__

#include "stdint.h"

typedef struct
{
    uint16_t x;
    uint16_t y;
}postion;

void epd_draw_range(postion *start, postion *end, char *newData, char inverse);
void epd_draw_line(postion *start, postion *end, char inverse);
void epd_draw_image(postion *start, postion *end, char *data, char inverse);
void epd_draw_update(void);


#endif // end of __EPD_DRAW_DRV_H__
