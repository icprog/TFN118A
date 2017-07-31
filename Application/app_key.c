#include "app_key.h"
#include "app_init.h"
/*******************************************************************************
** ��Ȩ:		
** �ļ���: 		app_key.c
** �汾��  		1.0
** ��������: 	MDK-ARM 5.23
** ����: 		cc
** ��������: 	2017-07-13
** ����:		  
** ����ļ�:	app_key.h
** �޸���־��	
** ��Ȩ����   
*******************************************************************************/

uint16_t key_tim;//����ֵ�����40ms
uint16_t key_double_tim;//��¼�����󣬰���̧��ʱ��
uint16_t key_up_tim;//����̧���ʱ
 
#define key_double_interval 12   //480ms
#define key_up_delay (key_double_interval*2)
extern uint16_t Key_Alarm_Delay;
uint8_t Key_Scan_En;//����ɨ��ʹ�ܱ�־λ

void Key_Func(void);//�������ܴ�����
extern uint8_t State_Key_Alram;//��������
//��ǩ״̬��
extern TAG_STATE_Typedef TAG_STATE;//��ǩ
typedef enum
{
	no_press,
	short_press=1,//�̰�
	double_press,//˫��
	long_press//����
}Key_Value;

//typedef struct
//{
//	uint8_t short_press_happen;//�̰���־λ
//	uint8_t double_press_happen;//˫����־λ
//	uint8_t long_press_happen;//������־λ	
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
@Description:����������ʱ��
@Input:��
@Output:��
@Return:��
*************************************************/ 
void key_tim_start(void)
{
	rtc1_init();
}

/************************************************* 
@Description:�����ж��¼�
@Input:��
@Output:��
@Return:��
*************************************************/ 
void onKeyEvent(void)
{
	key_tim_start();//����������ʱ��
	Key_Read_Disable_Interrupt();//�����жϲ�ʹ��
}


/************************************************* 
@Description:������ʱ��
@Input:��
@Output:��
@Return:��
*************************************************/ 
void RTC1_IRQHandler(void)
{
	if(NRF_RTC1->EVENTS_COMPARE[0])
	{
		NRF_RTC1->EVENTS_COMPARE[0]=0UL;	//clear event
		NRF_RTC1->TASKS_CLEAR=1UL;	//clear count
//		key_tim++;//��������
//		key_double_tim++;
		Key_Scan_En = 1;//����ɨ��ʹ��
	}
}


/************************************************* 
Description:������ʼ��
Input:��
Output:��
Return:��
*************************************************/
/*
void key_init(void)
{
	//����
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
@Description:����ɨ�� �̰�������
@Input:��
@Output:��
@Return:��
*************************************************/ 
//void Get_Key_Value(void)
//{
//	if(!Read_KEY)//��������
//	{
//		key_up_tim = 0;
//		if(0 == k1.Flag.short_press_happen)
//		{
//			k1.Flag.short_press_happen = 1;
//			key_tim = 0; //�������£���ʱ��0
//		}
//		else if(1 == k1.Flag.short_press_happen)
//		{
//			if(key_tim > Key_Alarm_Delay)//��������Ϊ1
//			{
//				k1.Value = long_press;//����
//				k1.Flag.long_press_happen = 1;
//				k1.Flag.short_press_happen = 0;
//			}
//		}
//	}
//	if(Read_KEY)//����̧��
//	{
//		key_up_tim++;
//		if(1 == k1.Flag.short_press_happen)//k1.Flag.short_press_happen=1��˵������Ϊ�̰�
//		{
//			k1.Flag.short_press_happen = 0;//����̰��¼�
//			if(0 == k1.Flag.double_press_happen)//��һ��̧��
//			{
//				k1.Flag.double_press_happen = 1;//����˫����־λ��1,�ȴ�ȷ���Ƿ�Ϊ˫��
//				key_double_tim = 0;
//			}
//			else if(1 == k1.Flag.double_press_happen)//�ڶ���̧��
//			{
//				if(key_double_tim < key_double_interval) //��һ�η����̰�����500ms�ڷ����ڶ��ζ̰������һ��˫��
//				{
//					k1.Value = double_press;//˫��
//					k1.Flag.double_press_happen = 0;//�����־λ
//				}
//			}
//		}
//		else if(1 == k1.Flag.double_press_happen)//��һ�ζ̰��󣬵ȴ�500ms�����δ�����̰�������ֵ
//		{
//			if(key_double_tim > key_double_interval)
//			{
//				k1.Flag.double_press_happen = 0;
//				k1.Value = short_press;//�̰�
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
		case 1://�̰������ж�
			if(Read_KEY)//�̰�
			{
				key_state = 3;
				key_double_tim = 0;
			}
			else if(!Read_KEY)
			{
				key_tim++;
				if(key_tim > Key_Alarm_Delay )//����
				{
					k1.Value = long_press;
					key_state = 2; 
				}
			}
			break;
		case 2://�����ͷ�
			if(Read_KEY)
			{
				key_state = 0;
			}
			break;
		case 3://�̰�˫���ж�
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
		case 4://˫�������ж�
			if(Read_KEY)//˫��
			{
				key_state = 0;
				k1.Value = double_press;
			}
			else if(!Read_KEY)//����
			{
				key_tim++;
				if(key_tim > Key_Alarm_Delay )//����
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
@Description:����������
@Input:��
@Output:��
@Return:���ؼ�ֵ
*************************************************/ 
void Key_Deal(void)
{
	if(Key_Scan_En)//40msɨ��һ�ΰ���
	{
		Key_Scan_En = 0;
		Get_Key_Value();//��ȡ��ֵ
		Key_Func();//�������ܺ���
	}
}

/************************************************* 
@Description:�����û�����
@Input:��
@Output:��
@Return:���ؼ�ֵ
*************************************************/ 
uint8_t key_cnt;
void Key_Func(void)
{

	if(0 == key_state)//̧��
	{
		rtc1_deinit();//ֹͣɨ�谴��	
		Key_Read_Enable_Interrupt();//���� �ж�ʹ��
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

