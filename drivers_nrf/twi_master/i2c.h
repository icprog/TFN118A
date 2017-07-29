#ifndef __I2C_EE_H
#define __I2C_EE_H



/* 包含头文件 ********************************************************/
#include "nrf.h"

/* 宏定义 --------------------------------------------------------------------*/
#define u8    uint8_t
#define u16   uint16_t

//AS7000设备地址
#define i2c_device_address 0x60  //0x30<<1

/* 函数定义------------------------------------------------------------------ */
char I2C_SendDates(u8 Slaveraddr,u8* SendData,u8 DataLength);
char I2C_ReadDatas(u8 Slaveraddr,u8 addr,u8 *dat, u8 nbytes);


#endif /* __I2C_EE_H */

/************ (C) COPYRIGHT 2011 Lierda **************文件结束*******/

