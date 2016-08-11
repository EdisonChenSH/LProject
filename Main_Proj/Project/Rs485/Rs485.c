#include "Rs485.h"
#include "delay.h"
#include "ff.h"
#include "jpegbmp.h"

#define countof(a)   (sizeof(a) / sizeof(*(a)))
#define TxCounter   (countof(TXBuffer) - 1)
Rs485_Rec Rs485_Rec_Flag;
uint8_t  TXBuffer[1040]="test\r\n";
//uint16_t TxCounter = 10;
uint8_t  receive_data[50];
uint8_t  receive_length=0;
uint8_t  gDeviceCANAddr=10;
uint8_t  gDevice485Addr=10;
uint32_t Flie_size=0;
uint8_t Rs485_Id_Rec=0;
void Init_Rs485(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_Initstructure;
	
	/* GPIOB,USART1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RS485_TX_GPIO_CLK | RS485_RX_GPIO_CLK | \
	                       RS485_CS_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
	RS485_PeriphClockCmd(RS485_USART_CLK,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	//GPIO_PinRemapConfig(RS485_USART_Remap, ENABLE);

  /* RX */
  GPIO_InitStructure.GPIO_Pin = RS485_RX_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(RS485_TX_Port, &GPIO_InitStructure);
	/* TX */
	GPIO_InitStructure.GPIO_Pin = RS485_TX_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(RS485_RX_Port, &GPIO_InitStructure);
	/* CS */
	GPIO_InitStructure.GPIO_Pin = RS485_CS_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RS485_CS_Port, &GPIO_InitStructure);
	GPIO_ResetBits(RS485_CS_Port,RS485_CS_Pin);
	
  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
  /* Enable the USARTy Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RS485_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	USART_DeInit(RS485_USART);

  USART_InitStructure.USART_BaudRate = 19200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	
	USART_Init(RS485_USART, &USART_InitStructure);
	
	//USART_ITConfig(RS485_USART, USART_IT_IDLE, ENABLE);//¿ªÆô´®¿Ú¿ÕÏÐÖÐ¶Ï
	USART_ITConfig(RS485_USART, USART_IT_RXNE, ENABLE);
	//USART_DMACmd(RS485_USART,USART_DMAReq_Rx,ENABLE);
//	USART_DMACmd(RS485_USART,USART_DMAReq_Tx,ENABLE);
	USART_Cmd(RS485_USART, ENABLE);
//	
//	
//	DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&RS485_USART->DR);; 
//	DMA_Initstructure.DMA_MemoryBaseAddr     = (u32)receive_data; 
//	DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralSRC; 
//	DMA_Initstructure.DMA_BufferSize = 100; 
//	DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
//	DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable; 
//	DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
//	DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
//	DMA_Initstructure.DMA_Mode = DMA_Mode_Normal; 
//	DMA_Initstructure.DMA_Priority = DMA_Priority_High; 
//	DMA_Initstructure.DMA_M2M = DMA_M2M_Disable; 
//	DMA_Init(DMA1_Channel3,&DMA_Initstructure);
//	DMA_Cmd(DMA1_Channel3,ENABLE);

//	DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&RS485_USART->DR);; 
//	DMA_Initstructure.DMA_MemoryBaseAddr     = (u32)TXBuffer; 
//	DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralDST; 
//	DMA_Initstructure.DMA_BufferSize = TxCounter; 
//	DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
//	DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable; 
//	DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
//	DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
//	DMA_Initstructure.DMA_Mode = DMA_Mode_Normal; 
//	DMA_Initstructure.DMA_Priority = DMA_Priority_VeryHigh; 
//	DMA_Initstructure.DMA_M2M = DMA_M2M_Disable; 
//	DMA_Init(DMA1_Channel2,&DMA_Initstructure);
//	DMA_Cmd(DMA1_Channel2,ENABLE);
}

void test_485_Trans(void)
{
	uint8_t TxBuffer1[] = "USART Interrupt Example: USARTy . USARTz using Interrupt";
	uint8_t NbrOfData = 20;
  uint8_t TxCounter1 = 0x00;
	GPIO_SetBits(RS485_CS_Port,RS485_CS_Pin);
	Delay_mS(10);
	while(NbrOfData--)
  {
    USART_SendData(RS485_USART, TxBuffer1[TxCounter1++]);
    while(USART_GetFlagStatus(RS485_USART, USART_FLAG_TXE) == RESET);          
  }
	Delay_mS(10);
	GPIO_ResetBits(RS485_CS_Port,RS485_CS_Pin);
}

void rs485SendByteArray(u8* bytesPtr,u16 length)
{
	GPIO_SetBits(RS485_CS_Port,RS485_CS_Pin);
	Delay_mS(10);
	while(length)
	{
		USART_SendData(RS485_USART,*bytesPtr);
		while(USART_GetFlagStatus(RS485_USART, USART_FLAG_TXE) == RESET);
		bytesPtr++;
		length--;
	}
	Delay_mS(10);
	GPIO_ResetBits(RS485_CS_Port,RS485_CS_Pin);
}

u16 UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
    uint32_t crc = crcIn;
    uint32_t in = byte|0x100;
    do
    {
        crc <<= 1;
        in <<= 1;
        if (in&0x100)
            ++crc;
        if (crc&0x10000)
            crc ^= 0x1021;
    }
    while (!(in&0x10000));
    return crc&0xffffu;
}

u16 Cal_CRC16(const uint8_t* data, uint32_t size)
{
    uint32_t crc = 0;
    const uint8_t* dataEnd = data+size;
    while (data<dataEnd)
        crc = UpdateCRC16(crc,*data++);

    crc = UpdateCRC16(crc,0);
    crc = UpdateCRC16(crc,0);
    return crc&0xffffu;
}

void Store485AddrToFlash()
{	
	FileInfoStruct_Flash flashFileInfo_ID;
	
	flashFileInfo_ID.F_Start = FileInfo_485ID;
	flashFileInfo_ID.F_Size = 1;
	
	jpg_buffer[0]=gDevice485Addr;
	F_Open_Flash(&flashFileInfo_ID);
	jWriteFlashC(&flashFileInfo_ID,flashFileInfo_ID.F_Size);
}

void StoreCANAddrToFlash()
{
	FileInfoStruct_Flash flashFileInfo_ID;
	
	flashFileInfo_ID.F_Start = FileInfo_CANID;
	flashFileInfo_ID.F_Size = 1;
	
	jpg_buffer[0]=gDeviceCANAddr;
	F_Open_Flash(&flashFileInfo_ID);
	jWriteFlashC(&flashFileInfo_ID,flashFileInfo_ID.F_Size);
}

uint8_t TRUNONOFF_LCD(uint8_t Rs485_Id,uint8_t OnOffCmd)
{	
  u16 crcValue=0;
	u8 i=0;
	u8 returnglag=0;
	
	Rs485_Id_Rec=Rs485_Id;
	
	TXBuffer[i]=Rs485_Id;i++;
	TXBuffer[i]=OnOffCmd;i++;
	crcValue=Cal_CRC16(TXBuffer,i);
	TXBuffer[i]=(u8)(crcValue>>8);i++;
	TXBuffer[i]=(u8)(crcValue&0x00ff);i++;
	rs485SendByteArray(TXBuffer,i);
	StartRecTimer();
	while(Rs485_Rec_Flag.OnOff_Flag==0 && CmpRecTimer(300))
	{
		Rs485_Process();
	}	
  StopRecTimer();
	returnglag=Rs485_Rec_Flag.OnOff_Flag;
	Rs485_Rec_Flag.OnOff_Flag = 0;
	return returnglag;
}

u8 Read_Ir(uint8_t Rs485_Id)
{
	u16 crcValue=0;
	u8 i=0;
	u8 returnSensor;
	
	Rs485_Id_Rec=Rs485_Id;
	
	TXBuffer[i]=Rs485_Id;i++;
	TXBuffer[i]=CMD_READ_SENOR;i++;
	crcValue=Cal_CRC16(TXBuffer,i);
	TXBuffer[i]=(u8)(crcValue>>8);i++;
	TXBuffer[i]=(u8)(crcValue&0x00ff);i++;
	rs485SendByteArray(TXBuffer,i);
	StartRecTimer();
	while(Rs485_Rec_Flag.Sensor_Flag==0 && CmpRecTimer(300))
	{
		Rs485_Process();
	}	
	if(Rs485_Rec_Flag.Sensor_Flag==0)returnSensor=0xFE;
	else returnSensor=receive_data[2];
  StopRecTimer();
	Rs485_Rec_Flag.Sensor_Flag = 0;
	return returnSensor;
}

void Get_Ir_In(uint8_t Rs485_Id)
{
	u16 crcValue=0;
	u8 i=0;
	TXBuffer[i]=Rs485_Id;i++;
	TXBuffer[i]=CMD_READ_SENOR;i++;
	crcValue=Cal_CRC16(TXBuffer,i);
	TXBuffer[i]=(u8)(crcValue>>8);i++;
	TXBuffer[i]=(u8)(crcValue&0x00ff);i++;
	rs485SendByteArray(TXBuffer,i);
}

void Save_Pic_Size(u32 FileInfo_PIC,PicInfo picinfo)
{
	FileInfoStruct_Flash flashFileInfo_ID;
	
	flashFileInfo_ID.F_Start = FileInfo_PIC;
	flashFileInfo_ID.F_Size = sizeof(picinfo);

	F_Open_Flash(&flashFileInfo_ID);
	memcpy(jpg_buffer,(void*)(&picinfo),flashFileInfo_ID.F_Size);
	jWriteFlashC(&flashFileInfo_ID,flashFileInfo_ID.F_Size);	
}

PicInfo Get_Pic_Size(u32 FileInfo_PIC)
{
	PicInfo picinfo;
	FileInfoStruct_Flash flashFileInfo_ID;
	
	flashFileInfo_ID.F_Start = FileInfo_PIC;
	flashFileInfo_ID.F_Size = sizeof(PicInfo);
	
	F_Open_Flash(&flashFileInfo_ID);
	sFLASH_ReadBuffer((void*)(&picinfo),flashFileInfo_ID.F_Start,flashFileInfo_ID.F_Size);
	return picinfo;
}
void Update_Pic(uint8_t Rs485_Id)
{
	FileInfoStruct_Flash flashFileInfo_Read;
	u32 btr;	
	u16 i=0,j=0;
	u16 readCount;
	u16 sendCount=0;
	u16 lastReadLength;
	u16 crcValue=0;
	u8 retransmissionsnum=0;
	PicInfo picinfo;

	Rs485_Id_Rec=Rs485_Id;
	picinfo=Get_Pic_Size(PIC1_Info);
	readCount=picinfo.PicSize/1024;
	lastReadLength=picinfo.PicSize%1024;
	if(lastReadLength!=0)readCount++;
	else lastReadLength=1024;
	
	flashFileInfo_Read.F_Start = FileInfo_PIC1;
	flashFileInfo_Read.F_Size  = picinfo.PicSize;
	F_Open_Flash(&flashFileInfo_Read);
	
	TXBuffer[sendCount]=Rs485_Id;sendCount++;
	TXBuffer[sendCount]=CMD_START_UPDATE_PIC;sendCount++;
	TXBuffer[sendCount]=picinfo.PicSize>>24;sendCount++;
	TXBuffer[sendCount]=picinfo.PicSize>>16;sendCount++;
	TXBuffer[sendCount]=picinfo.PicSize>>8;sendCount++;
	TXBuffer[sendCount]=picinfo.PicSize;sendCount++;
	TXBuffer[sendCount]=picinfo.x>>8;sendCount++;
	TXBuffer[sendCount]=picinfo.x;sendCount++;
	TXBuffer[sendCount]=picinfo.y>>8;sendCount++;
	TXBuffer[sendCount]=picinfo.y;sendCount++;
	TXBuffer[sendCount]=picinfo.weith>>8;sendCount++;
	TXBuffer[sendCount]=picinfo.weith;sendCount++;
	TXBuffer[sendCount]=picinfo.height>>8;sendCount++;
	TXBuffer[sendCount]=picinfo.height;sendCount++;
	crcValue=Cal_CRC16(TXBuffer,sendCount);
	TXBuffer[sendCount]=(u8)(crcValue>>8);sendCount++;
	TXBuffer[sendCount]=(u8)(crcValue&0x00ff);sendCount++;
	rs485SendByteArray(TXBuffer,sendCount);
	
	StartRecTimer();
	while(Rs485_Rec_Flag.UPDATE_PIC_Flag==0 && CmpRecTimer(300))
	{
		Rs485_Process();
	}	
  StopRecTimer();

	if(Rs485_Rec_Flag.UPDATE_PIC_Flag==0x01)
	{
		Rs485_Rec_Flag.UPDATE_PIC_Flag = 0;
		for(i=0;i<readCount;i++)
		{
			sendCount=0;Rs485_Rec_Flag.SEND_PIC_Flag=0;
			TXBuffer[sendCount]=Rs485_Id;sendCount++;
			TXBuffer[sendCount]=CMD_SEND_PIC_DATA;sendCount++;
			TXBuffer[sendCount]=i;sendCount++;
			if(i!=(readCount-1))
			{
				F_Read_Flash(&flashFileInfo_Read,TXBuffer+3);
				F_Read_Flash(&flashFileInfo_Read,TXBuffer+3+512);
				sendCount=sendCount+1024;
			}
			else
			{
				F_Read_Flash(&flashFileInfo_Read,TXBuffer+3);
				F_Read_Flash(&flashFileInfo_Read,TXBuffer+3+512);
				sendCount=sendCount+1024;		
			}
			crcValue=Cal_CRC16(TXBuffer,sendCount);
			TXBuffer[sendCount]=(u8)(crcValue>>8);sendCount++;
			TXBuffer[sendCount]=(u8)(crcValue&0x00ff);sendCount++;
			rs485SendByteArray(TXBuffer,sendCount);
			
			StartRecTimer();
			while(Rs485_Rec_Flag.SEND_PIC_Flag==0 && CmpRecTimer(1000))
			{
				Rs485_Process();
			}	
			if(Rs485_Rec_Flag.SEND_PIC_Flag==2 || CmpRecTimer(1000)!=1)
			{
				i--;sendCount=sendCount-1024;retransmissionsnum++;
			}
			if(retransmissionsnum>10)break;
			StopRecTimer();
		}
	}
	Rs485_Rec_Flag.SEND_PIC_Flag = 0;
	sendCount=0;
	TXBuffer[sendCount]=Rs485_Id;sendCount++;
	TXBuffer[sendCount]=CMD_FINISH_UPDATE_PIC;sendCount++;
	crcValue=Cal_CRC16(TXBuffer,sendCount);
	TXBuffer[sendCount]=(u8)(crcValue>>8);sendCount++;
	TXBuffer[sendCount]=(u8)(crcValue&0x00ff);sendCount++;
	rs485SendByteArray(TXBuffer,sendCount);
	
	StartRecTimer();
	while(Rs485_Rec_Flag.FINISH_UPDATE_Flag==0 && CmpRecTimer(300))
	{
		Rs485_Process();
	}	
  StopRecTimer();
	Rs485_Rec_Flag.FINISH_UPDATE_Flag = 0;
	if(Flie_size!=picinfo.PicSize)
	{
		
	}
}

void SlaveFunc()
{
	u16 crcValue=0;
	u8 i=0;
	if(receive_data[0]==0)
	{
		if(receive_data[1]==CMD_485GET_ADDR)
		{
			Rs485_LED_Tog();
			TXBuffer[i]=gDevice485Addr;i++;
			TXBuffer[i]=CMD_485GET_ADDR;i++;
			TXBuffer[i]=gDevice485Addr;i++;
			crcValue=Cal_CRC16(TXBuffer,i);
			TXBuffer[i]=(u8)(crcValue>>8);i++;
			TXBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(TXBuffer,i);
		}
		else if(receive_data[1]==CMD_485SET_ADDR)
		{
			if(receive_data[2]!=0)
			{
				Rs485_LED_Tog();
				gDevice485Addr=receive_data[2];
				Store485AddrToFlash();
				TXBuffer[i]=gDevice485Addr;i++;
				TXBuffer[i]=CMD_485SET_ADDR;i++;
				TXBuffer[i]=gDevice485Addr;i++;
				crcValue=Cal_CRC16(TXBuffer,i);
				TXBuffer[i]=(u8)(crcValue>>8);i++;
				TXBuffer[i]=(u8)(crcValue&0x00ff);i++;
				rs485SendByteArray(TXBuffer,i);
			}
		}
		else if(receive_data[1]==CMD_CANGET_ADDR)
		{
			Rs485_LED_Tog();
			TXBuffer[i]=gDeviceCANAddr;i++;
			TXBuffer[i]=CMD_485GET_ADDR;i++;
			TXBuffer[i]=gDeviceCANAddr;i++;
			crcValue=Cal_CRC16(TXBuffer,i);
			TXBuffer[i]=(u8)(crcValue>>8);i++;
			TXBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(TXBuffer,i);
		}
		else if(receive_data[1]==CMD_CANSET_ADDR)
		{
			if(receive_data[2]!=0)
			{
				Rs485_LED_Tog();
				gDeviceCANAddr=receive_data[2];
				StoreCANAddrToFlash();
				TXBuffer[i]=gDeviceCANAddr;i++;
				TXBuffer[i]=CMD_485SET_ADDR;i++;
				TXBuffer[i]=gDeviceCANAddr;i++;
				crcValue=Cal_CRC16(TXBuffer,i);
				TXBuffer[i]=(u8)(crcValue>>8);i++;
				TXBuffer[i]=(u8)(crcValue&0x00ff);i++;
				rs485SendByteArray(receive_data,i);
			}
		}
	}
	else
	{
		if(receive_data[1]==CMD_TRUNON_LCD)
		{
			Rs485_LED_Tog();
      Rs485_Rec_Flag.OnOff_Flag=0x01; 
		}
		else if(receive_data[1]==CMD_TRUNOFF_LCD)
		{
			Rs485_LED_Tog();
      Rs485_Rec_Flag.OnOff_Flag=0x01;
		}
		else if(receive_data[1]==CMD_READ_SENOR)
		{
			Rs485_LED_Tog();
      Rs485_Rec_Flag.Sensor_Flag=0x01;
		}
		else if(receive_data[1]==CMD_START_UPDATE_PIC)
		{
			 Rs485_LED_Tog();
       if(receive_data[2]==0)Rs485_Rec_Flag.UPDATE_PIC_Flag=0x01;
       else Rs485_Rec_Flag.UPDATE_PIC_Flag=0x02;		
		}
		else if(receive_data[1]==CMD_SEND_PIC_DATA)
		{
			 Rs485_LED_Tog();
       if(receive_data[2]==0)Rs485_Rec_Flag.SEND_PIC_Flag=0x01;
       else Rs485_Rec_Flag.SEND_PIC_Flag=0x02;	       
		}
		else if(receive_data[1]==CMD_FINISH_UPDATE_PIC)
		{
			 Rs485_LED_Tog();
       Rs485_Rec_Flag.FINISH_UPDATE_Flag=0x01;
       Flie_size=(receive_data[2]<<24)|(receive_data[3]<<16)|(receive_data[4]<<8)|(receive_data[5]);
		}
	}
}

u32 gSystemTicCounter=0;
u8 gSystemTicFlg=0;
void StartTimer()
{
	gSystemTicCounter=0;
	gSystemTicFlg=1;
}
void ResetTimer()
{
	gSystemTicCounter=0;
}
void StopTimer()
{
	gSystemTicFlg=0;
}
u8 CmpTimer(u16 Tic)
{
	if(Tic<gSystemTicCounter)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

u32 gSystemRecCounter=0;
u32 gSystemWorkLedCounter=0;
u8 gSystemRecFlg=0;
void StartRecTimer()
{
	gSystemRecCounter=0;
	gSystemRecFlg=1;
}
void ResetRecTimer()
{
	gSystemRecCounter=0;
}
void StopRecTimer()
{
	gSystemRecFlg=0;
}
u8 CmpRecTimer(u16 Tic)
{
	if(Tic<gSystemRecCounter)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}



void SysTick_Handler(void)
{
	if(gSystemTicFlg)gSystemTicCounter++;
	if(gSystemRecFlg)gSystemRecCounter++;
	gSystemWorkLedCounter++;
}

#define IDEL 0
#define CHECK_DATA 1
#define TODO 2

u8 gCurrentStatus=IDEL;

void Rs485_Process(void)
{
	u16 crcValue=0;
	switch(gCurrentStatus)
	{
		case IDEL:
		{
			if(CmpTimer(50))
			{
				if(receive_length>=4)
				{
						StopTimer();
						gCurrentStatus=CHECK_DATA;
				}
				else
				{
						StopTimer();
						ResetTimer();
						receive_length=0;
				}
			}
		}break;			
		case CHECK_DATA:
		{
			if(gDevice485Addr==receive_data[0]||receive_data[0]==0||Rs485_Id_Rec==receive_data[0])
			{
				crcValue=receive_data[receive_length-2];
				crcValue<<=8;
				crcValue+=receive_data[receive_length-1];
				if(crcValue==Cal_CRC16(receive_data,receive_length-2))
				{
					gCurrentStatus=TODO;
				}
				else
				{
					gCurrentStatus=IDEL;
					receive_length=0;						
				}
			}
			else
			{
				gCurrentStatus=IDEL;
				receive_length=0;
			}
		}
		break;
		case TODO:
		{
			 SlaveFunc();
			 receive_length=0;
			 gCurrentStatus=IDEL;
		}break;
	}
}

void RS485_IRQHandler(void)
{
//  unsigned char num=0;
//	if(USART_GetITStatus(RS485_USART,USART_IT_IDLE)==SET)
//	{
//		USART_ClearITPendingBit(RS485_USART,USART_IT_IDLE);
//		num=RS485_USART->SR;
//		num=RS485_USART->DR;  
//		receive_length=100-DMA_GetCurrDataCounter(DMA1_Channel3);
//		receive_data[receive_length]='\0';
//		DMA_Cmd(DMA1_Channel3,DISABLE);
//		DMA1_Channel3->CNDTR = 100;
//		DMA_Cmd(DMA1_Channel3,ENABLE);
//		receive_flag = 1;
//	}
	if(USART_GetITStatus(RS485_USART, USART_IT_RXNE) != RESET)
	{
		USART_ClearFlag(RS485_USART, USART_IT_RXNE);
		if(receive_length==0)
		{
			StartTimer();
		}
		else
		{
			ResetTimer();
		}
		if(gCurrentStatus==IDEL)
		{
			receive_data[receive_length]=USART_ReceiveData(RS485_USART);
			receive_length++;
		}
		else
		{
			USART_ReceiveData(RS485_USART);
		}
	}		
}
