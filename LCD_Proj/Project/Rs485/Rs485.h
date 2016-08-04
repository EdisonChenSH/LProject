#ifndef _RS485_H_
#define _RS485_H_
#include "stm32f10x.h"
#include "stdint.h"
#include <string.h>

extern void Init_Rs485(void);
extern void rs485SendByteArray(u8* bytesPtr,u16 length);
extern void rs485SetRxInterrupt(u8 status);

#endif
