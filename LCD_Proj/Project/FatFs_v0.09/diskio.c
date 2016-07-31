/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* by grqd_xp                                                            */
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include <string.h>
#include "diskio.h"
#include "spi_spiflash.h"
#include "delay.h"
/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
/* Note that Tiny-FatFs supports only single drive and always            */
/* accesses drive number 0.                                              */

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{	
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{	 
  sFLASH_sector_read((uint8_t *)buff,sector,count);
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	  sFLASH_sector_write((uint8_t *)(buff),sector,count);  
  	return RES_OK;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;
	DWORD nFrom,nTo;
	int i;
	
	switch(ctrl)
	{
		case CTRL_SYNC :
			break;
		
	  case GET_SECTOR_SIZE:
			*(WORD*)buff = FLASH_SECTOR_SIZE;
			break;
	 
	  case GET_SECTOR_COUNT:
			*(DWORD*)buff = FLASH_SECTOR_COUNT;
			break;
	 
	  case GET_BLOCK_SIZE:
			*(DWORD*)buff = FLASH_BLOCK_SIZE;
			break;
		
		//ÉÈÇø²Á³ý
		case CTRL_ERASE_SECTOR:
			nFrom = *((DWORD*)buff);
			nTo = *(((DWORD*)buff)+1);
			for(i = nFrom;i <= nTo;i ++)
				sFLASH_EraseSector(i*FLASH_SECTOR_SIZE);	
			break;
			
		default:
			res = RES_PARERR;
			break;
	}
	return res;
}

DWORD get_fattime(void){
	return 0;
}























