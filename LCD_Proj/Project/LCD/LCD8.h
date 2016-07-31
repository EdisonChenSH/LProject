#ifndef __LCD8_H__
#define __LCD8_H__

#include "stm32f10x.h"

#define LCD_W 240
#define LCD_H 320

void LCD8_Init(void);
void LCD8_init_config(void);
void LCD8_TestPin(void);
void LCD8_Demo(void);
void LCD8_Clear(u16 color);
void LCD8_DrawPoint(u16 color,u16 x, u16 y);

int LCD8_GetCurrCS(void);
void LCD8_SetCS(int i);

extern u16 LCD8_color;
extern u16 LCD8_x;
extern u16 LCD8_y;
void LCD8_DrawPoint_Fast(void);
void diag_LCD_signal(void);
void LCD8_ShowString(u16 x,u16 y,const u8 *p);

void LCD8_GoIdle(void);
void LCD8_LeaveIdle(void);


//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
//#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

extern u8  lcd_i; //��i��LCD��ʾ��
extern u16 BACK_COLOR, POINT_COLOR ;  



#endif
