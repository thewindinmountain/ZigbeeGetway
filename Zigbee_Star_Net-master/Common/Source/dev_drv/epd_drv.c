#include "epd_spi_drv.h"
#include "epd_drv.h"
#include "epd_drv_Config.h"
#include "hw_gpio.h"
#include "hw_config.h"

uint16_t xDotMax = 0;
uint16_t yDotMax = 0;

const unsigned char WF_200MS[]={	//濠电偞鎸�?��娑氣偓瑙�?��閺佸秹鎮滈崶鈺侇槱闂佹�?绋掑ú宥�?礈閿燂拷
	0x32,
	0x44,0xAA,0x55,0xAA,0x55,0xAA,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xAA,0xAA,0xAA,0x1A,0x00,0x00,0x00,0x00,0x00,0x00,
};
const unsigned char WF_300MS[]={	//闂佹寧绋掔敮顏嗘閳哄懎鐒洪柛鎰硶鎼村﹥绻涢幘顖涙珔閻庤濞婇崺澶�?�韫囨洖顦╅梺鎸庣⊕濞插秹宕滈敓锟�
	0x32,
	0x44,0xAA,0x55,0xAA,0x55,0xAA,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,
};
const unsigned char WF_500MS[]={	//闂佹寧绋掗崺濠勬閳哄懎鐒洪柛鎰硶鎼村﹥绻涢幘顖涙珔閻庤濞婇崺澶�?�韫囨洝袝闂佹�?绋掑ú宥�?礈閿燂拷
	0x32,
	0x44,0x44,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA,0x11,0x11,0x00,0x00,
	0x00,0x00,0x00,0x00,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0x01,0x00,0x00,
};
const unsigned char WF_700MS[]={	//濠电偞鎸�?��娑氣偓瑙�?��閸╁濡惰箛鏇熸倷闂佹�?绋掑ú宥�?礈閿燂拷
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
闂備礁鎲″濠氬窗閺囥垹绀傛慨妞�?亾闁轰�?绉撮～婵囶潙閺嵮�?��濠电偞鍨堕幖顐﹀�?��戣姤鐓傛繝濠傛噳閸�?挸鈽夊▎�?���?��犵妲呴崣鍐ㄧ暦濡　鏋庨�?��ㄦ尭�??煡�?哄Ч鍥у閻庢凹鍘煎嵄闁归棿绀佸婵�?煙鐎涙�?���?��顔哄灲閺屾稒绻濋崟鈹惧亾鐎ｎ剚顫曟繝闈涱儏缁�?��ユ煃鐞涒�?��壕缂傚倸绉甸崹鍧�?箚閸愵喖绀�?��鎺嶇悼閿曞倹鐓曟繛鍡�?箰閳ь剙缍婂鎶芥偄閸忓吋娅栭梺璺ㄥ櫐閹凤�?
闂備線�?荤拹鐔煎礉閹达箑鍌ㄩ柕鍫濇川绾剧偓銇�?��妤€浜惧┑鈽嗗灟缁€渚€鍩㈤幘璇�?��缂佸绨遍弸蹇涙⒑濞�?��绶涢柟鍑ゆ�?
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
闂備礁鎲″濠氬窗閺囥垹绀傛慨妞�?亾闁轰�?绉撮～婵嬵敆閳ь剟�?��幖浣�?��闁告洦鍋�?��鏃堟煏閸ャ劎鎽ffer
闂備礁鎲￠悷銉╁磹瑜版帒姹查柣鏂�?��閺咁剟鏌ㄩ悤鍌涘�?	XSize  x闂備礁鎼崐濠氬箠閹捐绠栨繝濠傜�?閺�?礁霉閸忓吋缍戞繛鍜冩�?  128闂備胶绮崝姗€锝炴径鎰摕闁绘梻鍘х憴锕傛煙閹碱厼鐏ｇ紒鈧径鎰仯閻庯綆浜滈弳鐐烘煛閸☆厾绋荤紒瀣樀閺佹挻绂掔€ｎ偄鍓梺鍛婃�?閸撴艾鈻撴繝姘厸濞达綀�??欢鏌ユ煥閻曞倹瀚�
		YSize  y闂備礁鎼崐濠氬箠閹捐绠栨繝濠傜�?閺�?礁霉閸忓吋缍戞繛鍜冩�?
		Dispbuff 闂備礁鎼€氼剚鏅舵禒瀣�?��慨妯挎硾閺�?線鏌ｅΔ鈧悧鍡欑矈閿曗偓鑿愰柛銉ｅ�?缁犳捇鏌ｆ幊閸斿秹骞忛崨顓у悑闁搞�?��滈悗顓㈡⒑闂堟稒锛旈柟�?庡厴閹兘�?村鍗炲闂佹椿浜欓崡鍐�?��椤愶附鍋�?��顐ｇ箘閵�?��姊哄Ч鍥у閻庢凹鍘鹃弫顕€骞�?拠鑼槴濠德板€撻悞锕傛偂濞嗘垟鍋撳▓鍨灈缂佺�?��楅幑銏ゅ焵椤掆偓椤啴濡堕崶鈺佺濡�?��甯楃划搴♀槈閻㈠憡鏅搁柨鐕傛�?
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
闂備礁鎲″濠氬窗閺囥垹绀傛慨妞�?亾闁轰�?绉归�?��绘晸閿燂�? 闂備礁鎲￠崝鏍偡閿曗偓铻ｅ璺虹灱閻棛绱掗敍鍕垫ffer 闂備胶鍎甸崑鎾�?礉瀹ュ拋鐒介柣銏犲悁閻掑﹪鏌涢埄鍐剧劷闁告ɑ鐟ラ埥澶�??閸曨剙�?堕梻浣�?级缁挸鐣烽悩璇�?��闁告洦鍘奸獮宥�?⒑閸涘﹤�?��柣鐔村劦瀹曪綁宕卞☉娆忓壆缂備�?顑嗙�?��囧磹闁秵鐓熼柕濞垮劚椤忣亝淇婇�?兼Ш缂侇喖鐗撳畷�?﹀礋椤愶絾娈介梺璇叉捣閹虫挾绮诲澶�?櫢闁跨噦鎷�
闂備礁鎲￠悷銉╁磹瑜版帒姹查柣鏂�?��閺咁剟鏌ㄩ悤鍌涘�?	XSize  x闂備礁鎼崐濠氬箠閹捐绠栨繝濠傜�?閺�?礁霉閸忓吋缍戞繛鍜冩�?  128闂備胶绮崝姗€锝炴径鎰摕闁绘梻鍘х憴锕傛煙閹碱厼鐏ｇ紒鈧径鎰仯閻庯綆浜滈弳鐐烘煛閸☆厾绋荤紒瀣樀閺佹挻绂掔€ｎ偄鍓梺鍛婃�?閸撴艾鈻撴繝姘厸濞达綀�??欢鏌ユ煥閻曞倹瀚�
		YSize  y闂備礁鎼崐濠氬箠閹捐绠栨繝濠傜�?閺�?礁霉閸忓吋缍戞繛鍜冩�?
		dispdata 闂備礁鎼€氼剚鏅舵禒瀣�?��慨妯挎硾閺�?線鏌ｅΔ鈧悧鍡欑矈閿曞倹鐓忛柛鈽嗗幗鐎氾拷
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
	unsigned char Rambypass[] 	= {0x21,0x03};	// Display update  A[7]=1(Enable bypass)  A[4]=0闂備胶枪缁�?牓宕濋幋鐘插灊闁跨噦鎷�(value will be used as for bypass)
	unsigned char MAsequency[] 	= {0x22,0xc7};	// clock
	unsigned char GDVol[] 		= {0x03,0xea};	// Gate voltage +15V/-15V
	unsigned char SDVol[] 		= {0x04,0x0a};	// Source voltage +15V/-15V
	unsigned char VCOMVol[] 	= {0x2c,0x5a};	// VCOM=-4.2V+5A*0.02=-2.4V
//	unsigned char BOOSTERFB[] 	= {0xf0,0x1f};	// Source voltage +15V/-15V
	unsigned char DummyLine[] 	= {0x3a,0x06};	// Set 50Hz   A[6:0]=06h[POR] Number of dummy line period in term of TGate
	unsigned char Gatetime[] 	= {0x3b,0x0b};	// A[3:0]=1011(78us)  Line width in us   78us*(250+6)=19968us=19.968ms
	unsigned char BorderWavefrom[] 		= {0x3c,0x33};	// Select border waveform for VBD GS1-->GS1  閻庢鍠掗崑鎾绘煛閸�?��顏犳い鎴濇�?缁�?鍩�?��掆偓閳�?酣鍨鹃崘鑼幈闂佸搫鍊�?鐪攔der婵炲�?��村ù宄扳枍瑜斿畷�?跺�?椤旂瓔浠�?
	unsigned char BorderWavefrom2[] 	= {0x3c,0x73};	// Select border waveform for VBD GS1-->GS1  閻庢鍠掗崑鎾绘煛閸�?��顏犳い鎴濇�?缁�?鍩�?��掆偓閳�?酣鍨鹃崘鑼幈闂佸搫鍊�?鐪攔der婵炲�?��村ù宄扳枍瑜斿畷�?跺�?椤旂瓔浠�?
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

