#ifndef __EPD_BLOCK_H__
#define __EPD_BLOCK_H__

#include "stdint.h"
#include "epd_draw_drv.h"

typedef struct
{
    postion start;
    uint16_t length;
    uint16_t width;
    char outline;
    char outlineStyle; // bit0 left；bit1 right；bit2 up，bit3 down
    char align;
    char *data;
    char inverse;
    char active;
}cell;

#endif //__EPD_BLOCK_H__
