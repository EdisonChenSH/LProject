#include "Process.h"
#include "jpegbmp.h"

u8 readBuf[]={"this is a rs485 demo"};
u8 gDeviceAddr=10;
u8 gPicDrawType=PIC_TYPE_BMP;
u8 gDeviceOnOffFlag=1;

FileInfoStruct_Flash flashFileInfo_ID;
FileInfoStruct_Flash flashFileInfo;
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

	flashFileInfo_ID.F_Start = FileInfo_485ID;
	flashFileInfo_ID.F_Size = 1;	
	F_Open_Flash(&flashFileInfo_ID);
	sFLASH_ReadBuffer(&gDeviceAddr,flashFileInfo_ID.F_Start,flashFileInfo_ID.F_Size);
	
	flashFileInfo_ID.F_Start = FileInfo_DrawType;
	flashFileInfo_ID.F_Size = 1;	
	F_Open_Flash(&flashFileInfo_ID);
	sFLASH_ReadBuffer(&gPicDrawType,flashFileInfo_ID.F_Start,flashFileInfo_ID.F_Size);
	
	flashFileInfo_ID.F_Start = FileInfo_OnOff;
	flashFileInfo_ID.F_Size = 1;	
	F_Open_Flash(&flashFileInfo_ID);
	sFLASH_ReadBuffer(&gDeviceOnOffFlag,flashFileInfo_ID.F_Start,flashFileInfo_ID.F_Size);
	
	if(gPicDrawType==PIC_TYPE_BMP)
	{
		if(gDeviceOnOffFlag==1)lcdLedSet(1);
		else lcdLedSet(0);
		flashFileInfo.F_Start = FileInfo_PIC1;
		F_Open_Flash(&flashFileInfo);				
		lcdDrawPicFromFs(flashFileInfo);
	}
	else if(gPicDrawType==PIC_TYPE_JPG)
	{
		if(gDeviceOnOffFlag==1)lcdLedSet(1);
		else lcdLedSet(0);
		flashFileInfo.F_Start = FileInfo_PIC2;
		lcdDrawJPicFromFs(flashFileInfo);
		lcdOpenWindows(0,0,lcdWIDTH,lcdHEIGHT);
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
	FileInfoStruct_Flash flashFileInfo_ID;
	
	flashFileInfo_ID.F_Start = FileInfo_485ID;
	flashFileInfo_ID.F_Size = 1;
	
	jpg_buffer[0]=gDeviceAddr;
	F_Open_Flash(&flashFileInfo_ID);
	jWriteFlashC(&flashFileInfo_ID,flashFileInfo_ID.F_Size);
}
void StoreOnOffFlagToFlash()
{
	FileInfoStruct_Flash flashFileInfo_ID;
	
	flashFileInfo_ID.F_Start = FileInfo_OnOff;
	flashFileInfo_ID.F_Size = 1;
	
	jpg_buffer[0]=gDeviceOnOffFlag;
	F_Open_Flash(&flashFileInfo_ID);
	jWriteFlashC(&flashFileInfo_ID,flashFileInfo_ID.F_Size);
}
#define CMD_GET_ADDR 0
#define CMD_SET_ADDR 1
#define CMD_TRUNON_LCD 2
#define CMD_TRUNOFF_LCD 3
#define CMD_READ_SENOR 4

#define CMD_START_UPDATE_PIC 5
#define CMD_SEND_PIC_DATA 6
#define CMD_FINISH_UPDATE_PIC 7

#define CMD_START_UPDATE_JPGPIC 8
#define CMD_SEND_JPGPIC_DATA 9
#define CMD_FINISH_UPDATE_JPGPIC 10


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
			gDeviceOnOffFlag=1;
      StoreOnOffFlagToFlash();
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
			gDeviceOnOffFlag=0;
      StoreOnOffFlagToFlash();
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

			flashFileInfo.F_Start = PIC1_Info;
			flashFileInfo.F_Size = sizeof(gPicInformation);
			F_Open_Flash(&flashFileInfo);
			memcpy(jpg_buffer,(void*)(&gPicInformation),flashFileInfo.F_Size);
			jWriteFlashC(&flashFileInfo,flashFileInfo.F_Size);

			flashFileInfo.F_Start = FileInfo_PIC1;
			flashFileInfo.F_Size = gPicInformation.PicSize;
			F_Open_Flash(&flashFileInfo);	
			gPkgIndex=0;
			
			gTxBuffer[i]=(u8)0x00;i++;
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
					memcpy(jpg_buffer,(gRxBuffer+3),1024);
					jWriteFlashC(&flashFileInfo,1024);
					flashFileInfo.F_Start+=1024;
					
					gPkgIndex++;
					gFileRxStatus=STATUS_OK;
				}
				else if(gPkgIndex+1==gFileRxPkgCount)
				{
          memcpy(jpg_buffer,(gRxBuffer+3),gFileRxLastPkgLength);
					jWriteFlashC(&flashFileInfo,gFileRxLastPkgLength);
					flashFileInfo.F_Start+=gFileRxLastPkgLength;
					
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

			gTxBuffer[i]=flashFileInfo.F_Start>>24;i++;
			gTxBuffer[i]=flashFileInfo.F_Start>>16;i++;
			gTxBuffer[i]=flashFileInfo.F_Start>>8;i++;
			gTxBuffer[i]=flashFileInfo.F_Start>>0;i++;

			if((flashFileInfo.F_Start-FileInfo_PIC1)==gPicInformation.PicSize)
			{
				isUpdateSuccess=1;
			}
			else
			{

			}
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
			if(isUpdateSuccess)
			{
				gPicDrawType=PIC_TYPE_BMP;
        
				flashFileInfo.F_Start = FileInfo_PIC1;
        F_Open_Flash(&flashFileInfo);				
				lcdDrawPicFromFs(flashFileInfo);
				
				flashFileInfo.F_Start = FileInfo_DrawType;
	      flashFileInfo.F_Size = 1;
	
	      jpg_buffer[0]=gPicDrawType;
	      F_Open_Flash(&flashFileInfo);
	      jWriteFlashC(&flashFileInfo,flashFileInfo.F_Size);
			}
		}
		else if(gRxBuffer[1]==CMD_START_UPDATE_JPGPIC)
		{
			//memcpy((void *)(&gPicInformation),(gRxBuffer+2),sizeof(gPicInformation));
			gPicInformation.PicSize=(gRxBuffer[2]<<24)+(gRxBuffer[3]<<16)+(gRxBuffer[4]<<8)+(gRxBuffer[5]);
			gPicInformation.x=(gRxBuffer[6]<<8)+(gRxBuffer[7]);
			gPicInformation.y=(gRxBuffer[8]<<8)+(gRxBuffer[9]);
			gPicInformation.weith=(gRxBuffer[10]<<8)+(gRxBuffer[11]);
			gPicInformation.height=(gRxBuffer[12]<<8)+(gRxBuffer[13]);

			gTxBuffer[i]=gDeviceAddr;i++;
			gTxBuffer[i]=CMD_START_UPDATE_JPGPIC;i++;

			flashFileInfo.F_Start = PIC2_Info;
			flashFileInfo.F_Size = sizeof(gPicInformation);
			F_Open_Flash(&flashFileInfo);
			memcpy(jpg_buffer,(void*)(&gPicInformation),flashFileInfo.F_Size);
			jWriteFlashC(&flashFileInfo,flashFileInfo.F_Size);

			flashFileInfo.F_Start = FileInfo_PIC2;
			flashFileInfo.F_Size = gPicInformation.PicSize;
			F_Open_Flash(&flashFileInfo);	
			gPkgIndex=0;
			
			gTxBuffer[i]=(u8)0x00;i++;
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
			gFileRxPkgCount=gPicInformation.PicSize/1024;
			gFileRxLastPkgLength=gPicInformation.PicSize%1024;
			if(gFileRxLastPkgLength!=0)gFileRxPkgCount+=1;
			else gFileRxLastPkgLength=1024;
		}
		else if(gRxBuffer[1]==CMD_SEND_JPGPIC_DATA)
		{
			if(gPkgIndex==gRxBuffer[2])
			{
				if(gPkgIndex+1<gFileRxPkgCount)
				{
					memcpy(jpg_buffer,(gRxBuffer+3),1024);
					jWriteFlashC(&flashFileInfo,1024);
					flashFileInfo.F_Start+=1024;
					
					gPkgIndex++;
					gFileRxStatus=STATUS_OK;
				}
				else if(gPkgIndex+1==gFileRxPkgCount)
				{
          memcpy(jpg_buffer,(gRxBuffer+3),gFileRxLastPkgLength);
					jWriteFlashC(&flashFileInfo,gFileRxLastPkgLength);
					flashFileInfo.F_Start+=gFileRxLastPkgLength;
					
					gPkgIndex++;
					gFileRxStatus=STATUS_OK;					
				}
				else
				{
					gFileRxStatus=STATUS_PKG_INDEX_ERROR;
				}
				if(gFileRxStatus==STATUS_OK)
				{
					printf("Rec Pic:%d/%d\n",gPkgIndex,gFileRxPkgCount);
				}
			}
			else
			{
				gFileRxStatus=STATUS_PKG_INDEX_ERROR;
			}
			gTxBuffer[i]=gDeviceAddr;i++;
			gTxBuffer[i]=CMD_SEND_JPGPIC_DATA;i++;			
			gTxBuffer[i]=gFileRxStatus;i++;
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
		}
		else if(gRxBuffer[1]==CMD_FINISH_UPDATE_JPGPIC)
		{
			isUpdateSuccess=0;
			gTxBuffer[i]=gDeviceAddr;i++;
			gTxBuffer[i]=CMD_FINISH_UPDATE_JPGPIC;i++;	

			gTxBuffer[i]=flashFileInfo.F_Start>>24;i++;
			gTxBuffer[i]=flashFileInfo.F_Start>>16;i++;
			gTxBuffer[i]=flashFileInfo.F_Start>>8;i++;
			gTxBuffer[i]=flashFileInfo.F_Start>>0;i++;

			if(flashFileInfo.F_Start-FileInfo_PIC2==gPicInformation.PicSize)
			{
				isUpdateSuccess=1;
			}
			else
			{

			}
			crcValue=Cal_CRC16(gTxBuffer,i);
			gTxBuffer[i]=(u8)(crcValue>>8);i++;
			gTxBuffer[i]=(u8)(crcValue&0x00ff);i++;
			rs485SendByteArray(gTxBuffer,i);
			if(isUpdateSuccess)
			{
				gPicDrawType=PIC_TYPE_JPG;
        flashFileInfo.F_Start = FileInfo_PIC2;
				lcdDrawJPicFromFs(flashFileInfo);
				
				flashFileInfo.F_Start = FileInfo_DrawType;
	      flashFileInfo.F_Size = 1;
	
	      jpg_buffer[0]=gPicDrawType;
	      F_Open_Flash(&flashFileInfo);
	      jWriteFlashC(&flashFileInfo,flashFileInfo.F_Size);
			}
			else
			{
				gPicDrawType=PIC_TYPE_BMP;
        
				flashFileInfo.F_Start = FileInfo_PIC1;
        F_Open_Flash(&flashFileInfo);				
				lcdDrawPicFromFs(flashFileInfo);
				
				flashFileInfo.F_Start = FileInfo_DrawType;
	      flashFileInfo.F_Size = 1;
	
	      jpg_buffer[0]=gPicDrawType;
	      F_Open_Flash(&flashFileInfo);
	      jWriteFlashC(&flashFileInfo,flashFileInfo.F_Size);
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