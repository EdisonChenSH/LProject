#ifndef _SPIFLASH_H_
#define _SPIFLASH_H_

#include "stm32f10x.h"

typedef struct{
	uint32_t F_Start;
	uint32_t F_Size;
	uint32_t F_Offset;
} FileInfoStruct_Flash;

#define FileInfo_DrawType   0x0000
#define FileInfo_485ID      0x1000

#define PIC1_Info        0x10000
#define FileInfo_PIC1    0x11000

#define PIC2_Info        0x40000
#define FileInfo_PIC2    0x41000

#define sFLASH_CMD_WRITE          0x02  /*!< Write to Memory instruction */
#define sFLASH_CMD_WRSR           0x01  /*!< Write Status Register instruction */
#define sFLASH_CMD_WREN           0x06  /*!< Write enable instruction */
#define sFLASH_CMD_READ           0x03  /*!< Read from Memory instruction */
#define sFLASH_CMD_RDSR           0x05  /*!< Read Status Register instruction  */
#define sFLASH_CMD_RDID           0x9F  /*!< Read identification */
#define sFLASH_CMD_SE             0x20  /*!< Sector Erase instruction */
#define sFLASH_CMD_BE             0xC7  /*!< Bulk Erase instruction */
#define sFLASH_CMD_PD             0xB9
#define sFLASH_CMD_Re             0xAB

#define sFLASH_WIP_FLAG           0x01  /*!< Write In Progress (WIP) flag */

#define sFLASH_DUMMY_BYTE         0x00
#define sFLASH_SPI_PAGESIZE       0x100
#define FLASH_SECTOR_SIZE         4096
#define FLASH_SECTOR_COUNT        512
#define FLASH_BLOCK_SIZE          65536

#define sFLASH_W25Q16_ID          0xEF4015

#define SPIx                             SPI1
#define sFLASH_SPI                       SPI1
#define SPIx_PeriphClockCmd              RCC_APB2PeriphClockCmd
#define SPIx_CLK                         RCC_APB2Periph_SPI1
#define SPIx_IRQn                        SPI1_IRQn
#define SPIx_IRQHandler                  SPI1_IRQHandler

#define SPIx_SCK_PIN                     GPIO_Pin_5                  
#define SPIx_SCK_GPIO_PORT               GPIOA                      
#define SPIx_SCK_GPIO_CLK                RCC_APB2Periph_GPIOA
#define SPIx_SCK_SOURCE                  GPIO_PinSource5
#define SPIx_SCK_AF                      GPIO_AF_0

#define SPIx_MISO_PIN                    GPIO_Pin_6                
#define SPIx_MISO_GPIO_PORT              GPIOA                      
#define SPIx_MISO_GPIO_CLK               RCC_APB2Periph_GPIOA
#define SPIx_MISO_SOURCE                 GPIO_PinSource6
#define SPIx_MISO_AF                     GPIO_AF_0

#define SPIx_MOSI_PIN                    GPIO_Pin_7         
#define SPIx_MOSI_GPIO_PORT              GPIOA                   
#define SPIx_MOSI_GPIO_CLK               RCC_APB2Periph_GPIOA
#define SPIx_MOSI_SOURCE                 GPIO_PinSource7
#define SPIx_MOSI_AF                     GPIO_AF_0

#define SPIx_CS_PIN                      GPIO_Pin_4              
#define SPIx_CS_GPIO_PORT                GPIOA                  

/* Select sFLASH: Chip Select pin low */
#define sFLASH_CS_LOW()       GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET)
/* Deselect sFLASH: Chip Select pin high */
#define sFLASH_CS_HIGH()      GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET) 

void SPI_FLASH_PageErase(uint32_t PageAddr);
void sFLASH_EraseSector(uint32_t SectorAddr);
void sFLASH_EraseBulk(void);
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void SPI_Flash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
uint32_t sFLASH_ReadID(void);
void sFLASH_StartReadSequence(uint32_t ReadAddr);

/**
  * @brief  Low layer functions
  */
uint8_t sFLASH_ReadByte(void);
uint8_t sFLASH_SendByte(uint8_t byte);
uint16_t sFLASH_SendHalfWord(uint16_t HalfWord);
void sFLASH_WriteEnable(void);
void sFLASH_WaitForWriteEnd(void);
void sFLASH_PowerDown(void);
void sFLASH_ReleasePowerDown(void);

void SPI_GPIO_Config(void);
void SPI_Config(void);
void sFLASH_sector_read(uint8_t * buffer, uint32_t sector, uint16_t sector_number);
void sFLASH_sector_write(uint8_t * buffer, uint32_t sector, uint16_t sector_number);

#endif
