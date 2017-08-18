#ifndef _AS3933_SPI
#define _AS3933_SPI

#include "sys.h"
#include "as3933.h"

extern u8 SPI_RW(uint8_t byte);//SPI FLASH 写数据
uint8_t as3933ReadRegister(uint8_t address,u8 *value);
void as3933WriteRegister(u8 address, u8 value);
u8 as3933ModifyRegister(u8 address, u8 mask, u8 value);
u8 as3933SendCommand(as3933Commands_t command);
#endif

