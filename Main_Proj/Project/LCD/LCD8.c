#include "LCD8.h"
#include "stdlib.h"
#include "delay.h"
#include "stdio.h"
#include "font.h"

#define LCD8_delay()   Delay_uS(1)

/*
LCD ports
PC1:5 - CS1:5
PC9:13 - CS6:10

RS PC8  // low for control, high for data
WR PC7  // low for write
RD PC6  // low for read

DB17-DB10 PB7:0
*/
/*
PA8 - WR
PA9 - RD
PB15- RS
PB14- LCD_CS
PB8 - LCD_ESET
PB9 - LCD_PWM
PB7:0-DB15-DB8
*/
static u16 CURR_CS_MASK = 0x3e3e;
int LCD8_Curr_CS;

#define LCD8_CS_MASK_ALL  0x3e3e

#define	LCD8_CS_H()   GPIOB->BSRR=(1<<14)
#define	LCD8_RS_H()   GPIOB->BSRR=(1<<15)
#define	LCD8_WR_H()   GPIOA->BSRR=(1<<8)
#define	LCD8_RD_H()   GPIOA->BSRR=(1<<9)

#define LCD8_ctl_H()  GPIOC->BSRR=(0x3ffe)//无用

#define	LCD8_CS_L()   GPIOB->BRR=(1<<14)
#define	LCD8_RS_L()   GPIOB->BRR=(1<<15)
#define	LCD8_WR_L()   GPIOA->BRR=(1<<8)
#define	LCD8_RD_L()   GPIOA->BRR=(1<<9)

#define LCD8_ctl_L()  GPIOC->BRR=(0x3ffe)//无用



// PB 7:0
#define LCD8_DoutMode()    GPIOB->CRL=0x33333333
#define LCD8_DinMode()     GPIOB->CRL=0x44444444
#define LCD8_putData(data) GPIOB->ODR = ((GPIOB->ODR & 0xff00)|data)
#define LCD8_readData()    GPIOB->IDR


#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 312

//画笔颜色,背景颜色
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	
u8 lcd_i; 

void LCD8_resync(void);
void LCD8_WriteU8(u8);
void LCD8_WriteU16(u16);
u16 LCD8_ReadU16(void);
u16 LCD8_ReadReg(u16 addr);
void LCD8_WriteReg(u16 addr,u16 data);
void LCD8_WriteData(u16 data);
void LCD8_init_config(void);
void LCD8_WriteControl(u16 data);


int LCD8_GetCurrCS(void) {
	return LCD8_Curr_CS;
}

// 1-10, 0 for all
void LCD8_SetCS(int i) {
//	printf("LCD8 CS = %d\n",i);
	LCD8_Curr_CS = i;
	if (i==0) {
		CURR_CS_MASK = 0x3e3e;
		return;
	}
	if (i<=5) {
		CURR_CS_MASK = (0x0001 << i);
		return;
	}
	if (i<=10) {
		CURR_CS_MASK = (0x0001 << (i+3));
		return;
	}
	
	CURR_CS_MASK = 0x3e3e;
	return;
}


void LCD8_Demo() {
	int colori;
	int i;
	u16 colors[] = {
		0xFFFF,0x0000,0x001F,0xF81F,
		0xFFE0,0x07FF,0xF800,0xF81F,
		0x07E0,0x7FFF,0xFFE0,0xBC40,
		0xFC07,0x8430,0x01CF,0x7D7C,
		0x5458
	};
	
	// set window
	LCD8_WriteReg(0x212, 0);
	LCD8_WriteReg(0x213, 400-1);
	LCD8_WriteReg(0x210, 0);
	LCD8_WriteReg(0x211, 240-1);
	
	// set start x, y
	LCD8_WriteReg(0x201, 0);
	LCD8_WriteReg(0x200, 0);
	
	// set g ram prepare
	LCD8_WriteControl(0x202);
	
	
	// write data
	while (1) {
		u16 color;
		if (colori >=17) 
			colori = 0;
		else 
			colori ++;
		
		color = colors[colori];
		for (i=0;i<(240*400);i++) {
			LCD8_WriteData(color);
		}
		Delay_mS(65);
	}
	
}


void LCD8_Init() {
  /*
	PA8 - WR
	PA9 - RD
	PB15- RS
	PB14- LCD_CS
	PB8 - LCD_ESET
	PB9 - LCD_PWM
	PB7:0-DB15-DB8
	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | \
	                              GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	LCD8_ctl_H();
	LCD8_DoutMode();
	LCD8_resync();	
	LCD8_init_config();
}


void LCD8_TestPin(void) {
	int i;
	u16 CS_LUT[]= {
	0x0002,0x0004,0x0008,0x0010,0x0020,
	0x0200,0x0400,0x0800,0x1000,0x2000};

	LCD8_ctl_L();
	LCD8_putData(0);

	for (i=0;i<10;i++) {
		CURR_CS_MASK = CS_LUT[i];
		LCD8_CS_H();
		Delay_mS(60);
		LCD8_CS_L();
	}

	LCD8_RS_H(); Delay_mS(60);
	LCD8_ctl_L();
	LCD8_WR_H(); Delay_mS(60);
	LCD8_ctl_L();
	LCD8_RD_H(); Delay_mS(60);
	LCD8_ctl_L();

	for (i=0;i<10;i++) {
		LCD8_putData(0x01<<i); Delay_mS(60);
	}
}

void LCD8_GoIdle(void) {
	LCD8_ctl_L();
	LCD8_delay();
	LCD8_putData(0x00);
	LCD8_delay();
	GPIOC->BRR = LCD8_CS_MASK_ALL;
	LCD8_delay();
}

void LCD8_LeaveIdle(void) {
	GPIOC->BSRR = LCD8_CS_MASK_ALL;
	LCD8_delay();
	LCD8_ctl_H();
	LCD8_delay();
}

void LCD8_WriteU8(u8 data) {

	Delay_mS(1);
	LCD8_putData(data);
	LCD8_WR_L();
	
	Delay_mS(1);
	LCD8_WR_H();
}

void LCD8_WriteU16(u16 data) {
	LCD8_CS_L();
	LCD8_WR_H();
	LCD8_RD_H();
	
	LCD8_putData(data>>8);	 
	LCD8_delay();
	LCD8_WR_L();   
	LCD8_delay();
	LCD8_WR_H();
	
	LCD8_putData(data&0xff);  
	LCD8_delay();
	LCD8_WR_L();   
	LCD8_delay();
	
	LCD8_WR_H();
	LCD8_CS_H();
}


void LCD8_WriteControl(u16 data) {
	LCD8_RS_L();	 
	LCD8_delay();
	LCD8_WriteU16(data);
}


void LCD8_WriteData(u16 data) {
	LCD8_RS_H();
	LCD8_WR_H();  LCD8_RD_H();
	
	LCD8_WriteU16(data);
}


void LCD8_WriteReg(u16 addr,u16 data) {
	LCD8_WriteControl(addr);  LCD8_delay();
	LCD8_WriteData(data);   LCD8_delay();
}

u16 LCD8_ReadU16(void) {
	u16 ans = 0;
	
	Delay_mS(2);LCD8_DinMode();
	Delay_mS(2);LCD8_RD_L();
	Delay_mS(2);
	ans = LCD8_readData();
	Delay_mS(2);LCD8_RD_H();
	
	ans <<= 8;
	ans &= 0xff00;
	
	Delay_mS(2);LCD8_RD_L();
	Delay_mS(2);
	ans |= (0xff & LCD8_readData());
	Delay_mS(2);LCD8_RD_H();
	
	Delay_mS(2);LCD8_CS_H();
	
	Delay_mS(2);LCD8_DoutMode();
	return ans;
}


u16 LCD8_ReadReg(u16 addr) {
	u16 ans;
    // write addr
	Delay_mS(2);LCD8_RS_L();
	Delay_mS(2);LCD8_CS_L();
	
	Delay_mS(2);LCD8_putData(addr>>8);
	Delay_mS(2);LCD8_WR_L();
	Delay_mS(2);LCD8_WR_H();

	Delay_mS(2);LCD8_putData(addr&0xff);
	Delay_mS(2);LCD8_WR_L();
	Delay_mS(2);LCD8_WR_H();
	Delay_mS(2);
	Delay_mS(2);LCD8_CS_H();
	Delay_mS(2);LCD8_RS_H();


    // read data
	Delay_mS(2);LCD8_RS_L();
	Delay_mS(2);LCD8_CS_L();
	Delay_mS(2);LCD8_DinMode();
	Delay_mS(2);LCD8_RD_L();
	Delay_mS(2);ans = LCD8_readData();
	Delay_mS(2);LCD8_RD_H();
	ans <<= 8;
	ans &= 0xff00;
	
	Delay_mS(2);LCD8_RD_L();
	Delay_mS(2);
	ans |= (0xff & LCD8_readData());
	Delay_mS(2);LCD8_RD_H();
	
	Delay_mS(2);LCD8_CS_H();
	Delay_mS(2);LCD8_RS_H();
	Delay_mS(2);LCD8_DoutMode();
	return ans;
}



void LCD8_resync() {
	Delay_mS(2);LCD8_ctl_H();
	Delay_mS(2);LCD8_RS_L();
	Delay_mS(2);LCD8_CS_L();
	Delay_mS(2);LCD8_WriteU8(0x00);
	Delay_mS(2);LCD8_WriteU8(0x00);
	Delay_mS(2);LCD8_WriteU8(0x00);
	Delay_mS(2);LCD8_WriteU8(0x00);
	Delay_mS(2);LCD8_CS_H();
	Delay_mS(2);LCD8_RS_H();
}


//void ILI9326_PVI28_Initial(void)
//{
//// VCI=2.8V
////************* Reset LCD Driver ****************//
//LCD_nRESET = 1;
//delayms(1);                           // Delay 1ms
//LCD_nRESET = 0;
//delayms(10);                          // Delay 10ms // This delay time is necessary
//LCD_nRESET = 1;
//delayms(50);                          // Delay 50 ms
////************* Start Initial Sequence **********//
//LCD_CtrlWrite_ILI9326(0x0702, 0x2008);// Set internal timing, don?t change this value
//LCD_CtrlWrite_ILI9326(0x0705, 0x0036);// Set internal timing, don?t change this value
//LCD_CtrlWrite_ILI9326(0x070B, 0x1213);// Set internal timing, don?t change this value
//LCD_CtrlWrite_ILI9326(0x0001, 0x0100);// set SS and SM bit
//LCD_CtrlWrite_ILI9326(0x0002, 0x0100);// set 1 line inversion
//LCD_CtrlWrite_ILI9326(0x0003, 0x1030);// set GRAM write direction and BGR=1.
//LCD_CtrlWrite_ILI9326(0x0008, 0x0202);// set the back porch and front porch
//LCD_CtrlWrite_ILI9326(0x0009, 0x0000);// set non-display area refresh cycle ISC[3:0]
//LCD_CtrlWrite_ILI9326(0x000C, 0x0000);// RGB interface setting
//LCD_CtrlWrite_ILI9326(0x000F, 0x0000);// RGB interface polarity
////*************Power On sequence ****************//
//LCD_CtrlWrite_ILI9326(0x0100, 0x0000);// SAP, BT[3:0], AP, DSTB, SLP, STB
//LCD_CtrlWrite_ILI9326(0x0102, 0x0000);// VREG1OUT voltage
//LCD_CtrlWrite_ILI9326(0x0103, 0x0000);// VDV[4:0] for VCOM amplitude
//delayms(200);                         // Dis-charge capacitor power voltage
//LCD_CtrlWrite_ILI9326(0x0100, 0x1190);// SAP, BT[3:0], AP, DSTB, SLP, STB
//LCD_CtrlWrite_ILI9326(0x0101, 0x0227);// DC1[2:0], DC0[2:0], VC[2:0]
//delayms(50);                          // Delay 50ms
//LCD_CtrlWrite_ILI9326(0x0102, 0x01BD);// VREG1OUT voltage
//delayms(50);                          // Delay 50ms
//LCD_CtrlWrite_ILI9326(0x0103, 0x3500);// VDV[4:0] for VCOM amplitude
//LCD_CtrlWrite_ILI9326(0x0281, 0x0014);// VCM[5:0] for VCOMH
//delayms(50);
//LCD_CtrlWrite_ILI9326(0x0200, 0x0000);// GRAM horizontal Address
//LCD_CtrlWrite_ILI9326(0x0201, 0x0000);// GRAM Vertical Address
//// ----------- Adjust the Gamma Curve ----------//
//LCD_CtrlWrite_ILI9326(0x0300, 0x0003);
//LCD_CtrlWrite_ILI9326(0x0301, 0x0607);
//LCD_CtrlWrite_ILI9326(0x0302, 0x0207);
//LCD_CtrlWrite_ILI9326(0x0305, 0x0001);
//LCD_CtrlWrite_ILI9326(0x0306, 0x080C);
//LCD_CtrlWrite_ILI9326(0x0307, 0x0004);
//LCD_CtrlWrite_ILI9326(0x0308, 0x0000);
//LCD_CtrlWrite_ILI9326(0x0309, 0x0407);
//LCD_CtrlWrite_ILI9326(0x030C, 0x0404);
//LCD_CtrlWrite_ILI9326(0x030D, 0x0F0A);
////------------------ Set GRAM area ---------------//
//LCD_CtrlWrite_ILI9326(0x0210, 0x0000);// Horizontal GRAM Start Address
//LCD_CtrlWrite_ILI9326(0x0211, 0x00EF);// Horizontal GRAM End Address
//LCD_CtrlWrite_ILI9326(0x0212, 0x0000);// Vertical GRAM Start Address
//LCD_CtrlWrite_ILI9326(0x0213, 0x01AF);// Vertical GRAM Start Address
//LCD_CtrlWrite_ILI9326(0x0400, 0x3100);// Gate Scan Line 400 lines
//LCD_CtrlWrite_ILI9326(0x0401, 0x0001);// NDL,VLE, REV
//LCD_CtrlWrite_ILI9326(0x0404, 0x0000);// set scrolling line
////-------------- Partial Display Control ---------//
//LCD_CtrlWrite_ILI9326(0x0500, 0x0000);// Partial Image 1 Display Position
//LCD_CtrlWrite_ILI9326(0x0501, 0x0000);// Partial Image 1 RAM Start/End Address
//LCD_CtrlWrite_ILI9326(0x0502, 0x0000);// Partial Image 1 RAM Start/End Address
//LCD_CtrlWrite_ILI9326(0x0503, 0x0000);// Partial Image 2 Display Position
//LCD_CtrlWrite_ILI9326(0x0504, 0x0000);// Partial Image 2 RAM Start/End Address
//LCD_CtrlWrite_ILI9326(0x0505, 0x0000);// Partial Image 2 RAM Start/End Address
////-------------- Panel Control -------------------//
//LCD_CtrlWrite_ILI9326(0x0010, 0x0010);// DIVI[1:0];RTNI[4:0]
//LCD_CtrlWrite_ILI9326(0x0011, 0x0600);// NOWI[2:0];SDTI[2:0]
//LCD_CtrlWrite_ILI9326(0x0020, 0x0002);// DIVE[1:0];RTNE[5:0]
//LCD_CtrlWrite_ILI9326(0x0007, 0x0173);// 262K color and display ON
//}

//void LCD_ExitSleep_ILI9326(void)
//{
////*************Power On sequence ******************//
//LCD_CtrlWrite_ILI9326(0x0100, 0x0000);// SAP, BT[3:0], AP, DSTB, SLP
//delayms(200);                         // Dis-charge capacitor power voltage
//LCD_CtrlWrite_ILI9326(0x0100, 0x1190);// SAP, BT[3:0], AP, DSTB, SLP, STB
//LCD_CtrlWrite_ILI9326(0x0101, 0x0227);// DC1[2:0], DC0[2:0], VC[2:0]
//delayms(50);                          // Delay 50ms
//LCD_CtrlWrite_ILI9326(0x0102, 0x01BD);// VREG1OUT voltage
//delayms(50);                          // Delay 50ms
//LCD_CtrlWrite_ILI9326(0x0103, 0x3800);// VDV[4:0] for VCOM amplitude
//delayms(50);                          // Delay 50ms
//LCD_CtrlWrite_ILI9326(0x0007, 0x0173);// 262K color and display ON
//}

//void LCD_EnterSleep_ILI9326(void)
//{
//LCD_CtrlWrite_ILI9326(0x0007, 0x0131);// Set D1=0, D0=1
//delayms(10);
//LCD_CtrlWrite_ILI9326(0x0007, 0x0130);// Set D1=0, D0=0
//delayms(10);
//LCD_CtrlWrite_ILI9326(0x0007, 0x0000);// display OFF
////************* Power OFF sequence **************//
//LCD_CtrlWrite_ILI9326(0x0100, 0x0000);// SAP, BT[3:0], APE, AP, DSTB, SLP
//delayms(200);                         // Dis-charge capacitor power voltage
//LCD_CtrlWrite_ILI9326(0x0100, 0x0002);// SAP, BT[3:0], APE, AP, DSTB, SLP
//}

void LCD8_init_config() {
	
	//************* Start Initial Sequence **********//
	LCD8_WriteReg(0x0702, 0x2008);// Set internal timing, don?t change this value
	LCD8_WriteReg(0x0705, 0x0036);// Set internal timing, don?t change this value
	LCD8_WriteReg(0x070B, 0x1213);// Set internal timing, don?t change this value
	LCD8_WriteReg(0x0001, 0x0100);// set SS and SM bit
	LCD8_WriteReg(0x0002, 0x0100);// set 1 line inversion
	LCD8_WriteReg(0x0003, 0x1030);// set GRAM write direction and BGR=1.
	LCD8_WriteReg(0x0008, 0x0202);// set the back porch and front porch
	LCD8_WriteReg(0x0009, 0x0000);// set non-display area refresh cycle ISC[3:0]
	LCD8_WriteReg(0x000C, 0x0000);// RGB interface setting
	LCD8_WriteReg(0x000F, 0x0000);// RGB interface polarity
	//*************Power On sequence ****************//
	LCD8_WriteReg(0x0100, 0x0000);// SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD8_WriteReg(0x0102, 0x0000);// VREG1OUT voltage
	LCD8_WriteReg(0x0103, 0x0000);// VDV[4:0] for VCOM amplitude
	Delay_mS(50);Delay_mS(50);Delay_mS(50);Delay_mS(50);// Dis-charge capacitor power voltage
	LCD8_WriteReg(0x0100, 0x1190);// SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD8_WriteReg(0x0101, 0x0227);// DC1[2:0], DC0[2:0], VC[2:0]
	Delay_mS(50);                          // Delay 50ms
	LCD8_WriteReg(0x0102, 0x01BD);// VREG1OUT voltage
	Delay_mS(50);                          // Delay 50ms
	LCD8_WriteReg(0x0103, 0x3500);// VDV[4:0] for VCOM amplitude
	LCD8_WriteReg(0x0281, 0x0014);// VCM[5:0] for VCOMH
	Delay_mS(50);
	LCD8_WriteReg(0x0200, 0x0000);// GRAM horizontal Address
	LCD8_WriteReg(0x0201, 0x0000);// GRAM Vertical Address
	// ----------- Adjust the Gamma Curve ----------//
	LCD8_WriteReg(0x0300, 0x0003);
	LCD8_WriteReg(0x0301, 0x0607);
	LCD8_WriteReg(0x0302, 0x0207);
	LCD8_WriteReg(0x0305, 0x0001);
	LCD8_WriteReg(0x0306, 0x080C);
	LCD8_WriteReg(0x0307, 0x0004);
	LCD8_WriteReg(0x0308, 0x0000);
	LCD8_WriteReg(0x0309, 0x0407);
	LCD8_WriteReg(0x030C, 0x0404);
	LCD8_WriteReg(0x030D, 0x0F0A);
	//------------------ Set GRAM area ---------------//
	LCD8_WriteReg(0x0210, 0x0000);// Horizontal GRAM Start Address
	LCD8_WriteReg(0x0211, 0x00EF);// Horizontal GRAM End Address
	LCD8_WriteReg(0x0212, 0x0000);// Vertical GRAM Start Address
	LCD8_WriteReg(0x0213, 0x01AF);// Vertical GRAM Start Address
	LCD8_WriteReg(0x0400, 0x3100);// Gate Scan Line 400 lines
	LCD8_WriteReg(0x0401, 0x0001);// NDL,VLE, REV
	LCD8_WriteReg(0x0404, 0x0000);// set scrolling line
	//-------------- Partial Display Control ---------//
	LCD8_WriteReg(0x0500, 0x0000);// Partial Image 1 Display Position
	LCD8_WriteReg(0x0501, 0x0000);// Partial Image 1 RAM Start/End Address
	LCD8_WriteReg(0x0502, 0x0000);// Partial Image 1 RAM Start/End Address
	LCD8_WriteReg(0x0503, 0x0000);// Partial Image 2 Display Position
	LCD8_WriteReg(0x0504, 0x0000);// Partial Image 2 RAM Start/End Address
	LCD8_WriteReg(0x0505, 0x0000);// Partial Image 2 RAM Start/End Address
	//-------------- Panel Control -------------------//
	LCD8_WriteReg(0x0010, 0x0010);// DIVI[1:0];RTNI[4:0]
	LCD8_WriteReg(0x0011, 0x0600);// NOWI[2:0];SDTI[2:0]
	LCD8_WriteReg(0x0020, 0x0002);// DIVE[1:0];RTNE[5:0]
	LCD8_WriteReg(0x0007, 0x0173);// 262K color and display ON	

	Delay_mS(50);Delay_mS(50);Delay_mS(50);Delay_mS(50);
}


void LCD8_Clear(u16 color) {
	int i;
	
	// set window
	LCD8_WriteReg(0x212, 0);
	LCD8_WriteReg(0x213, 400-1);
	LCD8_WriteReg(0x210, 0);
	LCD8_WriteReg(0x211, 240-1);
	
	// set start x, y
	LCD8_WriteReg(0x201, 0);
	LCD8_WriteReg(0x200, 0);
	
	// set g ram prepare
	LCD8_WriteControl(0x202);
	
	// write data
	for (i=0;i<(240*400);i++) {
		LCD8_WriteData(color);
	}
}


void LCD8_DrawPoint(u16 color,u16 x, u16 y) {
	LCD8_WriteReg(0x201, x);
	LCD8_WriteReg(0x200, y);
	LCD8_WriteControl(0x202);
	LCD8_WriteData(color);
}

u16 LCD8_color;
u16 LCD8_x;
u16 LCD8_y;

static u16 last_x;
static u16 last_y;

void LCD8_DrawPoint_Fast() {
	if (LCD8_y != last_y || LCD8_x != last_x) {
		LCD8_WriteReg(0x201, LCD8_x);
		LCD8_WriteReg(0x200, LCD8_y);
		LCD8_WriteControl(0x202);
	}
		
	LCD8_WriteData(LCD8_color);
	LCD8_x ++;
}


void diag_LCD_signal(void) {
	CURR_CS_MASK = 0x3e3e;
	LCD8_DoutMode();
	while (1) {		
		LCD8_CS_H();
		LCD8_RS_L();
		LCD8_WR_H();
		LCD8_RD_L();
		LCD8_putData(0x55);
		Delay_uS(1);

		LCD8_CS_L();
		LCD8_RS_H();
		LCD8_WR_L();
		LCD8_RD_H();
		LCD8_putData(0xaa);
		Delay_uS(1);
	}
}

void LCD8_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  
	
    u8 temp;
    u8 pos,t;
	u16 x0=x;
	u16 colortemp=POINT_COLOR;      
    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	    
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	if(!mode) //非叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)//POINT_COLOR=colortemp;
				//else POINT_COLOR=BACK_COLOR;
				//LCD_DrawPoint(x,y);
				LCD8_DrawPoint(POINT_COLOR,x,y);	
				temp>>=1; 
				x++;
		    }
			x=x0;
			y++;
		}	
	}else//叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01) LCD8_DrawPoint(POINT_COLOR,x+t,y+pos);//LCD_DrawPoint(x+t,y+pos);//画一个点     
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	    	   	 	  
}   


void LCD8_ShowString(u16 x,u16 y,const u8 *p)
{         
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;LCD8_Clear(WHITE);}
        LCD8_ShowChar(x,y,*p,16,0);
        x+=8;
        p++;
    }  
}

