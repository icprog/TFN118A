#ifndef _GT24L24A2Y
#define _GT24L24A2Y
#include "HFMA2Ylib.h"

//命令
#define READ_CMD 0X03     //一般读取		
#define FAST_READ_CMD 0X0B //快速读取

#define WRITE_EN_CMD 0X06	//写使能
#define WRITE_DIS_CMD 0X04  //写非能
#define PAGE_PROGRAM_CMD 0X02 //页写入
#define SECTOR_ERASE_CMD 0X20 //扇区擦除

#define DEEP_SLEEP_CMD 0XB9  //深度睡眠
#define LEAVE_SLEEP_CMD 0XAB //唤醒

extern void GT24L24A2Y_Test(void);
extern void GT24L24A2Y_Spi_Init(void);
extern void GT24L24A2Y_Spi_DeInit(void);
#endif
