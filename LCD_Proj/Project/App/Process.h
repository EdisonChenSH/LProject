#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "stm32f10x.h"
#include "LCD.h"
#include "delay.h"
#include "ff.h"
#include "spi_spiflash.h"
#include "string.h"
#include "stdio.h"
#include "Rs485.h"

typedef struct 
{
	u32 PicSize;
	u16 x;
	u16 y;
	u16 weith;
	u16 height;
}PicInfo;

extern void ProcessInit();
extern void Process();
#endif