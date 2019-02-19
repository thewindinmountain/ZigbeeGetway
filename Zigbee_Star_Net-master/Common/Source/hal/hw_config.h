/*
 * hw_config.h
 *
 *  Created on: 2017骞�鏈�6鏃�
 *      Author: xubov
 */

#ifndef HW_CONFIG_H_
#define HW_CONFIG_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define USR_TIMERS_NUM   5

//#define JN516X_EMSYM_ESL
#define JN516X_EMSYM_ESL_5A
//#define JN516X_EMSYM_CB
//#define JN516X_EMSYM_CRD

#ifdef JN516X_EMSYM_CB
#define LED1_PIN	(0)
#define LED2_PIN	(1)
#define LED3_PIN	(4)
#define LED4_PIN	(5)
#endif
#ifdef JN516X_EMSYM_CRD
#define LED1_PIN	(2)
#define LED2_PIN	(3)
#endif

#ifdef JN516X_EMSYM_ESL_5A
#define LED1_PIN	(11)
#define LED2_PIN	(12)
#define LED3_PIN	(13)
#endif

#ifdef  LED1_PIN
#define LED1_MASK (1UL<<LED1_PIN)
#define LED1 LED1_MASK
#endif
#ifdef  LED2_PIN
#define LED2_MASK (1UL<<LED2_PIN)
#define LED2 LED2_MASK
#endif
#ifdef  LED3_PIN
#define LED3_MASK (1UL<<LED3_PIN)
#define LED3 LED3_MASK
#endif
#ifdef  LED4_PIN
#define LED4_MASK (1UL<<LED4_PIN)
#define LED4 LED4_MASK
#endif

#ifdef  LED4_PIN
#define LED_ALL_MASK (LED1_MASK | LED2_MASK | LED3_MASK | LED4_MASK)
#elif   LED3_PIN
#define LED_ALL_MASK (LED1_MASK | LED2_MASK | LED3_MASK)
#elif   LED2_PIN
#define LED_ALL_MASK (LED1_MASK | LED2_MASK)
#elif   LED1_PIN
#define LED_ALL_MASK (LED1_MASK)
#endif


#ifdef JN516X_EMSYM_CB
#define SPI_MOSI      (17)
#define SPI_CLK       (16) // 20 for DO0, 21 for DO1
#define SPI_CS        (15)
#define LCD_BUSY_IO   (12)  // LCD_BUSY
#define LCD_DC_IO     (14)  // LCD_DC
#define LCD_RST_IO    (13)  // LCD_RST
#define LCD_BS1_IO    (10)  // LCD_BS1
#else
// after 2018-1-1
#define SPI_MOSI      (1)
#define SPI_CLK       (2)  //
#define SPI_CS        (3)
#define LCD_DC_IO     (18) // LCD_DC
#define LCD_RST_IO    (19) // LCD_RST
#define LCD_BUSY_IO   (4)  // LCD_BUSY
#define LCD_BS1_IO    (5)  // LCD_BS1
#endif

#define LCD_SPI_MOSI (1UL<<SPI_MOSI)
#define LCD_SPI_CLK  (1UL<<SPI_CLK)
#define LCD_SPI_CS   (1UL<<SPI_CS)
#define LCD_DC       (1UL<<LCD_DC_IO)
#define LCD_RST      (1UL<<LCD_RST_IO)
#define LCD_BS1      (1UL<<LCD_BS1_IO)
#define LCD_BUSY     (1UL<<LCD_BUSY_IO)

#define RISING_EDGE  1
#define FALLING_EDGE 2
#define BOTH_EDGE    3

#define USR_UART_SEL_DIO15 FALSE //TRUE for DIO15, FALSE for DIO9.

// WIFI INTERFACE
#define WIFI_RST         (13)
#define WIFI_RST_MASK    (1<<WIFI_RST)

#define WIFI_PWR_EN      (16)
#define WIFI_PWR_EN_MASK (1<<WIFI_PWR_EN)

#define WIFI_WAKE        (17)
#define WIFI_WAKE_MASK   (1<<WIFI_WAKE)

// #define KEY1                 (9)
// #define KEY1_INT_MASK        E_AHI_DIO9_INT
// #define KEY2                 (10)
// #define KEY2_INT_MASK        E_AHI_DIO10_INT
#define KEY1                 (14)
#define KEY1_INT_MASK        E_AHI_DIO14_INT
#define KEY2                 (15)
#define KEY2_INT_MASK        E_AHI_DIO15_INT
#define KEY_INT_ALL_MASK     (KEY1_INT_MASK | KEY2_INT_MASK)
#define APP_BUTTONS_DIO_MASK KEY_INT_ALL_MASK
#define APP_BUTTONS_NUM      2

#define KEY1_MASK (1UL<<KEY1)
#define KEY2_MASK (1UL<<KEY2)
#define KEY_ALL_MASK (KEY1_MASK | KEY2_MASK)

// PWM: Timer0 -> DIO[8-10]/DIO[2-4]
//      Timer1 -> DIO11/DIO5
//      Timer2 -> DIO12/DIO6
//      Timer3 -> DIO13/DIO7
//      Timer4 -> DIO17/DIO8
#define PWM_MASK_TIMER0  (1UL<<2)
#define PWM_MASK_TIMER1  (1UL<<3)
#define PWM_MASK_TIMER2  (1UL<<4)
#define PWM_MASK_TIMER3  (1UL<<5)
#define PWM_MASK_TIMER4  (1UL<<6)

#define ADC_MASK_VDD     (1UL<<0)
#define ADC_MASK_TEMP    (1ul<<1)
#define ADC_GAIN          2

//0-7
// app program from sector 0
// sector 6 for LCD Table;
// sector 5 for Net Table.
#define FLASH_START_ADDR  0x00080000
#define FLSAH_SECTOR_SIZE 0x8000

#define INTERNAL_FLASH_SECTOR_NET 5
#define FLASH_START_ADDR_NET (FLSAH_SECTOR_SIZE * INTERNAL_FLASH_SECTOR_NET)
#define USR_FLASH_START_ADDR FLASH_START_ADDR_NET

#define INTERNAL_FLASH_SECTOR_LCD 6
#define FLASH_START_ADDR_LCD (FLSAH_SECTOR_SIZE * INTERNAL_FLASH_SECTOR_LCD)

#define INTERNAL_FLASH_SECTOR_PARM 7
#define FLASH_START_ADDR_PARM (FLSAH_SECTOR_SIZE * INTERNAL_FLASH_SECTOR_PARM)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/


#endif /* HW_CONFIG_H_ */
