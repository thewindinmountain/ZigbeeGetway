
#include "stdint.h"
#include "stddef.h"
#include "epd_draw_drv.h"
#include "epd_drv.h"
#include "dbg.h"

char dispRam[4736+100] = {0};
extern uint16_t xDotMax;
extern uint16_t yDotMax;

/*
 * inverse: 1- inverse color; 0- normal
 * 
 */
void epd_draw_range(postion *start, postion *end, char *newData, char inverse)
{
    uint16_t x,y;
    uint16_t bitDispRamAddr;
    uint16_t bitNewDataAddr;

    // check data, start and end
    if(newData == NULL || start == NULL || end == NULL)
        return;
    if(start->x > xDotMax || start->y > yDotMax || end->x > xDotMax || end->y > yDotMax || start->x > end->x || start->y > end->y)
        return;

    // move new data to dispRam
    bitNewDataAddr = 0;

    if(inverse == 0)
    {
        for(y = start->y; y < end->y; y++)
        {
    		x = start->x;
    		bitDispRamAddr = y*xDotMax + x;
            for(; x < end->x; x++)
            {
                if(newData[bitNewDataAddr/8] & 1<<(bitNewDataAddr&0x07))
                {
                    // set 1
                    dispRam[bitDispRamAddr/8] |= 1<<(bitDispRamAddr&0x07);
                }else
                {
                    // set 0
                    dispRam[bitDispRamAddr/8] &= ~(1<<(bitDispRamAddr&0x07));
                }

                bitNewDataAddr++;
                bitDispRamAddr++;
            }
        }
    }else
    {
        for(y = start->y; y < end->y; y++)
        {
    		x = start->x;
    		bitDispRamAddr = y*xDotMax + x;
            for(; x < end->x; x++)
            {
                if(newData[bitNewDataAddr/8] & 1<<(bitNewDataAddr&0x07))
                {
                    // set 0
                    dispRam[bitDispRamAddr/8] &= ~(1<<(bitDispRamAddr&0x07));
                }else
                {
                    // set 1
                    dispRam[bitDispRamAddr/8] |= 1<<(bitDispRamAddr&0x07);
                }

                bitNewDataAddr++;
                bitDispRamAddr++;
            }
        }       
    }
}

/*
 * inverse: 1- inverse color; 0- normal
 */
void epd_draw_line(postion *start, postion *end, char inverse)
{
    uint16_t x,y;
    uint16_t bitDispRamAddr;
    char offset;

    // check start and end
    if(start == NULL || end == NULL)
        return;

    if(start->x > xDotMax || start->y > yDotMax || end->x > xDotMax || end->y > yDotMax || start->x > end->x || start->y > end->y)
        return;

    // move new data to dispRam
    if(inverse == 0)
    {
    	for(y = start->y; y < end->y; y++)
        {
    		x = start->x;
    		bitDispRamAddr = y*xDotMax + x;
            for(; x <= end->x; x++)
            {
                // set 1
            	offset = 7-(bitDispRamAddr&0x07);
                dispRam[bitDispRamAddr/8] |= 1<<offset;
//                dispRam[bitDispRamAddr/8] |= 1<<(7-(bitDispRamAddr%8));
                bitDispRamAddr ++;
            }
        }
    }else
    {
    	for(y = start->y; y < end->y; y++)
        {
    		x = start->x;
    		bitDispRamAddr = y*xDotMax + x;
            for(; x <= end->x; x++)
            {
                // set 0
            	offset = 7-(bitDispRamAddr&0x07);
                dispRam[bitDispRamAddr/8] &= ~(1<<offset);
                bitDispRamAddr ++;
            }
        }
    }
}

void epd_draw_image(postion *start, postion *end, char *data, char inverse)
{
    epd_draw_range(start, end, data, inverse);
}

void epd_draw_circle(postion *centre, uint16_t radius)
{

}

void epd_draw_update(void)
{
    EPD_Dis_Full((unsigned char*)dispRam,1);
}
// void epd_draw_char(postion *start, postion *end, char *data, char inverse)
// {

// }








