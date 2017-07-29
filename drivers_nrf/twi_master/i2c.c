/*******************************************************************************
** 版权:		杭州檀木科技股份有限公司
** 文件名: 		iic.c
** 版本：  		1.0
** 工作环境: 	MDK-ARM 5.23
** 作者: 		cc
** 生成日期: 	2017-07-07
** 功能:		  
** 相关文件:	iic.h
** 修改日志：	
** 版权所有   
*******************************************************************************/

/* 包含头文件 *****************************************************************/
#include "i2c.h"
#include "twi_master.h"
#include "sys.h"
/* 类型声明 ------------------------------------------------------------------*/
/* 宏 ------------------------------------------------------------------------*/
/* 变量 ----------------------------------------------------------------------*/

/* 函数声明 ------------------------------------------------------------------*/
/* 函数功能 ------------------------------------------------------------------*/

/************************************************* 
@Description:I2C 初始化
@Input:
@Output:无
@Return:
*************************************************/  
void I2C_Init(void)
{
	twi_master_init();
}
/************************************************* 
@Description:I2C 卸载
@Input:	
@Output:无
@Return:
*************************************************/  
void I2C_DeInit(void)
{
	twi_master_deinit();
}
/************************************************* 
@Description:I2C 写函数
@Input:	Slaveraddr((b7~b0)&0xfe):从机地址；
		SendData:寄存器地址+数据；
		DataLength：发送长度，寄存器地址+数据
@Output:无
@Return:TRUE:成功 FALSE:错误
*************************************************/  
char I2C_SendDates(u8 Slaveraddr,u8* SendData,u8 DataLength)
{
 		bool i2c_trans_state=false;   	
		i2c_trans_state  = twi_master_transfer(Slaveraddr,SendData,DataLength,TWI_ISSUE_STOP);  
		if(true == i2c_trans_state)
			return TRUE;
		else
			return FALSE;
}


/************************************************* 
@Description:I2C 写函数
@Input:	Slaveraddr((b7~b0)&0xfe):从机地址 
		ddr：寄存器地址-读起始地址 
		dat数据缓冲区 nbytes：数据个数  
@Output:dat输出读取的数据
@Return:TRUE:成功 FALSE:错误
*************************************************/ 
char I2C_ReadDatas(u8 Slaveraddr,u8 addr,u8 *dat, u8 nbytes)
{
	bool i2c_trans_state=false;  
	uint32_t timeout;
	i2c_trans_state=twi_master_transfer(Slaveraddr,&addr,1,TWI_ISSUE_STOP);   //设备地址+寄存器地址
	timeout = 0xFFFFFF;
	while((i2c_trans_state==false) && (--timeout));
	if (timeout == 0)
	{  
		return FALSE;
	}
	i2c_trans_state=twi_master_transfer((Slaveraddr|0x01),dat,nbytes,TWI_ISSUE_STOP);//读取数据  
	timeout = 0xFFFFFF;
	while((i2c_trans_state==false) && (--timeout));
	if (timeout == 0)
	{  
		return FALSE;
	}	
	if(true == i2c_trans_state)
		return TRUE;
	else
		return FALSE;
}


