#include "Process.h"
#include "Rs485.h"
#include "CAN.h"
#include "jpegbmp.h"

FileInfoStruct_Flash flashFileInfo_ID;

void GPIO_buffer(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	GPIO_SetBits(GPIOB,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);
}

void ProcessInit()
{
	delayByTIM_Init(72);
	SysTick_Config(SystemCoreClock/1000);
	Init_Rs485();
	SPI_Config();
  GPIO_buffer();
	CAN_Mode_Init(1,7,10,10,0);
	
	flashFileInfo_ID.F_Start = FileInfo_485ID;
	flashFileInfo_ID.F_Size = 1;	
	F_Open_Flash(&flashFileInfo_ID);
	sFLASH_ReadBuffer(&gDevice485Addr,flashFileInfo_ID.F_Start,flashFileInfo_ID.F_Size);
	
	flashFileInfo_ID.F_Start = FileInfo_CANID;
	flashFileInfo_ID.F_Size = 1;	
	F_Open_Flash(&flashFileInfo_ID);
	sFLASH_ReadBuffer(&gDeviceCANAddr,flashFileInfo_ID.F_Start,flashFileInfo_ID.F_Size);
}
