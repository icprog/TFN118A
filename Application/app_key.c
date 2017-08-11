#include "app_key.h"
#include "app_init.h"
/*******************************************************************************
** 版权:		
** 文件名: 		app_key.c
** 版本：  		1.0
** 工作环境: 	MDK-ARM 5.23
** 作者: 		cc
** 生成日期: 	2017-07-13
** 功能:		  
** 相关文件:	app_key.h
** 修改日志：	
** 版权所有   
*******************************************************************************/

uint16_t key_tim;//计数值，间隔40ms
uint16_t key_double_tim;//记录单机后，按键抬起时间
uint16_t key_up_tim;//按键抬起计时
 
#define key_double_interval 12   //480ms
#define key_up_delay (key_double_interval*2)
extern uint16_t Key_Alarm_Delay;
uint8_t Key_Scan_En;//按键扫描使能标志位

void Key_Func(void);//按键功能处理函数
extern uint8_t State_Key_Alram;//按键报警
//标签状态字
extern TAG_STATE_Typedef TAG_STATE;//标签
typedef enum
{
	no_press,
	short_press=1,//短按
	double_press,//双击
	long_press//长按
}Key_Value;

//typedef struct
//{
//	uint8_t short_press_happen;//短按标志位
//	uint8_t double_press_happen;//双击标志位
//	uint8_t long_press_happen;//长按标志位	
//}Key_Flag;


typedef struct
{
	Key_Value Value;
//	Key_Flag  Flag;
//	uint8_t key_up;
}Key_Typedef;

Key_Typedef k1;
uint8_t key_state;
//void Key_Init(void)
//{
//	
//}
/************************************************* 
@Description:按键消抖定时器
@Input:无
@Output:无
@Return:无
*************************************************/ 
void key_tim_start(void)
{
	rtc1_init();
}

/************************************************* 
@Description:按键中断事件
@Input:无
@Output:无
@Return:无
*************************************************/ 
void onKeyEvent(void)
{
	key_tim_start();//启动消抖定时器
	Key_Read_Disable_Interrupt();//按键中断不使能
}


/************************************************* 
@Description:消抖计时器
@Input:无
@Output:无
@Return:无
*************************************************/ 
void RTC1_IRQHandler(void)
{
	if(NRF_RTC1->EVENTS_COMPARE[0])
	{
		NRF_RTC1->EVENTS_COMPARE[0]=0UL;	//clear event
		NRF_RTC1->TASKS_CLEAR=1UL;	//clear count
//		key_tim++;//按键计数
//		key_double_tim++;
		Key_Scan_En = 1;//按键扫描使能
	}
}


/************************************************* 
Description:按键初始化
Input:无
Output:无
Return:无
*************************************************/
/*
void key_init(void)
{
	//按键
	NRF_GPIO->PIN_CNF[KEY_Pin_Num]=(GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos)//low level
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
	NRF_GPIOTE->EVENTS_PORT=0UL;
	NRF_GPIOTE->INTENSET=GPIOTE_INTENSET_PORT_Enabled << GPIOTE_INTENSET_PORT_Pos;	//PORT
	NVIC_SetPriority(GPIOTE_IRQn, PORT_PRIORITY);
	NVIC_EnableIRQ(GPIOTE_IRQn);
}
*/
/************************************************* 
@Description:按键扫描 短按、长按
@Input:无
@Output:无
@Return:无
*************************************************/ 
//void Get_Key_Value(void)
//{
//	if(!Read_KEY)//按键按下
//	{
//		key_up_tim = 0;
//		if(0 == k1.Flag.short_press_happen)
//		{
//			k1.Flag.short_press_happen = 1;
//			key_tim = 0; //按键按下，计时清0
//		}
//		else if(1 == k1.Flag.short_press_happen)
//		{
//			if(key_tim > Key_Alarm_Delay)//长按，置为1
//			{
//				k1.Value = long_press;//长按
//				k1.Flag.long_press_happen = 1;
//				k1.Flag.short_press_happen = 0;
//			}
//		}
//	}
//	if(Read_KEY)//按键抬起
//	{
//		key_up_tim++;
//		if(1 == k1.Flag.short_press_happen)//k1.Flag.short_press_happen=1，说明按键为短按
//		{
//			k1.Flag.short_press_happen = 0;//清除短按事件
//			if(0 == k1.Flag.double_press_happen)//第一次抬起
//			{
//				k1.Flag.double_press_happen = 1;//按键双击标志位置1,等待确认是否为双击
//				key_double_tim = 0;
//			}
//			else if(1 == k1.Flag.double_press_happen)//第二次抬起
//			{
//				if(key_double_tim < key_double_interval) //第一次发生短按后，在500ms内发生第二次短按，完成一次双击
//				{
//					k1.Value = double_press;//双击
//					k1.Flag.double_press_happen = 0;//清除标志位
//				}
//			}
//		}
//		else if(1 == k1.Flag.double_press_happen)//第一次短按后，等待500ms，如果未发生短按，更新值
//		{
//			if(key_double_tim > key_double_interval)
//			{
//				k1.Flag.double_press_happen = 0;
//				k1.Value = short_press;//短按
//			}
//		}
//		else if(1 == k1.Flag.long_press_happen)
//		{
//			k1.Flag.long_press_happen = 0;
//			k1.Flag.short_press_happen = 0;
//		}

//	}
//}


void Get_Key_Value(void)
{
	switch(key_state)//
	{
		case 0:if(!Read_KEY) { key_state = 1; key_tim=0;}break;
		case 1://短按长按判断
			if(Read_KEY)//短按
			{
				key_state = 3;
				key_double_tim = 0;
			}
			else if(!Read_KEY)
			{
				key_tim++;
				if(key_tim > Key_Alarm_Delay )//长按
				{
					k1.Value = long_press;
					key_state = 2; 
				}
			}
			break;
		case 2://长按释放
			if(Read_KEY)
			{
				key_state = 0;
			}
			break;
		case 3://短按双击判断
			key_double_tim++;
			if(!Read_KEY)
			{
				if(key_double_tim<key_double_interval)
				{
					key_state = 4;
					key_tim = 0;
				}
			}
			else if(Read_KEY)
			{
				if(key_double_tim > key_double_interval)
				{
					k1.Value = short_press;
					key_state = 0;
				}
			}
			break;
		case 4://双击长按判断
			if(Read_KEY)//双击
			{
				key_state = 0;
				k1.Value = double_press;
			}
			else if(!Read_KEY)//长按
			{
				key_tim++;
				if(key_tim > Key_Alarm_Delay )//长按
				{
					k1.Value = long_press;
					key_state = 2; 
				}
			}
			break;
		default : key_state = 0;break;
	}
}
/************************************************* 
@Description:按键处理函数
@Input:无
@Output:无
@Return:返回键值
*************************************************/ 
void Key_Deal(void)
{
	if(Key_Scan_En)//40ms扫描一次按键
	{
		Key_Scan_En = 0;
		Get_Key_Value();//获取键值
		Key_Func();//按键功能函数
	}
}

/************************************************* 
@Description:按键用户函数
@Input:无
@Output:无
@Return:返回键值
*************************************************/ 
uint8_t key_cnt;
void Key_Func(void)
{
	if(0 == key_state)//抬起
	{
		rtc1_deinit();//停止扫描按键	
		Key_Read_Enable_Interrupt();//按键 中断使能
	}
	switch(k1.Value)
	{
		case short_press:
			key_cnt = 1;
			k1.Value = no_press;
			break;
		case double_press:
			key_cnt = 2;
			k1.Value = no_press;		
			break;
		case long_press:
			key_cnt = 3;
			TAG_STATE.State_Key_Alram = 1;
			k1.Value = no_press;
			break;
		case no_press:
			break;
	}
//	key_cnt = 0;
}

