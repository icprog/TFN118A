/*******************************************************************************
** 版权:		
** 文件名: 		Debug_log.c
** 版本：  		1.0
** 工作环境: 	MDK-ARM 5.23
** 作者: 		cc
** 生成日期: 	2017-07-29
** 功能:		  
** 相关文件:	Debug_log.h
** 修改日志：	
** 版权所有   
** 使用说明
需要先初始化，比如调用一下函数
#ifdef LOG_ON
	debug_log_init();
#endif
当然需要定义LOG_ON
//调用debug_printf，需要
#define ENABLE_DEBUG_PRINT

*******************************************************************************/
#include "debug_log.h"
#include "app_init.h"
bool debug_log_init(void)
{
	bool result = true;
	
#if defined(ENABLE_UART_LOG)
//	result = UART_Init();
	UART_Init();
#else	// LCD
//	BSP_LCD_Init();
//	BSP_LCD_LayerDefaultInit(1, LCD_FRAME_BUFFER);
//	BSP_LCD_SelectLayer(1);
//	
//	LCD_LOG_Init();
//	LCD_LOG_SetHeader((uint8_t*)"This is the header");
//	LCD_LOG_SetFooter((uint8_t*)"This is the footer");
#endif

	return result;
}
