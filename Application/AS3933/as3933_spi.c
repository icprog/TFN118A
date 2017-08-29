#include "as3933_spi.h"
#define ERR_NONE 0
/************************************************* 
@Description:SPI读写
@Input:值
@Output:值
@Return:无
*************************************************/ 
u8 SPI_RW(uint8_t byte)//SPI FLASH 写数据
{
	uint8_t bit_ctr;
	AS3933_SCLK_Clr;//SCLK =0;
	__nop();__nop();
	for(bit_ctr=0;bit_ctr<8;bit_ctr++)            // 输出8位
	{
		//写，上升沿移位
		if(byte&0x80) 
		{	
			AS3933_SDI_Set;//SDI=1;
		}
		else 
		{
			AS3933_SDI_Clr;//SDI=0;
		}
		AS3933_SCLK_Set;//SCLK =1;			
		byte=(byte<<1);	                // shift next bit to MSB   
		__nop();__nop();
		//读：下降沿读数据
		AS3933_SCLK_Clr;//SCLK =0;
		if(Read_AS3933_SDO) 
		{	
			byte|=0x01;//SDO
		}
		else
		{			
			byte&=~0x01;
		}
		
		__nop();__nop();
	}
	return byte;
}  

/************************************************* 
@Description:读寄存器
@Input:地址，值
@Output:无
@Return:无
*************************************************/ 
uint8_t as3933ReadRegister(uint8_t address,u8 *value)
{
	u8 outbuf[2];

	outbuf[0] = 0x40 | (address & 0x3F);   // read mode
	outbuf[1] = 0x00;
	
	AS3933_CS_Set;//CS = 1;
	SPI_RW(outbuf[0]);
	*value = SPI_RW(0);
	AS3933_CS_Clr;//CS=0;
	__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();
	__nop();__nop();
	return ERR_NONE;
} 

u8 as3933ReadMultiRegister(u8 startAddress, u8 *values, u8 count)
{
    // ToDo: implementation of as3933ReadMultiRegister
    return ERR_NONE;
}


/************************************************* 
@Description:写寄存器
@Input:地址，值
@Output:无
@Return:无
*************************************************/ 
void as3933WriteRegister(u8 address, u8 value)
{
	u8 outbuf[2];

	outbuf[0] = address & 0x3F;   // write mode
	outbuf[1] = value;

	AS3933_CS_Set;//CS = 1;
	SPI_RW(outbuf[0]);
	SPI_RW(outbuf[1]);
	AS3933_CS_Clr;//CS=0;
	__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();
	__nop();__nop();
//		AS3933_CS_Set;//CS = 1;
} 

u8 as3933WriteMultiRegister(u8 startAddress, u8 *values, u8 count)
{
    // ToDo: implementation of as3933WriteMultiRegister
    return ERR_NONE;
}

/************************************************* 
@Description:对寄存器某些位进行修改
@Input:address：地址 ；比如修改bit0为1，mask=0x01 value = 0x01
@Output:无
@Return:无
*************************************************/ 
u8 as3933ModifyRegister(u8 address, u8 mask, u8 value)
{
	u8 registerValue;

	as3933ReadRegister(address, &registerValue);
	registerValue &= ~mask;   // clear all bits in masked area
	registerValue |= (value & mask);   // set all desired bits in masked area
	as3933WriteRegister(address, registerValue);

	return ERR_NONE;
}

/************************************************* 
@Description:发送命令
@Input:command：命令代码
@Output:无
@Return:无
*************************************************/ 
u8 as3933SendCommand(as3933Commands_t command)
{
	u8 outbuf;
	outbuf = 0xC0 | (command & 0x3F);   // command mode
	AS3933_CS_Set;//CS = 1;
	SPI_RW(outbuf);
	AS3933_CS_Clr;//CS=0;
	__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();
	__nop();__nop();
//		AS3933_CS_Set;//CS = 1;
	return ERR_NONE;
}



