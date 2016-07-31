#include "Process.h"

FATFS gFatfs;
FIL gFile;
FRESULT gRes;
u32 gByte2Write,gByte2Read;
u8 readBuf[]={"this is a rs485 demo"};
u8 gDeviceAddr=10;
void ProcessInit()
{
	delayByTIM_Init(72);
	lcdAllInit();
	printf("lcdAllInit() OK!\n");
	SysTick_Config(SystemCoreClock/1000);
	printf("SysTick_Config(SystemCoreClock/1000) OK!\n");
	Init_Rs485();
	printf("Init_Rs485() OK!\n");
	SPI_Config();
	printf("SPI_Config() OK!\n");
	gRes = f_mount(0,&gFatfs);
	gRes=f_open(&gFile,"0:485ADDr.adr",FA_READ);
	gRes=f_close(&gFile);
	if(gRes==FR_OK)
	{
		printf("f_mount(0,&gFatfs) OK!\n");
	}
	else if(gRes==FR_INVALID_OBJECT)
	{
		printf("f_mount(0,&gFatfs) Fail!\n");
		gRes = f_mkfs(0,1,4096);
		if(gRes==FR_OK)
		{
			printf("f_mkfs(0,1,4096) OK!\n");
		}
		else
		{
			printf("f_mkfs(0,1,4096) Fail!\n");
			printf("system boot Fail!\n");
			while(1);
		}
	}
	else
	{
		printf("f_mount(0,&gFatfs) Exciption!\n");
		printf("system boot Fail!\n");
		while(1);
	}	
	gRes=f_open(&gFile,"0:485ADDr.adr",FA_READ);
	if(gRes==FR_NO_FILE)
	{
		gRes=f_open(&gFile,"0:485ADDr.adr",FA_CREATE_NEW|FA_WRITE);
		gRes=f_write(&gFile,&gDeviceAddr,1,&gByte2Write);
		gRes=f_close(&gFile);
	}
	else if(gRes==FR_OK)
	{
		gRes=f_read(&gFile,&gDeviceAddr,1,&gByte2Write);
		gRes=f_close(&gFile);
	}
	else
	{
		printf("open files Fail!\n");
		printf("system boot Fail!\n");		
		while(1);
	}
	gRes=f_open(&gFile,"0:ready.pic",FA_READ);
	if(gRes==FR_NO_FILE)
	{
		printf("Ready Pic File Not Found!\n");
	}
	else if(gRes==FR_OK)
	{
		gRes=f_close(&gFile);
		lcdDrawPicFromFs("0:ready.pic");
	}
	else
	{
		printf("open files Fail!\n");
		printf("system boot Fail!\n");		
		while(1);
	}
	//lcdDrawPicFromFs("0:temp.pic");
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



#define MAX_RX_BUFFER_LENGTH 1300
#define MAX_TX_BUFFER_LENGTH 10
u8 gRxBuffer[MAX_RX_BUFFER_LENGTH];
u8 gTxBuffer[MAX_TX_BUFFER_LENGTH];
u16 gRxBufferIndex=0;
u8 gIsRxCp=0;


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
void StoreAddrToFlash()
{
	gRes=f_open(&gFile,"0:485ADDr.adr",FA_WRITE);
	gRes=f_write(&gFile,&gDeviceAddr,1,&gByte2Write);
	gRes=f_close(&gFile);
}
#define CMD_GET_ADDR 0
#define CMD_SET_ADDR 1
#define CMD_TRUNON_LCD 2
#define CMD_TRUNOFF_LCD 3
#define CMD_READ_SENOR 4
#define CMD_START_UPDATE_PIC 5
#define CMD_SEND_PIC_DATA 6
#define CMD_FINISH_UPDATE_PIC 7

#define STATUS_OK 0
#define STATUS_PKG_INDEX_ERROR 1

PicInfo gPicInformation;
u8 gPkgIndex=0;
u8 gFileRxStatus=0;
u8 gFileRxPkgCount=0;
u16 gFileRxLastPkgLength=0;
u8 isUpdateSuccess=0;
void SlaveFunc()
{
	u16 crcValue=0;
	u8 i=0;
	if(gRxBuffer[0]==0)
	{
		if(gRxBuffer[1]==CMD_GET_ADDR)
		{
			gTxBuffer[i]=gDeviceAddr;i++;
			gTxBuffer[i]=CMD_GET_ADDR;i++;
			gTxBuffer[i]=gDeviceAddr;i++;
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
		}
		else if(gRxBuffer[1]==CMD_SET_ADDR)
		{
			if(gRxBuffer[2]!=0)
			{
				gDeviceAddr=gRxBuffer[2];
				StoreAddrToFlash();
				
				gTxBuffer[i]=gDeviceAddr;i++;
				gTxBuffer[i]=CMD_SET_ADDR;i++;
				gTxBuffer[i]=gDeviceAddr;i++;
				crcValue=Cal_CRC16(gTxBuffer,i);
				gTxBuffer[i]=(u8)(crcValue>>8);i++;
				gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
				rs485SendByteArray(gTxBuffer,i);
			}

		}
	}
	else
	{
		if(gRxBuffer[1]==CMD_TRUNON_LCD)
		{
			lcdLedSet(1);

			gTxBuffer[i]=gDeviceAddr;i++;
			gTxBuffer[i]=CMD_TRUNON_LCD;i++;
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
		}
		else if(gRxBuffer[1]==CMD_TRUNOFF_LCD)
		{
			lcdLedSet(0);

			gTxBuffer[i]=gDeviceAddr;i++;
			gTxBuffer[i]=CMD_TRUNOFF_LCD;i++;
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
		}
		else if(gRxBuffer[1]==CMD_READ_SENOR)
		{
			gTxBuffer[i]=gDeviceAddr;i++;
			gTxBuffer[i]=CMD_READ_SENOR;i++;
			gTxBuffer[i]=readPhotoSenor();i++;
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
			
		}
		else if(gRxBuffer[1]==CMD_START_UPDATE_PIC)
		{
			//memcpy((void *)(&gPicInformation),(gRxBuffer+2),sizeof(gPicInformation));
			gPicInformation.PicSize=(gRxBuffer[2]<<24)+(gRxBuffer[3]<<16)+(gRxBuffer[4]<<8)+(gRxBuffer[5]);
			gPicInformation.x=(gRxBuffer[6]<<8)+(gRxBuffer[7]);
			gPicInformation.y=(gRxBuffer[8]<<8)+(gRxBuffer[9]);
			gPicInformation.weith=(gRxBuffer[10]<<8)+(gRxBuffer[11]);
			gPicInformation.height=(gRxBuffer[12]<<8)+(gRxBuffer[13]);

			gTxBuffer[i]=gDeviceAddr;i++;
			gTxBuffer[i]=CMD_START_UPDATE_PIC;i++;

			gRes=f_open(&gFile,"0:temp.pic",FA_CREATE_ALWAYS|FA_WRITE);	
			gRes=f_write(&gFile,(void *)(&gPicInformation),sizeof(gPicInformation),&gByte2Write);
			gRes=f_close(&gFile);
			gPkgIndex=0;
			
			gTxBuffer[i]=(u8)gRes;i++;
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
			gFileRxPkgCount=gPicInformation.PicSize/1024;
			gFileRxLastPkgLength=gPicInformation.PicSize%1024;
			if(gFileRxLastPkgLength!=0)gFileRxPkgCount+=1;
			else gFileRxLastPkgLength=1024;
			
		}
		else if(gRxBuffer[1]==CMD_SEND_PIC_DATA)
		{
			if(gPkgIndex==gRxBuffer[2])
			{
				if(gPkgIndex+1<gFileRxPkgCount)
				{
					gRes=f_open(&gFile,"0:temp.pic",FA_WRITE);	
					gRes=f_lseek(&gFile,gFile.fsize);
					gRes=f_write(&gFile,(gRxBuffer+3),1024,&gByte2Write);
					gRes=f_close(&gFile);
					gPkgIndex++;
					gFileRxStatus=STATUS_OK;
				}
				else if(gPkgIndex+1==gFileRxPkgCount)
				{
					gRes=f_open(&gFile,"0:temp.pic",FA_WRITE);	
					gRes=f_lseek(&gFile,gFile.fsize);
					gRes=f_write(&gFile,(gRxBuffer+3),gFileRxLastPkgLength,&gByte2Write);
					gRes=f_close(&gFile);
					gPkgIndex++;
					gFileRxStatus=STATUS_OK;					
				}
				else
				{
					gFileRxStatus=STATUS_PKG_INDEX_ERROR;
				}
			}
			else
			{
				gFileRxStatus=STATUS_PKG_INDEX_ERROR;
			}
			gTxBuffer[i]=gDeviceAddr;i++;
			gTxBuffer[i]=CMD_SEND_PIC_DATA;i++;			
			gTxBuffer[i]=gFileRxStatus;i++;
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
		}
		else if(gRxBuffer[1]==CMD_FINISH_UPDATE_PIC)
		{
			isUpdateSuccess=0;
			gTxBuffer[i]=gDeviceAddr;i++;
			gTxBuffer[i]=CMD_FINISH_UPDATE_PIC;i++;	

			
			gRes=f_open(&gFile,"0:temp.pic",FA_READ);	
			if(gRes==FR_OK)
			{
				gTxBuffer[i]=gFile.fsize>>24;i++;
				gTxBuffer[i]=gFile.fsize>>16;i++;
				gTxBuffer[i]=gFile.fsize>>8;i++;
				gTxBuffer[i]=gFile.fsize>>0;i++;
				gRes=f_close(&gFile);
				if(gFile.fsize-sizeof(gPicInformation)==gPicInformation.PicSize)
				{
					gRes=f_unlink("0:ready.pic");
					gRes=f_rename("0:temp.pic","ready.pic");
					isUpdateSuccess=1;
				}
				else
				{

				}

			}
			else
			{
				gTxBuffer[i]=0;i++;
				gTxBuffer[i]=0;i++;
				gTxBuffer[i]=0;i++;
				gTxBuffer[i]=0;i++;
			}
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
			if(isUpdateSuccess)
			{
				lcdDrawPicFromFs("0:ready.pic");
			}
		}
	}
}



#define IDEL 0
#define CHECK_DATA 1
#define TODO 2

u8 gCurrentStatus=IDEL;
void Process()
{
	u16 crcValue=0;
	while(1)
	{
		switch(gCurrentStatus)
		{
			case IDEL:
			{
				if(CmpTimer(50))
				{
					if(gRxBufferIndex>=4)
					{
							StopTimer();
							gCurrentStatus=CHECK_DATA;
					}
					else
					{
							StopTimer();
							ResetTimer();
							gRxBufferIndex=0;
					}
//					rs485SetRxInterrupt(DISABLE);
				}
			}break;
			case CHECK_DATA:
			{
				if(gDeviceAddr==gRxBuffer[0]||gRxBuffer[0]==0)
				{
					crcValue=gRxBuffer[gRxBufferIndex-2];
					crcValue<<=8;
					crcValue+=gRxBuffer[gRxBufferIndex-1];
					if(crcValue==Cal_CRC16(gRxBuffer,gRxBufferIndex-2))
					{
						gCurrentStatus=TODO;
					}
					else
					{
						gCurrentStatus=IDEL;
						gRxBufferIndex=0;						
					}
				}
				else
				{
					gCurrentStatus=IDEL;
					gRxBufferIndex=0;
				}

			}
			break;
			case TODO:
			{
				 SlaveFunc();
				 gRxBufferIndex=0;
				 gCurrentStatus=IDEL;
				 ResetTimer();
			}break;
		}
	}
}



u8 gCh=0;
void USART1_IRQHandler(void)
{
	//USART_ClearFlag(USART1, USART_IT_RXNE);
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		if(gRxBufferIndex==0)
		{
			StartTimer();
		}
		else
		{
			ResetTimer();
		}
		if(gCurrentStatus==IDEL)
		{
			gRxBuffer[gRxBufferIndex]=USART_ReceiveData(USART1);
			gRxBufferIndex++;
		}
		else
		{
			USART_ReceiveData(USART1);
		}
		
		//printf("%c",gCh);
	}	
}


void SysTick_Handler(void)
{
	if(gSystemTicFlg)gSystemTicCounter++;
}