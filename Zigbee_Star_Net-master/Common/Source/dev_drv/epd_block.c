/*
 * file name: epd_block.c
 * author: xubh
 * data: 2018-10-12
 */

#include "epd_block.h"
#include "epd_draw_drv.h"
#include "hw_config.h"
#include "drv_flash.h"

#define MAX_CELL 20
char cellBuf[5000];
cell table_cell[MAX_CELL];

void table_create();
void table_modify(char cellNum);
void table_cell_read(uint32_t table_num);
void table_cell_write(uint32_t table_num);

void table_cell_read(uint32_t table_num)
{
    uint32_t offset;
    uint8  status;

    offset = sizeof(table_cell) * table_num;
    // read table style from flash.
    drvFlash.read(FLASH_START_ADDR_LCD + offset, (uint8*)table_cell, sizeof(table_cell), &status);
}

void table_cell_write(uint32_t addr)
{
    // write table style to flash.
}

void table_cell_update(cell *cell)
{
    postion end;

    end.x = cell->start.x + cell->width - 1;
    end.y = cell->start.y + cell->length - 1;
    
    // draw outline

    // draw image
    // get cell data
    epd_draw_range(&(cell->start), &end, cell->data, cell->inverse);
}

void table_default_create(void)
{
    postion start;
    postion end;
    cell *curCell = table_cell;
    char i;
    // table_cell_read(0x00);

    for(i=0; i< MAX_CELL; i++)
    {
        if(curCell->active == 1)
        {
            table_cell_update(curCell);
        }
        curCell += 1;
    }

    // disp update, write data to epd
    epd_draw_update();
}

void table_create(void)
{
    postion start;
    postion end;
    cell *curCell = table_cell;
    char i;
    table_cell_read(0x00);

    for(i=0; i< MAX_CELL; i++)
    {
        if(curCell->active == 1)
        {
            table_cell_update(curCell);
        }
        curCell += 1;
    }

    // disp update, write data to epd
    epd_draw_update();
}

void table_modify(char cellNum)
{

}


// end of file
