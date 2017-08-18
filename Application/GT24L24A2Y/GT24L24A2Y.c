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
#include "HFMA2Ylib.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "oled.h"
//#define spi_hard
#ifdef spi_hard
#else
#define SPI_PSELSCK0              4   /*!< GPIO pin number for SPI clock (note that setting this to 31 will only work for loopback purposes as it not connected to a pin) */
#define SPI_PSELMOSI0             7   /*!< GPIO pin number for Master Out Slave In    */
#define SPI_PSELMISO0             5   /*!< GPIO pin number for Master In Slave Out    */
#define SPI_PSELSS0               3   /*!< GPIO pin number for Slave Select           */

#define Set_SPI_CLK_H() do{NRF_GPIO->OUTSET = (1 << SPI_PSELSCK0);}while(0)
#define Set_SPI_CLK_L() do{NRF_GPIO->OUTCLR = (1 << SPI_PSELSCK0);}while(0)

#define Set_SPI_CS_H() do{NRF_GPIO->OUTSET = (1 << SPI_PSELSS0);}while(0)
#define Set_SPI_CS_L() do{NRF_GPIO->OUTCLR = (1 << SPI_PSELSS0);}while(0)

#define Set_SPI_MOSI_H() do{NRF_GPIO->OUTSET = (1 << SPI_PSELMOSI0);}while(0)
#define Set_SPI_MOSI_L() do{NRF_GPIO->OUTCLR = (1 << SPI_PSELMOSI0);}while(0)

#define Read_SPI_MISO 	((NRF_GPIO->IN >> SPI_PSELMISO0)&1)	

#endif

uint8_t rxbuff[80];
unsigned int GB_CODE;
/************************************************* 
@Description:字库spi初始化
@Input:
@Output:无
@Return:无
*************************************************/  
void GT24L24A2Y_Spi_Init(void)
{
	#ifdef spi_hard
	spi_master_init(SPI0,SPI_MODE0,0);
	#else
	nrf_gpio_cfg_output(SPI_PSELSCK0);
	nrf_gpio_cfg_output(SPI_PSELMOSI0);
	nrf_gpio_cfg_input(SPI_PSELMISO0, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_output(SPI_PSELSS0);
	Set_SPI_CS_H();
	nrf_delay_ms(1);
	#endif
}

/************************************************* 
@Description:字库spi初始化
@Input:
@Output:无
@Return:无
*************************************************/  
void GT24L24A2Y_Spi_DeInit(void)
{
	#ifdef spi_hard

	#else
	NRF_GPIO->PIN_CNF[SPI_PSELSCK0] = IO_LP_State;
	NRF_GPIO->PIN_CNF[SPI_PSELMOSI0] = IO_LP_State;
	NRF_GPIO->PIN_CNF[SPI_PSELMISO0] = IO_LP_State;
	NRF_GPIO->PIN_CNF[SPI_PSELSS0] = IO_LP_State;
	#endif
}

#ifdef spi_hard
/************************************************* 
@Description:字库读点阵数据
	addr：地址，length:数据长度,prbuff接收数据
@Input:
@Output:无
@Return:无
*************************************************/ 
static void GT24L24A2Y_Read_Bytes(u32 addr,u8 length,u8* prbuff)
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
@DTAB_addr:表示字符点阵在芯片中的字节地址。
@DNum:      	是读点阵数据字节数。
@*p_arr   ： 是保存读出的点阵数据的数组。
*****************************************************/
void r_dat_bat(unsigned long TAB_addr,unsigned int Num,unsigned char *p_arr)
{
	GT24L24A2Y_Read_Bytes(TAB_addr,Num,p_arr);
	my_memcpy(rxbuff,&rxbuff[4],Num);
}
#else
void SendByte(u32 cmd)
{
	u8 i;
	cmd=cmd|0x03000000;
	for(i=0;i<32;i++)
	{
		Set_SPI_CLK_L();
		if(cmd&0x80000000)
		{
			Set_SPI_MOSI_H();
		}
		else 
		{
			Set_SPI_MOSI_L();
		}
		Set_SPI_CLK_H();
		cmd=cmd<<1;
	}					
}

u8 ReadByte(void)
{
	u8 i;
	u8 dat=0;
	Set_SPI_CLK_H();
	for(i=0;i<8;i++)
	{
		Set_SPI_CLK_L();
		dat=dat<<1;
		if(Read_SPI_MISO)
		{
			dat=dat|0x01;
		}
		else 
		{
			dat&=0xfe;
		}
		Set_SPI_CLK_H();		
	}	
	return dat;
}
/****************************************************
u8 r_dat_bat(u32 address,u8 byte_long,u8 *p_arr)
@Description:
DTAB_addr:表示字符点阵在芯片中的字节地址。
DNum:      	是读点阵数据字节数。
*p_arr   ： 是保存读出的点阵数据的数组。
*****************************************************/
void r_dat_bat(unsigned long TAB_addr,unsigned int Num,unsigned char *p_arr)
{
	u32 j=0;
	Set_SPI_CS_L();
	SendByte(TAB_addr);	 
	for(j=0;j<Num;j++)
	{
		p_arr[j]=ReadByte();
	}
	Set_SPI_CS_H();
}



#endif

/************************************************* 
@Description:字库读点阵测试
	addr：地址，length:数据长度,prbuff接收数据
@Input:
@Output:无
@Return:无
*************************************************/ 

void GT24L24A2Y_Test(void)
{
	GB_CODE=U2G(0x554a);
	ASCII_GetData('A',ASCII_8X16,rxbuff);
	my_memset(rxbuff,0,80);
	hzbmp16(SEL_GB, 0xb0a1, 0, 16,rxbuff);
	my_memset(rxbuff,0,80);
	OLED_Show_Chinese(0,8,0xb0a1,1);
	OLED_Refresh_Gram();
//	r_dat_bat(0,32,rxbuff);
//	my_memset(rxbuff,0,80);
}


