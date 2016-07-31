#ifndef __CAN_H
#define __CAN_H	 
#include "stm32f10x.h"	    
//CAN接收RX0中断使能
#define CAN_RX0_INT_ENABLE	0		//0,不使能;1,使能.
	
#define FALSE 0
#define TRUE  1 

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
u8 Can_Tx_Msg(u32 id,u8 ide,u8 rtr,u8 len,u8 *dat);		//发送数据
u8 Can_Msg_Pend(u8 fifox);								//查询邮箱报文
void Can_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat);//接收数据
u8 Can_Tx_Staus(u8 mbox);  								//返回发送状态
u8 Can_Send_Msg(u8* msg,u8 len);						//发送数据
//u8 Can_Receive_Msg(u8 *buf);							//接收数据
u8 Can_Receive_Msg(u8 *buf,u32 meID);

void Init_WindowCon(void);
#endif




















