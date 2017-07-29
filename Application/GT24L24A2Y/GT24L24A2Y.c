/*******************************************************************************
** 版权:		杭州檀木科技股份有限公司
** 文件名: 		GT24L24A2Y.c
** 版本：  		1.0
** 工作环境: 	MDK-ARM 5.23
** 作者: 		cc
** 生成日期: 	2017-07-07
** 功能:		  
** 相关文件:	GT24L24A2Y.h
** 修改日志：	
** 版权所有
** SPI模式 : SPI0 SPI_MODE0(CPOL=0,CPHA=0)  MsbFirst  1Mbps(SPI_OPERATING_FREQUENCY)
*******************************************************************************/

#include "GT24L24A2Y.h"
#include "spi_master.h"
#include "app_init.h"
#include "HFMA2Ylib.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
/************************************************* 
@Description:字库spi初始化
@Input:
@Output:无
@Return:无
*************************************************/  
void GT24L24A2Y_Init(void)
{
	spi_master_init(SPI0,SPI_MODE0,0);
}

/************************************************* 
@Description:字库读点阵数据
	addr：地址，length:数据长度,prbuff接收数据
@Input:
@Output:无
@Return:无
*************************************************/ 
static void GT24L24A2Y_Read_Bytes(u32 addr,u8 length,u8*prbuff)
{
	u8 txbuff[100];
	txbuff[0]=READ_CMD;
	txbuff[1]=addr>>16;
	txbuff[2]=addr>>8;
	txbuff[3]=addr;
	spi_master_tx_rx((uint32_t *)NRF_SPI0,length+4,txbuff,prbuff);
}


// 附：	r_dat_bat 从字库中读数据函数 该函数需客户自己实现，如下是实现该函数的参考程序。
/****************************************************
u8 r_dat_bat(u32 address,u8 byte_long,u8 *p_arr)
@Description:
DTAB_addr:表示字符点阵在芯片中的字节地址。
DNum:      	是读点阵数据字节数。
*p_arr   ： 是保存读出的点阵数据的数组。
*****************************************************/
void r_dat_bat(unsigned long TAB_addr,unsigned int Num,unsigned char *p_arr)
{
	GT24L24A2Y_Read_Bytes(TAB_addr,Num,p_arr);
}


/************************************************* 
@Description:字库读点阵测试
	addr：地址，length:数据长度,prbuff接收数据
@Input:
@Output:无
@Return:无
*************************************************/ 
uint8_t buff[40];
void GT24L24A2Y_Test(void)
{
	nrf_gpio_cfg_output(3);
	nrf_gpio_cfg_output(4);
	nrf_gpio_cfg_output(5);
	nrf_gpio_cfg_output(6);

    nrf_gpio_pin_set(3); /* disable Set slave select (inactive high) */
	nrf_gpio_pin_set(4); /* disable Set slave select (inactive high) */
	nrf_gpio_pin_set(5); /* disable Set slave select (inactive high) */
	nrf_gpio_pin_set(6); /* disable Set slave select (inactive high) */
	
	while(1)
	{
		nrf_gpio_pin_set(5);
		nrf_gpio_pin_set(6);
		nrf_delay_ms(10);
		nrf_gpio_pin_clear(5);
		nrf_gpio_pin_clear(6);
		nrf_delay_ms(10);
	}
//	GT24L24A2Y_Init();
//	ASCII_GetData('A',ASCII_8X16,buff);
}


