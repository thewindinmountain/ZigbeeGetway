Version V0.0.1
date: 2018-1-22

* data: 2018-10-10  
api_disp  
由于字库与标签本身分离，基本上都是以图片的形式进行更新
app_table
* 创建界面步骤：1. 背景色，写白；2. block内容更新；3. 屏幕刷新


epd_api
* 提供画线，图片覆盖功能
* init_lcd_io
* app_lcd_port_init //调用init_lcd_io
* epd_init //调用app_lcd_port_init
* epd_test

epd_block
* 定义显示的最小单元，将屏幕划分为多个分区，暂时定义最大20个
* 分区的属性为：起始坐标(左上角像素坐标)，长，宽，是否反色，对齐方式，边框粗细，边框位置，内容


epd_draw_drv
* epd_draw_range //更新局部内容，并调用EPD_Dis_Full
* epd_draw_line //计算起始地址，然后调用epd_draw_range
* epd_draw_image // 调用epd_draw_range
* epd_draw_circle // 计算正方形区域，读区域内容，修改圆形部分，调用epd_draw_range
* epd_draw_char //获取字符表，再调用epd_draw_image 

epd_drv  
* ReadBusy //等待控制器空闲
* EPD_Write // 数据写，参数第一个数为写命令
* EPD_WriteCMD // 命令写，无参数
* EPD_WriteCMD_p1 // 命令写，1个参数/数据
* EPD_WriteCMD_p2 // 命令写，2个参数/数据
* EPD_WriteDispRam //写显示器RAM区
* EPD_WriteDispRamMono //用1个值刷新显示器RAM区
* EPD_POWERON
* EPD_POWEROFF
* part_display //设置刷新区域，调用SetRamArea和SetRamPointer
* EPD_SetRamArea //设置写入RAM区域范围
* EPD_SetRamPointer //设置写入RAM起始坐标
* EPD_DispInit
* EPD_Init
* EPD_Update //将RAM内的数据更新到显示器
* EPD_Update_Part //将RAM内所设置区域的数据更新到显示器
* EPD_WirteLUT
* EPD_init_Full //初始化为全局更新方式
* EPD_init_Part //初始化为局部更新方式
* EPD_Dis_Full   
  //写整个屏幕，调用SetRamPointer + WriteDispRam/Mono + Update
* EPD_Dis_Part  
  //写局部屏幕，调用part_display + WriteDispRam/Mono + Update

epd_spi_drv  
* SPI_Write  

hw_gpio_drv
* setGpio
* getGpio
* u32GetGpio
