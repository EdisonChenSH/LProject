#include "stm32f10x.h"
#include "delay.h"   

#define DelayTIM TIM3 //TIM 1 2 3 4 ���á�
#define RCC_DelayTIM RCC_APB1Periph_TIM3
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

void delayByTIM_Init(uint8_t SYSCLK)//SYSCLK=system clock/1M
{
	RCC_APB1PeriphClockCmd(RCC_DelayTIM, ENABLE);//����޸��� ʹ�õ�TIM �˴�ҲӦ������Ӧ�޸�
	
	TIM_TimeBaseStructure.TIM_Period = 1;   //�Զ�װ��
	TIM_TimeBaseStructure.TIM_Prescaler = SYSCLK-1;       //��Ƶ�ʵ�1Mhz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //���¼���
	TIM_TimeBaseInit(DelayTIM, &TIM_TimeBaseStructure);
}
void Delay_uS(uint16_t nCount)//��������65535uS һֱ�ȴ� nCount ��1Mhz�ݼ���0 ���ж�ģʽ
{
	TIM_TimeBaseInit(DelayTIM, &TIM_TimeBaseStructure);
  TIM_SetCounter(DelayTIM, nCount);
  TIM_Cmd(DelayTIM, ENABLE); 
  while((DelayTIM->CNT)>0);
  TIM_Cmd(DelayTIM, DISABLE);
}
void Delay_mS(uint16_t nCount)//��������65mS
{
  nCount*=1000;
	TIM_TimeBaseInit(DelayTIM, &TIM_TimeBaseStructure);
  TIM_SetCounter(DelayTIM, nCount);
  TIM_Cmd(DelayTIM, ENABLE); 
  while((DelayTIM->CNT)>0);
  TIM_Cmd(DelayTIM, DISABLE);
} 
//void Delay_S(uint8_t SYSCLK,uint16_t nCount)//��������65S
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

//  RCC_APB2PeriphClockCmd(RCC_DelayTIM, ENABLE);//����޸��� ʹ�õ�TIM �˴�ҲӦ������Ӧ�޸�
//	
//	TIM_TimeBaseStructure.TIM_Period = 1;              //�Զ�װ��
//	TIM_TimeBaseStructure.TIM_Prescaler = (SYSCLK*1000)-1;       //��Ƶ�ʵ�1Khz
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //���¼���
//	TIM_TimeBaseInit(DelayTIM, &TIM_TimeBaseStructure);
//	
//	nCount*=1000;
//	TIM_SetCounter(DelayTIM, nCount);
//  TIM_Cmd(DelayTIM, ENABLE); 
//  while((DelayTIM->CNT)>0);
//  TIM_Cmd(DelayTIM, DISABLE);
//	
//	TIM_TimeBaseStructure.TIM_Period = 1;                 //�Զ�װ��
//	TIM_TimeBaseStructure.TIM_Prescaler = SYSCLK-1;       //72M��Ƶ�ʵ�1Mhz
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //���¼���
//	TIM_TimeBaseInit(DelayTIM, &TIM_TimeBaseStructure);
//}
