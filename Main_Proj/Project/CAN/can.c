#include "can.h"
#include "delay.h"

u32 sdcanID = 0xf88;	
static CanTxMsg TxMessage;
CanRxMsg RxMessage;
uint8_t KeyNumber;
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:1~3;
//tbs2:时间段2的时间单元.范围:1~8;
//tbs1:时间段1的时间单元.范围:1~16;
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//注意以上参数任何一个都不能设为0,否则会乱.
//波特率=Fpclk1/((tbs1+tbs2+1)*brp);
//mode:0,普通模式;1,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Normal_Init(1,8,7,5,1);
//则波特率为:36M/((8+7+1)*5)=450Kbps
//返回值:0,初始化OK;
//其他,初始化失败;

CanTxMsg TxMessage1;
CanRxMsg RxMessage2;
uint32_t i = 0;
uint8_t TransmitMailbox = 0;
void Init_WindowCon(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
	CAN_InitTypeDef   CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/* GPIOB,CAN Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/* PB9-->TX  PB8-->RX */	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinRemapConfig(GPIO_Remap1_CAN1 , ENABLE);
  
  /* CAN register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  
  /* CAN Baudrate = 50KBps*/
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
  CAN_InitStructure.CAN_Prescaler = 80;
  CAN_Init(CAN1, &CAN_InitStructure);
	
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

void Init_RxMes(CanRxMsg *RxMessage)
{
  uint8_t i = 0;

  RxMessage->StdId = 0x320;
  RxMessage->ExtId = 0x01;
  RxMessage->IDE = CAN_ID_STD;
	RxMessage->RTR = CAN_RTR_DATA;
  RxMessage->DLC = 1;
  RxMessage->FMI = 0;
	
  for (i = 0;i < 8;i++)
  {
    RxMessage->Data[i] = 0x00;
  }
}

/*
Parameter:ID DEVICES id NO.
data:transmit data array
num:transmit data length 0~8;
*/
void CAN_Trans_Data(uint16_t ID,uint8_t *data,uint8_t num)
{
	uint8_t i;
	TxMessage.StdId = 0;
	TxMessage.DLC = num;
	for (i = 0;i < num;i++)
  {
    TxMessage.Data[i] = data[i];
  }
	TxMessage.IDE = CAN_ID_EXT;
	TxMessage.ExtId = ID;
	TxMessage.RTR = CAN_RTR_DATA;
	CAN_Transmit(CAN1, &TxMessage);
}

void test_can_Trans(void)
{
	if(KeyNumber == 0x4) 
	{
		KeyNumber = 0x00;
	}
	else
	{
		TxMessage.Data[0] = KeyNumber;
		CAN_Trans_Data(0x321,&KeyNumber,1);
		Delay_mS(50);
	}
	KeyNumber++;
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_15);
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
  if ((RxMessage.StdId == 0x101)&&(RxMessage.IDE == CAN_ID_STD) && (RxMessage.DLC == 8))
  {
    //Window_commcommand=RxMessage.Data[1];
  }
}

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
	CAN_InitTypeDef   CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	u16 i=0;
	
	
	/* GPIOB,CAN Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/* PB9-->TX  PB8-->RX */	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinRemapConfig(GPIO_Remap1_CAN1 , ENABLE);
	
 	if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
	tsjw-=1;//先减去1.再用于设置
	tbs2-=1;
	tbs1-=1;
	brp-=1;

	CAN1->MCR=0x0000;	//退出睡眠模式(同时设置所有位为0)
	CAN1->MCR|=1<<0;		//请求CAN进入初始化模式
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100)return 2;//进入初始化模式失败
	}
	CAN1->MCR|=0<<7;		//非时间触发通信模式
	CAN1->MCR|=0<<6;		//软件自动离线管理
	CAN1->MCR|=0<<5;		//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN1->MCR|=1<<4;		//禁止报文自动传送
	CAN1->MCR|=0<<3;		//报文不锁定,新的覆盖旧的
	CAN1->MCR|=0<<2;		//优先级由报文标识符决定
	CAN1->BTR=0x00000000;//清除原来的设置.
	CAN1->BTR|=mode<<30;	//模式设置 0,普通模式;1,回环模式;
	CAN1->BTR|=tsjw<<24; //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
	CAN1->BTR|=tbs2<<20; //Tbs2=tbs2+1个时间单位
	CAN1->BTR|=tbs1<<16;	//Tbs1=tbs1+1个时间单位
	CAN1->BTR|=brp<<0;  	//分频系数(Fdiv)为brp+1
						//波特率:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)
	CAN1->MCR&=~(1<<0);	//请求CAN退出初始化模式
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0XFFF0)return 3;//退出初始化模式失败
	}
	//过滤器初始化
	CAN1->FMR|=1<<0;			//过滤器组工作在初始化模式
	CAN1->FA1R&=~(1<<0);		//过滤器0不激活
	CAN1->FS1R|=1<<0; 		//过滤器位宽为32位.
	CAN1->FM1R|=0<<0;		//过滤器0工作在标识符屏蔽位模式
	CAN1->FFA1R|=0<<0;		//过滤器0关联到FIFO0
	CAN1->sFilterRegister[0].FR1=0X00000000;//32位ID
	CAN1->sFilterRegister[0].FR2=0X00000000;//32位MASK
	CAN1->FA1R|=1<<0;		//激活过滤器0
	CAN1->FMR&=0<<0;			//过滤器组进入正常模式

#if CAN_RX0_INT_ENABLE
 	//使用中断接收
	CAN->IER|=1<<1;			//FIFO0消息挂号中断允许.	    
	MY_NVIC_Init(1,0,USB_LP_CAN_RX0_IRQChannel,2);//组2
#endif
	return 0;
}
//id:标准ID(11位)/扩展ID(11位+18位)	    
//ide:0,标准帧;1,扩展帧
//rtr:0,数据帧;1,远程帧
//len:要发送的数据长度(固定为8个字节,在时间触发模式下,有效数据为6个字节)
//*dat:数据指针.
//返回值:0~3,邮箱编号.0XFF,无有效邮箱.
u8 Can_Tx_Msg(u32 id,u8 ide,u8 rtr,u8 len,u8 *dat)
{   
	u8 mbox;	  
	if(CAN1->TSR&(1<<26))mbox=0;			//邮箱0为空
	else if(CAN1->TSR&(1<<27))mbox=1;	//邮箱1为空
	else if(CAN1->TSR&(1<<28))mbox=2;	//邮箱2为空
	else return 0XFF;					//无空邮箱,无法发送 
	CAN1->sTxMailBox[mbox].TIR=0;		//清除之前的设置
	if(ide==0)	//标准帧
	{
		id&=0x7ff;//取低11位stdid
		id<<=21;		  
	}else		//扩展帧
	{
		id&=0X1FFFFFFF;//取低32位extid
		id<<=3;									   
	}
	CAN1->sTxMailBox[mbox].TIR|=id;		 
	CAN1->sTxMailBox[mbox].TIR|=ide<<2;	  
	CAN1->sTxMailBox[mbox].TIR|=rtr<<1;
	len&=0X0F;//得到低四位
	CAN1->sTxMailBox[mbox].TDTR&=~(0X0000000F);
	CAN1->sTxMailBox[mbox].TDTR|=len;		   //设置DLC.
	//待发送数据存入邮箱.
	CAN1->sTxMailBox[mbox].TDHR=(((u32)dat[7]<<24)|
								((u32)dat[6]<<16)|
 								((u32)dat[5]<<8)|
								((u32)dat[4]));
	CAN1->sTxMailBox[mbox].TDLR=(((u32)dat[3]<<24)|
								((u32)dat[2]<<16)|
 								((u32)dat[1]<<8)|
								((u32)dat[0]));
	CAN1->sTxMailBox[mbox].TIR|=1<<0; //请求发送邮箱数据
	return mbox;
}
//获得发送状态.
//mbox:邮箱编号;
//返回值:发送状态. 0,挂起;0X05,发送失败;0X07,发送成功.
u8 Can_Tx_Staus(u8 mbox)
{	
	u8 sta=0;					    
	switch (mbox)
	{
		case 0: 
			sta |= CAN1->TSR&(1<<0);			//RQCP0
			sta |= CAN1->TSR&(1<<1);			//TXOK0
			sta |=((CAN1->TSR&(1<<26))>>24);	//TME0
			break;
		case 1: 
			sta |= CAN1->TSR&(1<<8)>>8;		//RQCP1
			sta |= CAN1->TSR&(1<<9)>>8;		//TXOK1
			sta |=((CAN1->TSR&(1<<27))>>25);	//TME1	   
			break;
		case 2: 
			sta |= CAN1->TSR&(1<<16)>>16;	//RQCP2
			sta |= CAN1->TSR&(1<<17)>>16;	//TXOK2
			sta |=((CAN1->TSR&(1<<28))>>26);	//TME2
			break;
		default:
			sta=0X05;//邮箱号不对,肯定失败.
		break;
	}
	return sta;
} 
//得到在FIFO0/FIFO1中接收到的报文个数.
//fifox:0/1.FIFO编号;
//返回值:FIFO0/FIFO1中的报文个数.
u8 Can_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN1->RF0R&0x03; 
	else if(fifox==1)return CAN1->RF1R&0x03; 
	else return 0;
}
//接收数据
//fifox:邮箱号
//id:标准ID(11位)/扩展ID(11位+18位)	    
//ide:0,标准帧;1,扩展帧
//rtr:0,数据帧;1,远程帧
//len:接收到的数据长度(固定为8个字节,在时间触发模式下,有效数据为6个字节)
//dat:数据缓存区
void Can_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat)
{	   
	*ide=CAN1->sFIFOMailBox[fifox].RIR&0x04;//得到标识符选择位的值  
 	if(*ide==0)//标准标识符
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>21;
	}else	   //扩展标识符
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>3;
	}
	*rtr=CAN1->sFIFOMailBox[fifox].RIR&0x02;	//得到远程发送请求值.
	*len=CAN1->sFIFOMailBox[fifox].RDTR&0x0F;//得到DLC
 	//*fmi=(CAN->sFIFOMailBox[FIFONumber].RDTR>>8)&0xFF;//得到FMI
	//接收数据
	dat[0]=CAN1->sFIFOMailBox[fifox].RDLR&0XFF;
	dat[1]=(CAN1->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	dat[2]=(CAN1->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	dat[3]=(CAN1->sFIFOMailBox[fifox].RDLR>>24)&0XFF;    
	dat[4]=CAN1->sFIFOMailBox[fifox].RDHR&0XFF;
	dat[5]=(CAN1->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	dat[6]=(CAN1->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	dat[7]=(CAN1->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN1->RF0R|=0X20;//释放FIFO0邮箱
	else if(fifox==1)CAN1->RF1R|=0X20;//释放FIFO1邮箱	 
}

#if CAN_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	u8 rxbuf[8];
	u32 id;
	u8 ide,rtr,len;     
 	Can_Rx_Msg(0,&id,&ide,&rtr,&len,rxbuf);
}		
#endif

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//    其他,失败;
u8 Can_Send_Msg(u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	mbox=Can_Tx_Msg(sdcanID,0,0,len,msg);
	while((Can_Tx_Staus(mbox)!=0X07)&&(i<0XFFF))i++;//等待发送结束
	if(i>=0XFFF)return 1;							//发送失败?
	return 0;										//发送成功;
}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//其他,接收的数据长度;
u8 Can_Receive_Msg(u8 *buf,u32 meID)
{		   		   
	u32 id;
	u8 ide,rtr,len; 
	if(Can_Msg_Pend(0)==0)return 0;			//没有接收到数据,直接退出 
		 
  Can_Rx_Msg(0,&id,&ide,&rtr,&len,buf); 	//读取数据

  if(id!=meID||ide!=0||rtr!=0)len=0;		//接收错误	   

	return len;	
}














