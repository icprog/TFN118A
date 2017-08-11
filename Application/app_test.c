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
const uint16_t Motor_Buf[4] = {0xB5E7,0XBBFA,0XB2E2,0XCAD4}; //电机测试GB18030
const uint16_t GSensor_Buf[5] = {0xBCD3,0XCBD9,0XB6C8,0XB2E2,0XCAD4};//加速度测试GB18030
const uint16_t GSensorWrong_Buf[5] = {0xBCD3,0XCBD9,0XB6C8,0XCAA7,0XC0DC};//加速度失败GB18030
void function_test(void)
{

	OLED_Init();//OLED测试
	FilleScreen(COLOR_WHITE);//点亮全屏
	nrf_delay_ms(1000);
//	OLED_Show_ChineseS(8,0,Motor_Buf,4);
//	OLED_Refresh_Gram();
//	Motor_Run();//电机测试
//	nrf_delay_ms(500);
//	Motor_Stop();
//	nrf_delay_ms(1000);
//	OLED_Show_ChineseS(8,0,GSensor_Buf,5);
//	OLED_Refresh_Gram();
//	if(!lis3dhInit())//加速度测试
//	{
//		OLED_Show_ChineseS(8,0,GSensorWrong_Buf,5);
//	}

}
#endif

