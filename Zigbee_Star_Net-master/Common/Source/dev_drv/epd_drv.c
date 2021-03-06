#include "epd_spi_drv.h"
#include "epd_drv.h"
#include "epd_drv_Config.h"
#include "hw_gpio.h"
#include "hw_config.h"

uint16_t xDotMax = 0;
uint16_t yDotMax = 0;

const unsigned char WF_200MS[]={	//æ¿ çµåé¸æ?¼²å¨æ°£åçå?éºä½¸ç§¹é®æ»å´¶éºä¾æ§±éä½¹å?ç»æÃºå®¥å?ç¤é¿çæ·
	0x32,
	0x44,0xAA,0x55,0xAA,0x55,0xAA,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xAA,0xAA,0xAA,0x1A,0x00,0x00,0x00,0x00,0x00,0x00,
};
const unsigned char WF_300MS[]={	//éä½¹å¯§ç»ææ®é¡åîé³åæéæ´ªæé°îç¡¶é¼æï¹¥ç»»æ¶¢å¹é¡æ¶çé»åº¤îæ¿å©å´ºæ¾¶æ?§é«å¨æ´é¡¦âæ¢ºé¸åº£âæ¿æç§¹å®æ»æéï¿½
	0x32,
	0x44,0xAA,0x55,0xAA,0x55,0xAA,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,
};
const unsigned char WF_500MS[]={	//éä½¹å¯§ç»æå´ºæ¿ å¬îé³åæéæ´ªæé°îç¡¶é¼æï¹¥ç»»æ¶¢å¹é¡æ¶çé»åº¤îæ¿å©å´ºæ¾¶æ?§é«å¨æ´è¢éä½¹å?ç»æÃºå®¥å?ç¤é¿çæ·
	0x32,
	0x44,0x44,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x11,0x11,0x00,0x00,
	0x00,0x00,0x00,0x00,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0x01,0x00,0x00,
};
const unsigned char WF_700MS[]={	//æ¿ çµåé¸æ?¼²å¨æ°£åçå?é¸âîµæ¿¡æ°ç®éç¸å·éä½¹å?ç»æÃºå®¥å?ç¤é¿çæ·
	0x32,
	0x44,0x44,0x44,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0xAA,0x55,0x55,0x55,0xAA,0xAA,0xAA,
	0x11,0x11,0x11,0x00,0xFF,0xF5,0xFF,0xFF,0x5F,0xFF,0xF5,0x5F,0xFF,0x15,
};

const unsigned char LUTDefault_full[]={
0x32,	// command

0x22,
0x55,
0xAA,
0x55,
0xAA,
0x55,
0xAA,
0x11,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x1E,
0x1E,
0x1E,
0x1E,
0x1E,
0x1E,
0x1E,
0x1E,
0x01,
0x00,
0x00,
0x00,
0x00,
};

const unsigned char LUTDefault_part[]={
0x32,	// command

0x18,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x0F,
0x01,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
};

static void EPD_WirteLUT(unsigned char *LUTvalue,unsigned char Size);

static unsigned char ReadBusy(void)
{
  unsigned long i=0;

  
  for(i=0;i<400;i++)
  {
      if(isEPD_BUSY==EPD_BUSY_LEVEL) {
      return 1;
      }
	  driver_delay_xms(10);
  }

  return 0;
}

static void EPD_WriteCMD(unsigned char command)
{
    EPD_CS_0;                   
	EPD_DC_0;		// command write
	SPI_Write(command);
	EPD_CS_1;
}

//static void EPD_WriteDATA(unsigned char command)
//{
//    EPD_CS_0;                   
//	EPD_DC_1;		// command write
//	SPI_Write(command);
//	EPD_CS_1;
//}
	
static void EPD_WriteCMD_p1(unsigned char command,unsigned char para)
{
	//while(isEPD_BUSY == 1);	// wait
	ReadBusy();	

    EPD_CS_0;                   
	EPD_DC_0;		// command write
	SPI_Write(command);
	EPD_DC_1;		// command write
	SPI_Write(para);
	EPD_CS_1;
}

static void EPD_WriteCMD_p2(unsigned char command,unsigned char para1,unsigned char para2)
{
	while(isEPD_BUSY == 1);	// wait
	ReadBusy();	

    EPD_CS_0;                   
	EPD_DC_0;		// command write
	SPI_Write(command);
	EPD_DC_1;		// command write
	SPI_Write(para1);
	SPI_Write(para2);
	EPD_CS_1;
} 

/*********************************************
éåç¤é²â³îæ¿ æ°¬çªéºå¥å¹ç»åæ¨å¦è?äº¾éè½°ç?ç»æ®ï½å©µå¶æ½éºåµ®å?æ¿ çµåé¨å å¹é¡ï¹ç®?»æ£å§¤éåç¹æ¿ åå³é¸å?æ¸é½å¤âå¦?¤æ?©µçµî½å¦²å´å´£éã§æ¦æ¿¡îãéåº¨ç?¥ã¦å°­æ¿??ç¡å?åÐ§é¥Ñîé»åº¢å¹éçåµéå½æ£¿ç»ä½¸îå©µå?çéæ¶Ã?»æ°?é¡åç²éºå±¾ç¨ç»»æ¿å´é¹æ§äº¾éï½åé¡«æç¹éæ¶±åç¼â?«ã¦çéæ¶â?å£ç¼åå¸ç»ç¸å´¹é§æ?ç®é¸æµåç»å¬?éºå¶æ¼é¿æå¹éæç¹é¡æ?ç®°é³Ñåç¼å©î£é¶è¥åé¸å¿åå¨æ ­æ¢ºçºã¥æ«é¹å¤æ?
éåç·å?è¤æ¹éçç¤é¹è¾¾ç®éã©æé«æ¿å·ç»¾å§åéå?¼´å¦¤â¬æµæ§âé½åçç¼â¬æ¸â¬é©ã¤å¹çè?§ç¼ä½¸îç»¨éå¼¸è¹æ¶âæ¿è?²¯ç»¶æ¶¢æéãå?
*********************************************/
static void EPD_Write(unsigned char *value, unsigned char datalen)
{
	unsigned char i = 0;
	unsigned char *ptemp;
	
	ptemp = value;
	
    EPD_CS_0;
	EPD_DC_0;		// command write
	SPI_Write(*ptemp);
	ptemp++;
	EPD_DC_1;		// data write
	for(i= 0;i<datalen-1;i++)	// sub the command
	{
		SPI_Write(*ptemp);
		ptemp++;
	}
	EPD_CS_1;
}

/*********************************************
éåç¤é²â³îæ¿ æ°¬çªéºå¥å¹ç»åæ¨å¦è?äº¾éè½°ç?ç»æ®ï½å©µå¬µæé³Ñåå¯?¥å¹æµ£å?¸éåæ´¦éå?éå çé¸ã£åé½îffer
éåç¤é²ï¿ æ·éâç£¹ççå¸å§¹æ¥æ£éå?éºååéã©æ¤éæ¶î?	XSize  xéåç¤é¼î¦å´æ¿ æ°¬ç® é¹æî§ç» æ ¨ç¹æ¿ åå?éºå?ç¤éé¸å¿åç¼æç¹éå©å«?  128éåè¶ç»®îå´å§â¬éç´å¾é°îæéç»æ¢»éÑæ´éåçé¹ç¢±å¼éï½ç´é§îå¾é°îä»¯é»åº¯ç¶æµæ»å¼³éççé¸âå¾ç»è¤ç´ç£îæ¨éºä½¹æ»ç»æâ¬ï½åéî¢æ¢ºéå©å?é¸æ´è¾é»æ´ç¹å§î å¸æ¿è¾¾ç¶é¡??æ¬¢éã¦ç¥é»æå¹çï¿½
		YSize  yéåç¤é¼î¦å´æ¿ æ°¬ç® é¹æî§ç» æ ¨ç¹æ¿ åå?éºå?ç¤éé¸å¿åç¼æç¹éå©å«?
		Dispbuff éåç¤é¼î¤â¬æ°¼åéèµç¦ç£î?¸½æ¨å¦¯æç¡¾éºå?ç·éï½Îé§îæ§é¡æ¬çé¿æåé¿æ°æéï½å?ç¼ç³æéï½å¹é¸æ¿ç§¹éªå¿å´¨é¡Ñæéæå?æ»æé¡ã¡âéå ç¨éæææ?åº¡å´é¹î¾åå?æîéç²îéä½¹æ¤¿æµæ¬å´¡éå·?æ¤¤æ¶ééå¬?é¡ï½ç®éµå ?®å§åÐ§é¥Ñîé»åº¢å¹éé¹å¼«é¡â¬éªæ?æ é¼îæ§´æ¿ å¾·æ¿â¬æ»æéååæ¿ååéæ³âé¨î½çç¼ä½ºç²?æ¥å¹éãçµæ¤¤æåæ¤¤îå´æ¿¡å å´¶éºä½ºî¼æ¿¡ç?ç¯æ¥åæ´âæ§é»ã æ¡éææ¨éåå?
*********************************************/
static void EPD_WriteDispRam(unsigned char XSize,unsigned int YSize,
							unsigned char *Dispbuff)
{
	int i = 0,j = 0;

	if(XSize%8 != 0)
	{
		XSize = XSize+(8-XSize%8);
	}
	XSize = XSize/8;

	//while(isEPD_BUSY == 1);	//wait
	ReadBusy();		
	
    EPD_CS_0;                   
	EPD_DC_0;		//command write
	SPI_Write(0x24);
	
	EPD_DC_1;		//data write
	for(i=0;i<YSize;i++)
	{
		for(j=0;j<XSize;j++)
		{
			SPI_Write(*Dispbuff);
			Dispbuff++;
		}
	}
	
	EPD_CS_1;
}

/*********************************************
éåç¤é²â³îæ¿ æ°¬çªéºå¥å¹ç»åæ¨å¦è?äº¾éè½°ç?ç»å½å¼?¾ç»æ¸é¿çæ? éåç¤é²ï¿ å´éîå¡é¿æåé»ï½î¿çºè¹ç±é»îæ£ç»±ææéå«î¾ffer éåè¶éç¸å´é¾è?ç¤ç¹ã¥æéä»æ£éç²æé»æï¹ªéæ¶¢åéå§å·éåÉéã©å¥æ¾¶æ??é¸æ¨åé?å æ¢»æµ£ç?çº§ç¼î½æ¸é£ç½æ©çè¹?´¶éåæ´¦éå¥¸ç®å®¥å?âé¸æ¶ï¹¤æ¿? ¥æ£éæå¦ç¹æªç¶å®åâå¨å¿å£ç¼åç?é¡åâ?¬å§ç£¹éîç§µéç¼ææ¿å®åæ¤¤å¿£äºæ·å©îå?å¼Ð¨ç¼ä¾åéæ³ç·é?ï¹ç¤æ¤¤æ¶çµ¾å¨ä»æ¢ºçåæ£é¹è«æ¾ç»®è¯²î¼æ¾¶å?æ«¢éè·¨å¦é·ï¿½
éåç¤é²ï¿ æ·éâç£¹ççå¸å§¹æ¥æ£éå?éºååéã©æ¤éæ¶î?	XSize  xéåç¤é¼î¦å´æ¿ æ°¬ç® é¹æî§ç» æ ¨ç¹æ¿ åå?éºå?ç¤éé¸å¿åç¼æç¹éå©å«?  128éåè¶ç»®îå´å§â¬éç´å¾é°îæéç»æ¢»éÑæ´éåçé¹ç¢±å¼éï½ç´é§îå¾é°îä»¯é»åº¯ç¶æµæ»å¼³éççé¸âå¾ç»è¤ç´ç£îæ¨éºä½¹æ»ç»æâ¬ï½åéî¢æ¢ºéå©å?é¸æ´è¾é»æ´ç¹å§î å¸æ¿è¾¾ç¶é¡??æ¬¢éã¦ç¥é»æå¹çï¿½
		YSize  yéåç¤é¼î¦å´æ¿ æ°¬ç® é¹æî§ç» æ ¨ç¹æ¿ åå?éºå?ç¤éé¸å¿åç¼æç¹éå©å«?
		dispdata éåç¤é¼î¤â¬æ°¼åéèµç¦ç£î?¸½æ¨å¦¯æç¡¾éºå?ç·éï½Îé§îæ§é¡æ¬çé¿æå¹éå¿æé½åå¹éæ°¾æ·
*********************************************/
static void EPD_WriteDispRamMono(unsigned char XSize,unsigned int YSize,
							unsigned char dispdata)
{
	int i = 0,j = 0;

	if(XSize%8 != 0)
	{
		XSize = XSize+(8-XSize%8);
	}
	XSize = XSize/8;
	//while(isEPD_BUSY == 1);	// wait	
	ReadBusy();	

    EPD_CS_0;                   
	EPD_DC_0;		// command write
	SPI_Write(0x24);
	
	EPD_DC_1;		// data write
	for(i=0;i<YSize;i++)
	{
		for(j=0;j<XSize;j++)
		{
		 SPI_Write(dispdata);
		}
	}
	
	EPD_CS_1;
}

static void EPD_POWERON(void)
{
	EPD_WriteCMD_p1(0x22,0xc0);
	EPD_WriteCMD(0x20);
}

void EPD_POWEROFF(void)
{
	// EPD_WriteCMD_p1(0x22,0xc3);
	EPD_WriteCMD_p1(0x22,0xc7);
	EPD_WriteCMD(0x20);
}


static void EPD_SetRamArea(unsigned char Xstart,unsigned char Xend,
						       unsigned char Ystart,unsigned char Ystart1,unsigned char Yend,unsigned char Yend1)
{
    unsigned char RamAreaX[3];	// X start and end
	unsigned char RamAreaY[5]; 	// Y start and end
	RamAreaX[0] = 0x44;	// command
	RamAreaX[1] = Xstart;
	RamAreaX[2] = Xend;
	RamAreaY[0] = 0x45;	// command
	RamAreaY[1] = Ystart;
	RamAreaY[2] = Ystart1;
	RamAreaY[3] = Yend;
    RamAreaY[4] = Yend1;
	EPD_Write(RamAreaX, sizeof(RamAreaX));
	EPD_Write(RamAreaY, sizeof(RamAreaY));
}
static void EPD_SetRamPointer(unsigned char addrX,unsigned char addrY,unsigned char addrY1)
{
    unsigned char RamPointerX[2];	// default (0,0)
	unsigned char RamPointerY[3]; 	
	RamPointerX[0] = 0x4e;
	RamPointerX[1] = addrX;
	RamPointerY[0] = 0x4f;
	RamPointerY[1] = addrY;
	RamPointerY[2] = addrY1;
	
	EPD_Write(RamPointerX, sizeof(RamPointerX));
	EPD_Write(RamPointerY, sizeof(RamPointerY));
}

static void part_display(unsigned char RAM_XST,unsigned char RAM_XEND,unsigned char RAM_YST,unsigned char RAM_YST1,unsigned char RAM_YEND,unsigned char RAM_YEND1)
{    
	EPD_SetRamArea(RAM_XST,RAM_XEND,RAM_YST,RAM_YST1,RAM_YEND,RAM_YEND1);  	/*set w h*/
    EPD_SetRamPointer (RAM_XST,RAM_YST,RAM_YST1);		    /*set orginal*/
}

//static void EPD_UpdataDisplay(unsigned char *imgbuff,unsigned char xram,unsigned int yram)
//{
//	EPD_WriteDispRam(xram, yram, imgbuff);
////	EPD_Update();
//}

//=========================functions============================

//-------------------------------------------------------
//Func	: void EPD_DispInit(void)
//Desc	: display parameters initinal
//Input	: none
//Output: none
//Return: none
//Author: 
//Date	: 2011/12/24
//-------------------------------------------------------
static void EPD_DispInit(void)
{
	unsigned char GDOControl[]	= {0x01,(yDotMax-1)%256,(yDotMax-1)/256,0x00}; //for *inch
//	unsigned char softstart[]	= {0x0c,0xd7,0xd6,0x9d};
	unsigned char Rambypass[] 	= {0x21,0x03};	// Display update  A[7]=1(Enable bypass)  A[4]=0éåè¶æªç¼è?çå®æ¿å¹éæçéè·¨å¦é·ï¿½(value will be used as for bypass)
	unsigned char MAsequency[] 	= {0x22,0xc7};	// clock
	unsigned char GDVol[] 		= {0x03,0xea};	// Gate voltage +15V/-15V
	unsigned char SDVol[] 		= {0x04,0x0a};	// Source voltage +15V/-15V
	unsigned char VCOMVol[] 	= {0x2c,0x5a};	// VCOM=-4.2V+5A*0.02=-2.4V
//	unsigned char BOOSTERFB[] 	= {0xf0,0x1f};	// Source voltage +15V/-15V
	unsigned char DummyLine[] 	= {0x3a,0x06};	// Set 50Hz   A[6:0]=06h[POR] Number of dummy line period in term of TGate
	unsigned char Gatetime[] 	= {0x3b,0x0b};	// A[3:0]=1011(78us)  Line width in us   78us*(250+6)=19968us=19.968ms
	unsigned char BorderWavefrom[] 		= {0x3c,0x33};	// Select border waveform for VBD GS1-->GS1  é»åº¢îé æå´é¾ç»çé¸î?µ¾é¡ç³ãé´æ¿å?ç¼å?îµé©â?¤¤æåé³è?é£é¨é¹å´é¼îå¹éä½¸æ«éè?îéªæderå©µç²î?¿®æÃ¹å®æ³æçæ¿ç·å?è·ºç?æ¤¤æçæµ ï¿?
	unsigned char BorderWavefrom2[] 	= {0x3c,0x73};	// Select border waveform for VBD GS1-->GS1  é»åº¢îé æå´é¾ç»çé¸î?µ¾é¡ç³ãé´æ¿å?ç¼å?îµé©â?¤¤æåé³è?é£é¨é¹å´é¼îå¹éä½¸æ«éè?îéªæderå©µç²î?¿®æÃ¹å®æ³æçæ¿ç·å?è·ºç?æ¤¤æçæµ ï¿?
	unsigned char RamDataEntryMode[] 	= {0x11,0x01};	// Ram data entry mode

	EPD_Write(GDOControl, sizeof(GDOControl));	// Pannel configuration, Gate selection
	EPD_Write(GDVol, sizeof(GDVol));
	EPD_Write(SDVol, sizeof(SDVol));
	EPD_Write(DummyLine, sizeof(DummyLine));	// dummy line per gate
	EPD_Write(Gatetime, sizeof(Gatetime));		// Gage time setting
	EPD_Write(BorderWavefrom, sizeof(BorderWavefrom));		// Gage time setting
	EPD_Write(RamDataEntryMode, sizeof(RamDataEntryMode));	// X increase, Y decrease
	EPD_SetRamArea(0x00,(xDotMax-1)/8,(yDotMax-1)%256,(yDotMax-1)/256,0x00,0x00);	// X-source area,Y-gage area
//	EPD_SetRamPointer(0x00,(yDotMax-1)%256,(yDotMax-1)/256);	// set ram

//    EPD_Write(softstart, sizeof(softstart));	// X decrease, Y decrease
	
	EPD_Write(VCOMVol, sizeof(VCOMVol));		// VCOM settin
	EPD_WirteLUT((unsigned char *)WF_500MS,sizeof(WF_500MS));
	EPD_Write(Rambypass, sizeof(Rambypass));
	EPD_Write(MAsequency, sizeof(MAsequency));
	EPD_Write(BorderWavefrom2, sizeof(BorderWavefrom2));		// VCOM setting
}

void EPD_set_dot_range(uint16_t xDot, uint16_t yDot)
{
	if(xDot == 0 || yDot == 0)
	{
		xDotMax = xMax;
		yDotMax = yMax;
	}else
	{
		xDotMax = xDot;
		yDotMax = yDot;
	}
}

void EPD_Init()
{
	if(xDotMax == 0 || yDotMax == 0)
	{
		xDotMax = xMax;
		yDotMax = yMax;
	}

	EPD_BS_0;		// 4 wire spi mode selected

	EPD_RST_0;		// Module reset
	driver_delay_xms(100);
	EPD_RST_1;
	driver_delay_xms(100); 
}
//-------------------------------------------------------
//Func	: EPD_EnableChargepump
//Desc	: 
//Input	:
//Output: 
//Return: 
//Author: 
//Date	: 2011/12/24
//-------------------------------------------------------
//static void EPD_EnableChargepump(void)
//{
//	EPD_WriteCMD_p1(0xf0,0x8f);
//	EPD_WriteCMD_p1(0x22,0xc0);
//	EPD_WriteCMD(0x20);
//	EPD_WriteCMD(0xff);
//}

//-------------------------------------------------------
//Func	: EPD_DisableChargepump
//Desc	: 
//Input	:
//Output: 
//Return: 
//Author: 
//Date	: 2011/12/24
//-------------------------------------------------------
//static void EPD_DisableChargepump(void)
//{
//	EPD_WriteCMD_p1(0x22,0xf0);
//	EPD_WriteCMD(0x20);
//	EPD_WriteCMD(0xff);
//}
//-------------------------------------------------------
//Func	: EPD_Update
//Desc	: 
//Input	:
//Output: 
//Return: 
//Author: 
//Date	: 2011/12/24
//-------------------------------------------------------
static void EPD_Update(void)
{
	EPD_WriteCMD_p1(0x22,0xc7);//0xc4
	EPD_WriteCMD(0x20);
//	EPD_WriteCMD(0xff);
	ReadBusy();
}

static void EPD_Update_Part(void)
{
	EPD_WriteCMD_p1(0x22,0x04);
	EPD_WriteCMD(0x20);
	EPD_WriteCMD(0xff);
}
//-------------------------------------------------------
//Func	: EPD_WirteLUT(unsigned char *LUTvalue)
//Desc	: write the waveform to the dirver's ram 
//Input	: *LUTvalue, the wavefrom tabe address
//Output: none
//Return: none
//Author: 
//Date	: 2011/12/24
//-------------------------------------------------------
static void EPD_WirteLUT(unsigned char *LUTvalue,unsigned char Size)
{	
	EPD_Write(LUTvalue, Size);
}

void EPD_init_Full(void)
{
	EPD_Init();			// display

	EPD_DispInit();		// pannel configure

	driver_delay_xms(200);
}

void EPD_init_Part(void)
{		
	EPD_Init();			// display
	
	EPD_DispInit();		// pannel configure

	EPD_WirteLUT((unsigned char *)LUTDefault_part,sizeof(LUTDefault_part));

	EPD_POWERON();
	driver_delay_xms(200);
}

void EPD_Dis_Full(unsigned char *DisBuffer,unsigned char Label)
{
    EPD_SetRamPointer(0x00,(yDotMax-1)%256,(yDotMax-1)/256);	// set ram

	if(Label==0)
	{
		EPD_WriteDispRamMono(xDotMax, yDotMax, *DisBuffer);	// white	
	}
	else
	{
		EPD_WriteDispRam(xDotMax, yDotMax, DisBuffer);	// white
	}	

	EPD_Update();	
	driver_delay_xms(200);
}

void EPD_Dis_Part(unsigned char xStart,unsigned char xEnd,unsigned long yStart,unsigned long yEnd,unsigned char *DisBuffer,unsigned char Label)
{
	part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);	// set ram

	if(Label==0)
	{
		EPD_WriteDispRamMono(xEnd-xStart, yEnd-yStart+1, DisBuffer[0]);	// black
	}
	else
	{
		EPD_WriteDispRam(xEnd-xStart, yEnd-yStart+1,DisBuffer);
	}

	EPD_Update_Part();
}
/***********************************************************
						end file
***********************************************************/

