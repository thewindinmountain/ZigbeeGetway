#include "epd_spi_drv.h"
#include "hw_gpio.h"
#include "hw_config.h"
#include "dbg.h"

#define TRACE_DISPLAY TRUE

void SPI_Write(unsigned char value)                                    
{                                                           
    unsigned char i;

    for(i=0; i<8; i++)   
    {
        EPD_CLK_0;
        if(value & 0x80)
        	EPD_MOSI_1;
        else
        	EPD_MOSI_0;		
        value = (value << 1); 
        EPD_CLK_1;  
    }
}

void driver_delay_xms(unsigned long xms)	
{	
    unsigned long i = 0, j = 0;

	for(j = 0; j < xms; j++)
	{
		for(i = 0; i<2800; i++) asm volatile("");
	}
}

void driver_delay_us(unsigned int xus)
{
	for(;xus>4;xus--);
}

void lcdIoTest(void)
{
 EPD_MOSI_0;
 EPD_CLK_0;
 EPD_CS_0;
 EPD_DC_0;
 EPD_RST_0;
 driver_delay_xms(100);

 EPD_MOSI_1;
 EPD_CLK_1;
 EPD_CS_1;
 EPD_DC_1;
 EPD_RST_1;
 driver_delay_xms(100);
}
/***********************************************************
						end file
***********************************************************/

