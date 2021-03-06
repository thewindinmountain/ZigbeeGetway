/*
 * epd_api.c
 *
 *  Created on: 2017-12-5
 *      Author: xubov
 */

#include "hw_config.h"
#include "jendefs.h"
#include "hw_gpio.h"
//#include "drv_lcd.h"
// #include "epd_spi_drv.h"
#include "epd_drv.h"
#include "dbg.h"
#include "image.h"
#include "epd_draw_drv.h"
#include "epd_spi_drv.h"

uint8 lcdPortInitStatus = 0;
uint8 table_sel = 0;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
const unsigned char Num[10][128] = {//32*32
{
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0x01,0xFF,0xFF,0xE0,0x00,0x3F,
0xFF,0xC0,0x00,0x1F,0xFF,0x83,0xFE,0x0F,0xFF,0x1F,0xFF,0xC7,0xFF,0x3F,0xFF,0xE7,
0xFF,0x7F,0xFF,0xF7,0xFF,0x7F,0xFF,0xF7,0xFF,0x3F,0xFF,0xE7,0xFF,0x1F,0xFF,0xC7,
0xFF,0x87,0xFF,0x0F,0xFF,0xC0,0x00,0x1F,0xFF,0xE0,0x00,0x3F,0xFF,0xFC,0x01,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
},
{// 1
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xDF,0xFF,0xF7,
0xFF,0xDF,0xFF,0xF7,0xFF,0xDF,0xFF,0xF7,0xFF,0xDF,0xFF,0xE7,0xFF,0x80,0x00,0x07,
0xFF,0x00,0x00,0x07,0xFF,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xE7,0xFF,0xFF,0xFF,0xF7,
0xFF,0xFF,0xFF,0xF7,0xFF,0xFF,0xFF,0xF7,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
},
{// 2
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE1,0xFF,0xC7,0xFF,0xC1,0xFF,0x87,
0xFF,0x99,0xFF,0x27,0xFF,0x3F,0xFE,0x67,0xFF,0x7F,0xFC,0xE7,0xFF,0x7F,0xF9,0xE7,
0xFF,0x7F,0xF3,0xE7,0xFF,0x7F,0xE7,0xE7,0xFF,0x3F,0xCF,0xE7,0xFF,0x1F,0x1F,0xE7,
0xFF,0x80,0x3F,0xC7,0xFF,0x80,0x7E,0x07,0xFF,0xE0,0xFE,0x1F,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
},
{//	3
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC3,0xFE,0x1F,0xFF,0x83,0xFE,0x0F,
0xFF,0x83,0xFE,0x0F,0xFF,0x3F,0xFF,0xE7,0xFF,0x7F,0xBF,0xF7,0xFF,0x7F,0xBF,0xF7,
0xFF,0x7F,0xBF,0xF7,0xFF,0x3F,0x1F,0xF7,0xFF,0x1E,0x1F,0xE7,0xFF,0x80,0x4F,0xCF,
0xFF,0x80,0xC0,0x0F,0xFF,0xE1,0xE0,0x1F,0xFF,0xFF,0xF8,0x7F,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
},
{//	4
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFD,0xFF,0xFF,0xFF,0xF1,0xFF,0xFF,0xFF,0xE1,0xFF,
0xFF,0xFF,0x8D,0xFF,0xFF,0xFE,0x1D,0xFF,0xFF,0xFC,0x7D,0xFB,0xFF,0xF0,0xFD,0xFB,
0xFF,0xE3,0xFD,0xFB,0xFF,0x80,0x00,0x03,0xFF,0x00,0x00,0x03,0xFF,0x00,0x00,0x03,
0xFF,0x00,0x00,0x03,0xFF,0xFF,0xFD,0xFB,0xFF,0xFF,0xFD,0xFB,0xFF,0xFF,0xFD,0xFB,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
},
{//	5
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0x1F,0xFF,0xF8,0x1E,0x0F,
0xFF,0x00,0x1E,0x67,0xFF,0x07,0x3F,0xF7,0xFF,0x3E,0x7F,0xF7,0xFF,0x3E,0xFF,0xF7,
0xFF,0x3E,0xFF,0xF7,0xFF,0x3E,0xFF,0xF7,0xFF,0x3E,0x7F,0xE7,0xFF,0x3E,0x3F,0x8F,
0xFF,0x3F,0x00,0x0F,0xFF,0x3F,0x80,0x1F,0xFF,0x3F,0xE0,0x7F,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
},
{//	6
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0x00,0xFF,0xFF,0xF0,0x00,0x3F,
0xFF,0xE0,0x00,0x1F,0xFF,0xC3,0x8F,0x0F,0xFF,0x9F,0x9F,0xC7,0xFF,0x3F,0x3F,0xE7,
0xFF,0x3F,0x7F,0xF7,0xFF,0x7F,0x7F,0xF7,0xFF,0x7F,0x7F,0xF7,0xFF,0x7F,0x3F,0xE7,
0xFF,0x0F,0x1F,0xCF,0xFF,0x8F,0x80,0x0F,0xFF,0xCF,0xC0,0x1F,0xFF,0xFF,0xE0,0x7F,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
},
{//	7
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC3,0xFF,0xFF,0xFF,0x03,0xFF,0xFF,
0xFF,0x0F,0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x3F,0xFE,0x07,0xFF,0x3F,0xF0,0x07,
0xFF,0x3F,0x80,0x07,0xFF,0x3E,0x0F,0xFF,0xFF,0x38,0x7F,0xFF,0xFF,0x21,0xFF,0xFF,
0xFF,0x07,0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
},
{//	8
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,0x3F,0xFF,0xE1,0xF0,0x1F,0xFF,0xC0,0xE0,0x0F,
0xFF,0x80,0x47,0xCF,0xFF,0x1C,0x0F,0xE7,0xFF,0x3E,0x1F,0xF7,0xFF,0x7F,0x1F,0xF7,
0xFF,0x7F,0x9F,0xF7,0xFF,0x7F,0x8F,0xF7,0xFF,0x3F,0x87,0xE7,0xFF,0x1E,0x03,0xCF,
0xFF,0x80,0x60,0x0F,0xFF,0xC0,0xF0,0x1F,0xFF,0xE1,0xF8,0x3F,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
},
{//	9
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x3F,0xFF,0xFF,0xC0,0x1F,0x8F,0xFF,0x80,0x0F,0x8F,
0xFF,0x8F,0xC7,0x87,0xFF,0x3F,0xE7,0xF7,0xFF,0x7F,0xF7,0xF7,0xFF,0x7F,0xF7,0xF7,
0xFF,0x7F,0xF7,0xE7,0xFF,0x7F,0xE7,0xC7,0xFF,0x3F,0xCF,0x8F,0xFF,0x8F,0x8C,0x1F,
0xFF,0xC0,0x00,0x3F,0xFF,0xE0,0x00,0x7F,0xFF,0xF8,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
},
};

void init_lcd_io(void)
{
	initGpio(LCD_BUSY, (LCD_SPI_MOSI|LCD_SPI_CLK|LCD_SPI_CS|LCD_DC|LCD_RST|LCD_BS1));
}

void app_lcd_port_init(void)
{
	if(lcdPortInitStatus != 1)
	{
		init_lcd_io();
		lcdPortInitStatus = 1;
	}
}

void epd_init(uint16_t xDot, uint16_t yDot)
{
	app_lcd_port_init();

	EPD_set_dot_range(xDot, yDot);
	EPD_init_Full();
}

void epd_test(void)
{
	postion start,end;
	epd_init(0,0);

	start.x = 0;
	start.y = 0;

	end.x = 128;
	end.y = 296;

	DBG_vPrintf(TRUE, "disp logo.\r\n");
	epd_draw_image(&start, &end, (char *)gImage_image, 0);
	driver_delay_xms(2000);

	DBG_vPrintf(TRUE, "disp all white.\r\n");
	epd_draw_line(&start, &end, 0);
	epd_draw_update();
	driver_delay_xms(2000);

	DBG_vPrintf(TRUE, "disp a line1.\r\n");
	start.x = 0;
	start.y = 0;
	end.x = 7;
	end.y = 7;
	epd_draw_line(&start, &end, 1);
	epd_draw_update();
	driver_delay_xms(2000);

	DBG_vPrintf(TRUE, "disp a line2.\r\n");
	start.x = 16;
	start.y = 16;
	end.x = 35;
	end.y = 35;
	epd_draw_line(&start, &end, 1);
	epd_draw_update();
	driver_delay_xms(2000);

	DBG_vPrintf(TRUE, "disp a line3.\r\n");
	start.x = 10;
	start.y = 20;
	end.x = 110;
	end.y = 40;
	epd_draw_line(&start, &end, 1);
	epd_draw_update();
	driver_delay_xms(2000);

	DBG_vPrintf(TRUE, "disp a line4.\r\n");
	start.x = 50;
	start.y = 60;
	end.x = 80;
	end.y = 70;
	epd_draw_line(&start, &end, 1);
	epd_draw_update();
	driver_delay_xms(2000);
//	while(1);
}

// end of file
