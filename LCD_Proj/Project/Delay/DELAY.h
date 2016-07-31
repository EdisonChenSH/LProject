
#ifndef __DELAY_H
#define __DELAY_H 
#include "stm32f10x.h"

void Delay_uS(uint16_t nCount);
void Delay_mS(uint16_t nCount);
void Delay_S(uint8_t SYSCLK,uint16_t nCount);
void delayByTIM_Init(uint8_t SYSCLK);

#endif 
