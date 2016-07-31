#include "stm32f10x.h"
#include "delay.h"   

#define DelayTIM TIM3 //TIM 1 2 3 4 可用。
#define RCC_DelayTIM RCC_APB1Periph_TIM3
void delayByTIM_Init(uint8_t SYSCLK)//SYSCLK=system clock/1M
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_DelayTIM, ENABLE);//如果修改了 使用的TIM 此处也应该做相应修改
	
	TIM_TimeBaseStructure.TIM_Period = 1;   //自动装载
	TIM_TimeBaseStructure.TIM_Prescaler = SYSCLK-1;       //分频率到1Mhz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //向下计数
	TIM_TimeBaseInit(DelayTIM, &TIM_TimeBaseStructure);
}
void Delay_uS(uint16_t nCount)//最大可阻塞65535uS 一直等待 nCount 以1Mhz递减到0 非中断模式
{
	delayByTIM_Init(72);
  TIM_SetCounter(DelayTIM, nCount);
  TIM_Cmd(DelayTIM, ENABLE); 
  while((DelayTIM->CNT)>0);
  TIM_Cmd(DelayTIM, DISABLE);
}
void Delay_mS(uint16_t nCount)//最大可阻塞65mS
{
  nCount*=1000;
	delayByTIM_Init(72);
  TIM_SetCounter(DelayTIM, nCount);
  TIM_Cmd(DelayTIM, ENABLE); 
  while((DelayTIM->CNT)>0);
  TIM_Cmd(DelayTIM, DISABLE);
} 
void Delay_S(uint8_t SYSCLK,uint16_t nCount)//最大可阻塞65S
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  RCC_APB2PeriphClockCmd(RCC_DelayTIM, ENABLE);//如果修改了 使用的TIM 此处也应该做相应修改
	
	TIM_TimeBaseStructure.TIM_Period = 1;              //自动装载
	TIM_TimeBaseStructure.TIM_Prescaler = (SYSCLK*1000)-1;       //分频率到1Khz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //向下计数
	TIM_TimeBaseInit(DelayTIM, &TIM_TimeBaseStructure);
	
	nCount*=1000;
	TIM_SetCounter(DelayTIM, nCount);
  TIM_Cmd(DelayTIM, ENABLE); 
  while((DelayTIM->CNT)>0);
  TIM_Cmd(DelayTIM, DISABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 1;                 //自动装载
	TIM_TimeBaseStructure.TIM_Prescaler = SYSCLK-1;       //72M分频率到1Mhz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //向下计数
	TIM_TimeBaseInit(DelayTIM, &TIM_TimeBaseStructure);
}
