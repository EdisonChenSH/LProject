#ifndef __SPI_FLASH_UTIL_H__
#define __SPI_FLASH_UTIL_H__

#include "stm32f10x.h"
#include <stdio.h>
#include "spi_spiflash.h"

extern u8 jpg_buffer[1024];//Êý¾Ý»º´æÇø

void dump_SPI_Flash_part(u32 start, u32 length);
void dump_SPI_FLASH(void);
void Flash_Erase_Range(u32 toAddr,u32 size);
unsigned char F_Read_Flash(FileInfoStruct_Flash *f,u8 *buf);
void F_Open_Flash(FileInfoStruct_Flash *f);
void diag_write_counter(u32 start, u32 length, u8 *buf_w);
void jWriteFlashC(FileInfoStruct_Flash *file, u32 len);
#endif
