
#ifndef _DISPLAY_EPD_AUX_H_
#define _DISPLAY_EPD_AUX_H_
//#include "stm32f10x.h"
#include "jendefs.h"
#include "hw_gpio.h"
#include "hw_config.h"

#ifdef JN516X_EMSYM_ESL
#define EPD_CLK_0	setDO(0, LCD_SPI_CLK)
#define EPD_CLK_1	setDO(LCD_SPI_CLK, 0)
#else
#define EPD_CLK_0	setGpio(0, LCD_SPI_CLK)
#define EPD_CLK_1	setGpio(LCD_SPI_CLK, 0)
#endif

#define EPD_MOSI_0	setGpio(0, LCD_SPI_MOSI)
#define EPD_MOSI_1	setGpio(LCD_SPI_MOSI, 0)

#define EPD_CS_0	setGpio(0, LCD_SPI_CS)
#define EPD_CS_1	setGpio(LCD_SPI_CS, 0)

#define EPD_DC_0	setGpio(0, LCD_DC)
#define EPD_DC_1	setGpio(LCD_DC, 0)

#define EPD_RST_0	setGpio(0, LCD_RST)
#define EPD_RST_1	setGpio(LCD_RST, 0)

#define EPD_BS_0	setGpio(0, LCD_BS1)

#define EPD_BUSY_LEVEL 0
#define isEPD_BUSY  (u32GetGpio(LCD_BUSY)>0 ? 1 : 0)

void SPI_Write(unsigned char value);
void driver_delay_xms(unsigned long xms) __attribute__ ((noinline));
void driver_delay_us(unsigned int xus) __attribute__ ((noinline));
void lcdIoTest(void);
#endif
/***********************************************************
						end file
***********************************************************/


