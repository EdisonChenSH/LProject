#include "stm32f10x.h"
#include <stdio.h>
#include "SPI_flash_util.h"


extern u8 jpg_buffer[1024];//数据缓存区

void dump_SPI_Flash_part(u32 start, u32 length) {
	int i;
	u8 buf[256]; // 256 Bytes
	length = start + length; // end
	
	while (start<length) {
		sFLASH_ReadBuffer(buf,start,256);
		
		printf("%06x : ",start);
		
		i=0;
		while (i<256) {
			printf("%02x ",buf[i++]);
			printf("%02x ",buf[i++]);
		}
		printf("\n");
		
		start+=256;
	}
}

void dump_SPI_FLASH(void) {	
	//dump_SPI_Flash_part(0x1000,96524);
	dump_SPI_Flash_part(0x1000+96524,100);
}

void Flash_Erase_Range(u32 toAddr,u32 size) {
	u16 startAddr = toAddr >> 12;
	u16 endAddr = (toAddr+size) >> 12;
	
	u16 i = startAddr;
	while (i <= endAddr) {
		sFLASH_EraseSector(i);
		i++;
	}
}


void diag_write_counter(u32 start, u32 length, u8 *buf_w) 
{
	int i = 0;
	
	while (i<length) {
		SPI_Flash_Write(buf_w+i,start,256);
		start += 256;
		i+= 256;
	}
}

//读取512个字节
//FileInfo:要读取的文件
//buf     :数据缓存区
//返回值  :0,操作失败,1,操作成功
unsigned char F_Read_Flash(FileInfoStruct_Flash *f,u8 *buf)
{
	if (f->F_Offset > f->F_Size) 
	{
		return 0;
	}
	sFLASH_ReadBuffer(buf, f->F_Start+f->F_Offset,512);
	f->F_Offset += 512;
	return 1;
}


void F_Open_Flash(FileInfoStruct_Flash *f)
{
	f->F_Offset=0;
}

void jWriteFlashC(FileInfoStruct_Flash *file, u32 len)
{
   diag_write_counter(file->F_Start,len,jpg_buffer);
}

