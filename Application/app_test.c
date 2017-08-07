/*******************************************************************************
** 版权:		
** 文件名: 		app_test.c
** 版本：  		1.0
** 工作环境: 	MDK-ARM 5.23
** 作者: 		cc
** 生成日期: 	2017-07-13
** 功能:		  
** 相关文件:	app_test.h
** 修改日志：	
** 版权所有   
*******************************************************************************/

#include "app_test.h"
#include "oled.h"
#include "sys.h"
#include "lis3dh.h"


/************************************************* 
@Description:硬件功能测试
@Input:无
@Output:
@Return:无
*************************************************/ 
#define TEST 1
#if TEST
void function_test(void)
{
	
	OLED_Init();//OLED测试
	Motor_Run();//电机测试
	nrf_delay_ms(250);
	Motor_Stop();
	nrf_delay_ms(1000);	
	lis3dhInit();//加速度测试
}
#endif