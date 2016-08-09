#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "stm32f10x.h"
#include "delay.h"
#include "ff.h"
#include "spi_spiflash.h"
#include "string.h"
#include "stdio.h"

typedef struct{
	uint32_t PicSize;
	uint16_t x;
	uint16_t y;
	uint16_t weith;
	uint16_t height;
} PicInfo;

extern void ProcessInit();
extern void Process();
void GPIO_buffer(void);
void Work_LED_Tog(void);
void Rs485_LED_Tog(void);
void CAN_LED_Tog(void);
#endif