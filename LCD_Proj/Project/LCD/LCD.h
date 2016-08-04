#ifndef _LCD_H_
#define _LCD_H_

#include "stm32f10x.h"
#include "jpegbmp.h"

#define lcdWIDTH  ( 400 )
#define lcdHEIGHT ( 240 )

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

extern void lcdAllInit();
extern void lcdLedSet(u8 isTurnOn);
extern u8 readPhotoSenor();
extern void lcdDrawPicFromFs(FileInfoStruct_Flash FileInfo);
extern void lcdDrawJPicFromFs(FileInfoStruct_Flash FileInfo);

void lcdWriteReg(u16 reg);
void lcdWriteData(u16 data);
void lcdSetRamAddr(u16 x,u16 y);
void lcdOpenWindows(u16 x,u16 y,u16 width,u16 height);

#endif

