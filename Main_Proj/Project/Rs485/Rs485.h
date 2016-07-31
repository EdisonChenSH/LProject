#ifndef _Rs485_H_
#define _Rs485_H_
#include "stm32f10x.h"
#include "stdint.h"
#include <string.h>
#include "Process.h"

typedef struct{
	uint8_t OnOff_Flag;
	uint8_t Sensor_Flag;
	uint8_t UPDATE_PIC_Flag;
	uint8_t SEND_PIC_Flag;
	uint8_t FINISH_UPDATE_Flag;
} Rs485_Rec;

#define RS485_USART             USART3
#define RS485_USART_IRQn        USART3_IRQn
#define RS485_USART_Remap       GPIO_FullRemap_USART3
#define RS485_USART_CLK         RCC_APB1Periph_USART3
#define RS485_PeriphClockCmd    RCC_APB1PeriphClockCmd    
#define RS485_IRQHandler        USART3_IRQHandler

#define RS485_TX_GPIO_CLK       RCC_APB2Periph_GPIOB
#define RS485_TX_Pin            GPIO_Pin_10
#define RS485_TX_Port           GPIOB

#define RS485_RX_GPIO_CLK       RCC_APB2Periph_GPIOB
#define RS485_RX_Pin            GPIO_Pin_11
#define RS485_RX_Port           GPIOB

#define RS485_CS_GPIO_CLK       RCC_APB2Periph_GPIOB
#define RS485_CS_Pin            GPIO_Pin_1
#define RS485_CS_Port           GPIOB

#define CMD_CANGET_ADDR 0x10
#define CMD_CANSET_ADDR 0x11
#define CMD_485GET_ADDR 0
#define CMD_485SET_ADDR 1
#define CMD_TRUNON_LCD 2
#define CMD_TRUNOFF_LCD 3
#define CMD_READ_SENOR 4
#define CMD_START_UPDATE_PIC 8
#define CMD_SEND_PIC_DATA 9
#define CMD_FINISH_UPDATE_PIC 10

#define STATUS_OK 0
#define STATUS_PKG_INDEX_ERROR 1

extern uint8_t  gDeviceCANAddr;
extern uint8_t  gDevice485Addr;

void test_485_Trans(void);
void Init_Rs485(void);
void Rs485_Process(void);
void TRUNONOFF_LCD(uint8_t Rs485_Id,uint8_t OnOffCmd);
void Save_Pic_Size(u32 FileInfo_PIC,PicInfo picinfo);
PicInfo Get_Pic_Size(u32 FileInfo_PIC);
void Update_Pic(uint8_t Rs485_Id);
u8 Read_Ir(uint8_t Rs485_Id);

void StartRecTimer();
void ResetRecTimer();
void StopRecTimer();
u8 CmpRecTimer(u16 Tic);

#endif
