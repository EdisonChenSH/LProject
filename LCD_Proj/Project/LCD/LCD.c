#include "LCD.h"
#include "stm32f10x.h"
#include "delay.h"
#include "nAsciidot.h"
#include "stdio.h"
#include "ff.h"
#include "Process.h"

#define ledSetLight() GPIO_SetBits(GPIOB,GPIO_Pin_11)
#define ledClrLight()	GPIO_ResetBits(GPIOB,GPIO_Pin_11)

#define lcdSetBackLight() GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define lcdClrBackLight() GPIO_ResetBits(GPIOB,GPIO_Pin_9)

#define lcdSetReset() GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define lcdClrReset() GPIO_ResetBits(GPIOB,GPIO_Pin_8)

#define lcdSetCs() GPIO_SetBits(GPIOB,GPIO_Pin_14)
#define lcdClrCs() GPIO_ResetBits(GPIOB,GPIO_Pin_14)

#define lcdSetDC() GPIO_SetBits(GPIOB,GPIO_Pin_15)
#define lcdClrDC() GPIO_ResetBits(GPIOB,GPIO_Pin_15)

#define lcdSetWR() GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define lcdClrWR() GPIO_ResetBits(GPIOA,GPIO_Pin_8)

#define lcdSetRD() GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define lcdClrRD() GPIO_ResetBits(GPIOB,GPIO_Pin_12)

//extern const unsigned char gImage_ico[32768];
extern const unsigned char gImage_ico1[28800];
void LCD_GPIO_Init()
{
	GPIO_InitTypeDef g;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	g.GPIO_Mode=GPIO_Mode_Out_PP;
	//
	g.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7 \
	|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15;
	g.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&g);
	g.GPIO_Pin=GPIO_Pin_8;
	GPIO_Init(GPIOA,&g);
	g.GPIO_Mode=GPIO_Mode_IPU;
	g.GPIO_Pin=GPIO_Pin_10;
	lcdSetBackLight();
}

void lcdGpioPutData(u8 data)
{
	u16 outdata=0;
	outdata=GPIO_ReadOutputData(GPIOB);
	outdata&=0xFF00;
	outdata+=data;
	GPIO_Write(GPIOB,outdata);	
}

void lcdWriteReg(u16 reg)
{
	lcdSetRD();
	lcdClrDC();
	
	lcdClrCs();
	
	lcdClrWR();
	lcdGpioPutData(reg>>8);
	lcdSetWR();
	
	lcdClrWR();
	lcdGpioPutData(reg&0x00ff);
	lcdSetWR();
	
	lcdSetCs();
}



void lcdWriteData(u16 data)
{
	lcdSetRD();
	lcdSetDC();
	
	lcdClrCs();
	
	lcdClrWR();
	lcdGpioPutData(data>>8);
	lcdSetWR();
	
	lcdClrWR();
	lcdGpioPutData(data&0x00ff);
	lcdSetWR();
	
	lcdSetCs();
}
void lcdWriteCmd(u16 idx,u16 data)
{
	lcdSetRD();
	lcdClrDC();
	
	lcdClrCs();
	
	lcdClrWR();
	lcdGpioPutData(idx>>8);
	lcdSetWR();
	
	lcdClrWR();
	lcdGpioPutData(idx&0x00ff);
	lcdSetWR();
	
	lcdSetDC();
	
	lcdClrWR();
	lcdGpioPutData(data>>8);
	lcdSetWR();
	
	lcdClrWR();
	lcdGpioPutData(data&0x00ff);
	lcdSetWR();
	
	lcdSetCs();
}
void LCD_Rst(void)
{			

    lcdClrReset();
    Delay_mS(50);		
		Delay_mS(50);	
		Delay_mS(50);	
		Delay_mS(50);		
    lcdSetReset(); 	 
    Delay_mS(50);		
		Delay_mS(50);	
		Delay_mS(50);	
		Delay_mS(50);
}

void MainLCD_Init(void)
{
	LCD_Rst();

	lcdWriteReg(0X0000);
	lcdWriteReg(0X0000);
	lcdWriteReg(0X0000);
	lcdWriteReg(0X0000);
	
		//************* Start Initial Sequence **********//
	lcdWriteCmd(0x0702, 0x3008);// Set internal timing, don?t change this value
	lcdWriteCmd(0x0705, 0x0036);// Set internal timing, don?t change this value
	lcdWriteCmd(0x070B, 0x1213);// Set internal timing, don?t change this value
	lcdWriteCmd(0x0001, 0x0100);// set SS and SM bit
	lcdWriteCmd(0x0002, 0x0100);// set 1 line inversion
	lcdWriteCmd(0x0003, 0x1038);// set GRAM write direction and BGR=1.
	lcdWriteCmd(0x0008, 0x0202);// set the back porch and front porch
	lcdWriteCmd(0x0009, 0x0000);// set non-display area refresh cycle ISC[3:0]
	lcdWriteCmd(0x000C, 0x0000);// RGB interface setting
	lcdWriteCmd(0x000F, 0x0000);// RGB interface polarity
	//*************Power On sequence ****************//
	lcdWriteCmd(0x0100, 0x0000);// SAP, BT[3:0], AP, DSTB, SLP, STB
	lcdWriteCmd(0x0102, 0x0000);// VREG1OUT voltage
	lcdWriteCmd(0x0103, 0x0000);// VDV[4:0] for VCOM amplitude
	Delay_mS(50);Delay_mS(50);Delay_mS(50);Delay_mS(50);// Dis-charge capacitor power voltage
	lcdWriteCmd(0x0100, 0x1190);// SAP, BT[3:0], AP, DSTB, SLP, STB
	lcdWriteCmd(0x0101, 0x0227);// DC1[2:0], DC0[2:0], VC[2:0]
	Delay_mS(50);                          // Delay 50ms
	lcdWriteCmd(0x0102, 0x01BD);// VREG1OUT voltage
	Delay_mS(50);                          // Delay 50ms
	lcdWriteCmd(0x0103, 0x2500);// VDV[4:0] for VCOM amplitude
	lcdWriteCmd(0x0281, 0x000F);// VCM[5:0] for VCOMH
	Delay_mS(50);
	lcdWriteCmd(0x0200, 0x0000);// GRAM horizontal Address
	lcdWriteCmd(0x0201, 0x0000);// GRAM Vertical Address
	// ----------- Adjust the Gamma Curve ----------//
	lcdWriteCmd(0x0300, 0x0003);
	lcdWriteCmd(0x0301, 0x0707);
	lcdWriteCmd(0x0302, 0x0606);
	lcdWriteCmd(0x0305, 0x0000);
	lcdWriteCmd(0x0306, 0x0D00);
	lcdWriteCmd(0x0307, 0x0706);
	lcdWriteCmd(0x0308, 0x0005);
	lcdWriteCmd(0x0309, 0x0007);
	lcdWriteCmd(0x030C, 0x0000);
	lcdWriteCmd(0x030D, 0x000A);
	//------------------ Set GRAM area ---------------//
	lcdWriteCmd(0x0210, 0x0000);// Horizontal GRAM Start Address
	lcdWriteCmd(0x0211, 0x00EF);// Horizontal GRAM End Address
	lcdWriteCmd(0x0212, 0x0000);// Vertical GRAM Start Address
	lcdWriteCmd(0x0213, 0x018F);// Vertical GRAM End Address
	lcdWriteCmd(0x0400, 0x3100);// Gate Scan Line 400 lines
	lcdWriteCmd(0x0401, 0x0001);// NDL,VLE, REV
	lcdWriteCmd(0x0404, 0x0000);// set scrolling line
	//-------------- Partial Display Control ---------//
	lcdWriteCmd(0x0500, 0x0000);// Partial Image 1 Display Position
	lcdWriteCmd(0x0501, 0x0000);// Partial Image 1 RAM Start/End Address
	lcdWriteCmd(0x0502, 0x0000);// Partial Image 1 RAM Start/End Address
	lcdWriteCmd(0x0503, 0x0000);// Partial Image 2 Display Position
	lcdWriteCmd(0x0504, 0x0000);// Partial Image 2 RAM Start/End Address
	lcdWriteCmd(0x0505, 0x0000);// Partial Image 2 RAM Start/End Address
	//-------------- Panel Control -------------------//
	lcdWriteCmd(0x0003, 0x1018);
	lcdWriteCmd(0x0010, 0x0010);// DIVI[1:0];RTNI[4:0]
	lcdWriteCmd(0x0011, 0x0600);// NOWI[2:0];SDTI[2:0]
	lcdWriteCmd(0x0020, 0x0002);// DIVE[1:0];RTNE[5:0]
	lcdWriteCmd(0x0007, 0x0173);// 262K color and display ON	

	Delay_mS(50);Delay_mS(50);Delay_mS(50);Delay_mS(50);
	
}
void lcdSetRamAddr(u16 x,u16 y)
{
	x=399-x;
	//y=399-y;
	lcdWriteCmd(0x200,y);       
	lcdWriteCmd(0x201,x);        
}

void lcdClearScreen(u16 color)
{
	u32 i=0;
	lcdSetRamAddr(0,0);
	lcdWriteReg(0x202);
	for(i=0;i<96000;i++)
	{
		lcdWriteData(color);
	}
}
void lcdDrawChar(u16 x, u16 y, char Char, uint16_t Color)
{
	unsigned char i = 0,j=0;
	unsigned int temp = y;
	unsigned int padd= 0;
	padd = (Char-32)*16;
	y=temp;
	for(i=0;i<16;i++)
	{
		lcdSetRamAddr(x,y);
		lcdWriteReg(0x202);
		for(j=0;j<8;j++)
		{
			if(ascii_8x16[padd]&(0x80>>j))
			{
				lcdWriteData(Color);
			}
			else
			{
				lcdWriteData(BLACK);
			}
		}
		padd++;
		y++;
	}
}

u16 lcdXAddr=0,lcdYaddr=0;
void reDefinePrintfAddr(u16 x,u16 y)
{
	lcdXAddr=x;
	lcdYaddr=y;
}
void lcdOpenWindows(u16 x,u16 y,u16 width,u16 height)
{
	u16 xStartRam,yStartRam,xEndRam,yEndRam;
	
	xStartRam=y;
	xEndRam=y+(height-1);
	
	yStartRam=399-x-(width-1);
	yEndRam=399-x;
	
	lcdWriteCmd(0x0210, xStartRam);// Horizontal GRAM Start Address
	lcdWriteCmd(0x0211, xEndRam);// Horizontal GRAM End Address
	
	lcdWriteCmd(0x0212, yStartRam);// Vertical GRAM Start Address
	lcdWriteCmd(0x0213, yEndRam);// Vertical GRAM End Address
}
void lcdFillColor(u16 x,u16 y,u16 width,u16 height,u16 color)
{
	u32 i=0;
	lcdSetRamAddr(x,y);
	lcdOpenWindows(x,y,width,height);
	lcdWriteReg(0x202);
	for(i=0;i<width*height;i++)
	{
		lcdWriteData(color);
	}
	lcdOpenWindows(0,0,lcdWIDTH,lcdHEIGHT);
	
}
void lcdDrawPic(u16 x,u16 y,u16 width,u16 height,const u8* pic)
{
	u32 i=0;
	u16 pix=0;
	lcdSetRamAddr(x,y);
	lcdOpenWindows(x,y,width,height);
	lcdWriteReg(0x202);
	for(i=0;i<width*height;i++)
	{
		pix=*pic;
		pix<<=8;
		pix+=*(pic+1);
		lcdWriteData(pix);
		pic+=2;
	}
	lcdOpenWindows(0,0,lcdWIDTH,lcdHEIGHT);
}
void lcdLedSet(u8 isTurnOn)
{
	if(isTurnOn)
	{
		ledSetLight();
		lcdSetBackLight();
		lcdWriteCmd(0x0007, 0x0173);
	}
	else
	{
		ledClrLight();
		lcdClrBackLight();
		lcdWriteCmd(0x0007, 0x0172);		
	}
}
u8 picWriteBuf[1024];

	PicInfo picinfo;
	FIL picfile;
	FRESULT res;
	u32 btr;	
	u16 i=0,j=0;
	u16 readCount;
	u16 lastReadLength;

void lcdDrawPicFromFs(const char* path)
{

	
	lcdClearScreen(BLACK);
	res=f_open(&picfile,path,FA_READ);
	res=f_read(&picfile,(void*)(&picinfo),sizeof(picinfo),&btr);
	
	readCount=picinfo.PicSize/1024;
	lastReadLength=picinfo.PicSize%1024;
	if(lastReadLength!=0)readCount++;
	else lastReadLength=1024;
	lcdSetRamAddr(picinfo.x,picinfo.y);
	lcdOpenWindows(picinfo.x,picinfo.y,picinfo.weith,picinfo.height);
	lcdWriteReg(0x202);
	for(i=0;i<readCount;i++)
	{
		if(i!=(readCount-1))
		{
			res=f_read(&picfile,picWriteBuf,1024,&btr);
			for(j=0;j<1024;)
			{
				lcdWriteData((picWriteBuf[j]<<8)+(picWriteBuf[j+1]));
				j+=2;
			}	
		}
		else
		{
			res=f_read(&picfile,picWriteBuf,lastReadLength,&btr);
			for(j=0;j<lastReadLength;)
			{
				lcdWriteData((picWriteBuf[j]<<8)+(picWriteBuf[j+1]));
				j+=2;
			}				
		}
	}
	f_close(&picfile);
	lcdOpenWindows(0,0,lcdWIDTH,lcdHEIGHT);	
}
u8 readPhotoSenor()
{
	return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10);
}
void lcdAllInit()
{	
	LCD_GPIO_Init();
	MainLCD_Init();
	lcdClearScreen(BLACK);
	lcdFillColor(100,100,50,100,LIGHTBLUE);	
	lcdDrawPic(0,0,120,120,gImage_ico1);
}



// 将printf(...)函数输出定向到LCD
#ifdef __GNUC__ 
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
	if(lcdXAddr > lcdWIDTH - 8)  //X_MAX????, ???240
	{
		lcdXAddr = 0;           //x??
		lcdYaddr += 16;         //y?????
	}
	if(lcdYaddr > lcdHEIGHT - 16)  //Y_MAX????, ???320
	{
		lcdYaddr=0;lcdClearScreen(BLACK);
	}
	if(ch == '\n')
	{
		lcdXAddr = 0;
		lcdYaddr += 16;
		return ch;
	}
	lcdDrawChar(lcdXAddr, lcdYaddr, (int)ch, WHITE);
	lcdXAddr += 8; 
return ch;
}