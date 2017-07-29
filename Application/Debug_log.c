/*******************************************************************************
** ��Ȩ:		
** �ļ���: 		Debug_log.c
** �汾��  		1.0
** ��������: 	MDK-ARM 5.23
** ����: 		cc
** ��������: 	2017-07-29
** ����:		  
** ����ļ�:	Debug_log.h
** �޸���־��	
** ��Ȩ����   
** ʹ��˵��
��Ҫ�ȳ�ʼ�����������һ�º���
#ifdef LOG_ON
	debug_log_init();
#endif
��Ȼ��Ҫ����LOG_ON
//����debug_printf����Ҫ
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
