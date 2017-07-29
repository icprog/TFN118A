/*******************************************************************************
** ��Ȩ:		
** �ļ���: 		rtc.c
** �汾��  		1.0
** ��������: 	MDK-ARM 5.23
** ����: 		cc
** ��������: 	2017-07-14
** ����:		  
** ����ļ�:	rtc.h
** �޸���־��	
** ��Ȩ����   
*******************************************************************************/

#include "rtc.h"
#include "app_init.h"

#define TEST 1
#if TEST
//rtc_typedef Global_Time = {0x20,0x02,0x28,0x23,0x59,0x50,0x07};  //������ʱ���� ���� 
//uint8_t Global_Time[7] = {20,2,29,23,59,50,7};  //������ʱ���� ���� 
rtc_typedef Global_Time = {0x20,0x12,0x31,0x23,0x59,0x50,0x07};  //������ʱ���� ���� 

//rtc_typedef Global_Time = {0x21,0x02,0x28,0x23,0x59,0x50,0x04};//������ʱ���� ���� 
#else
rtc_typedef Global_Time = {0x17,0x07,0x14,0x15,0x09,0x00,0x05};  //������ʱ���� ���� 
#endif

uint8_t rtc_flag;//��ʱ����Ƶ����
//if(1==get_uart1_ready(0xffff))
//{
#define BCD
#ifdef BCD
/*BCD�����㷨*/
//uint8_t BCDInc(uint8_t *ucByte, uint8_t ucMin, uint8_t ucMax)
//{
//	if(*ucByte<ucMin||*ucByte>ucMax) *ucByte=ucMin;
//	if(*ucByte==ucMax)
//	{
//		*ucByte=ucMin;
//		return 1;
//	}
//	if((++*ucByte&0x0f)>9) *ucByte+=6;
//	return 0;
//}

////�����ṩ���ꡢ�¼��㵱�����һ�գ�֧�����꣬BCD��ʽ
//uint8_t DateMaxCalc21Cn(uint8_t ucBcdYeah, uint8_t ucBcdMonth)
//{
//	uint8_t ucBcdtmp1;

//	if(ucBcdMonth&0x10) ucBcdtmp1=(ucBcdMonth&0x01)?0x30:0x31;	//10,11,12
//	else
//	{
//		if(ucBcdMonth==2)	//2
//		{
//			ucBcdtmp1=0x28;
//			if((ucBcdYeah&0x01)==0)
//			{
//				if(ucBcdYeah&0x02)
//				{
//					if(ucBcdYeah&0x10) ucBcdtmp1=0x29;
//				}
//				else
//				{
//					if((ucBcdYeah&0x10)==0) ucBcdtmp1=0x29;
//				}
//			}
//		}
//		else	//1,3~9
//		{
//			ucBcdtmp1=(ucBcdMonth&0x08)?(ucBcdMonth-1):ucBcdMonth;	//8,9-->7,8
//			ucBcdtmp1=(ucBcdtmp1&0x01)?0x31:0x30;	
//		}				
//	}
//	return ucBcdtmp1;
//}

//void Calendar21Century(uint8_t * RTCtime)	//20xx
//{
//	if(BCDInc(&RTCtime[5],0x00,0x59))		//second
//	if(BCDInc(&RTCtime[4],0x00,0x59))		//minute
//	if(BCDInc(&RTCtime[3],0x00,0x23))		//hour
//	if(BCDInc(&RTCtime[2],0x01,DateMaxCalc21Cn(RTCtime[0], RTCtime[1])))	//date
//	if(BCDInc(&RTCtime[1],0x01,0x12))		//month
//	BCDInc(&RTCtime[0],0x00,0x99);		//year			
//}


/************************************************* 
@Description:ʮ����ת����BCD
       16->0X16
@Input:��
@Output:��
@Return:��
*************************************************/ 
u8 DecToBCD(uint8_t src)
{
	return ((src/10<<4)|src%10);
}

/************************************************* 
@Description:BCDת����10����
       16->0X16
@Input:��
@Output:��
@Return:��
*************************************************/ 
u8 BCDToDec(uint8_t src)
{
	return ((src >> 4)*10 +(src&0x0f));
}


/************************************************* 
@Description:rtcʱ���ۼӣ�����ʱ��
@Input:��
@Output:����1�������Ƿ���º����ʱ��
@Return:��
*************************************************/ 
uint8_t CalendarIncBCD(uint8_t *ucByte, uint8_t ucMin, uint8_t ucMax)
{
	if(*ucByte<ucMin||*ucByte>ucMax) *ucByte=ucMin;
	if(*ucByte==ucMax)
	{
		*ucByte=ucMin;
		return 1;
	}
	if((++*ucByte&0x0f)>9) *ucByte+=6;
	return 0;
}

/************************************************* 
@Description:�����ṩ���ꡢ�¼��㵱�����һ�գ�֧������,����ֻ����20xx��
@Input:��
@Output:��
@Return:��
*************************************************/ 
static uint8_t DateMaxCalc21Cn(uint8_t Year, uint8_t Month)
{
	uint8_t tmp1;
	switch(Month)
	{
		case 0x01:case 0x03:case 0x05:case 0x07:case 0x08:case 0x10:case 0x12: //31��
			tmp1 = 0x31;
			break;
		case 0x04:case 0x06:case 0x09:case 0x11://30��
			tmp1 = 0x30;
			break;
		case 2:
			Year = BCDToDec(Year);//ת��10����
			if(0 == (Year%4))//����ݲ���������ʱ������ܱ�4�����������꣬������ƽ��
				tmp1 = 0x29;//����29��
			else
				tmp1 = 0x28;//ƽ��28��
		default:
			break;
	}			
	return tmp1;
}

/************************************************* 
@Description:ʱ�����
@Input:��
@Output:��
@Return:��
*************************************************/ 
void Calendar21Century(rtc_typedef* pRTCtime)	//20xx
{
	if(CalendarIncBCD(&pRTCtime->sec,0x00,0x59))		//second
	{
		if(CalendarIncBCD(&pRTCtime->min,0x00,0x59))		//minute
		{
			if(CalendarIncBCD(&pRTCtime->hour,0x00,0x23))		//hour
			{
				CalendarIncBCD(&pRTCtime->week,0x01,0x07);//����
				if(CalendarIncBCD(&pRTCtime->day,1,DateMaxCalc21Cn(pRTCtime->year, pRTCtime->month)))	//date
				{
					if(CalendarIncBCD(&pRTCtime->month,0x01,0x12))		//month
					{
						CalendarIncBCD((uint8_t*)&pRTCtime->year,0x00,0x99);		//year	
					}
				}
			}
		}
	}								
}


/************************************************* 
@Description:rtcʱ������
@Input:��
@Output:��
@Return:��
*************************************************/ 
#define RTC_Sec_Pos	0
#define RTC_Sec_Msk (0x3f<<RTC_Sec_Pos)
#define RTC_Min_Pos 6
#define RTC_Min_Msk (0x3f<<RTC_Min_Pos)
#define RTC_Hour_Pos 12
#define RTC_Hour_Msk (0x1f<<RTC_Hour_Pos)
#define RTC_Day_Pos 17
#define RTC_Day_Msk (0x1f << RTC_Day_Pos)
#define RTC_Month_Pos 22
#define RTC_Month_Msk ( 0x0f << RTC_Month_Pos )
#define RTC_Year_Pos 26
#define RTC_Year_Msk (0x1f << RTC_Year_Pos)
void RTC_Time_Set(uint32_t RTCtime,uint8_t Week)
{
	rtc_typedef  temp_time;
	#if TEST
	Global_Time.year = 17;Global_Time.month = 7;Global_Time.day=22;
	Global_Time.hour = 18;Global_Time.min = 25;Global_Time.sec=50;
	RTCtime = (Global_Time.year << RTC_Year_Pos)|(Global_Time.month <<RTC_Month_Pos)|(Global_Time.day<<RTC_Day_Pos)
			| (Global_Time.hour << RTC_Hour_Pos) | (Global_Time.min << RTC_Min_Pos)|(Global_Time.sec <<RTC_Sec_Pos) ;
	#endif
	temp_time.sec = ((RTCtime&RTC_Sec_Msk)>>RTC_Sec_Pos);
	temp_time.min = ((RTCtime&RTC_Min_Msk)>>RTC_Min_Pos);
	temp_time.hour = ((RTCtime&RTC_Hour_Msk)>>RTC_Hour_Pos);
	temp_time.day = ((RTCtime&RTC_Day_Msk)>>RTC_Day_Pos);
	temp_time.month = ((RTCtime&RTC_Month_Msk)>>RTC_Month_Pos);
	temp_time.year = ((RTCtime&RTC_Year_Msk)>>RTC_Year_Pos);
	//ת��BCD
	Global_Time.sec = DecToBCD(temp_time.sec);//second
	Global_Time.min = DecToBCD(temp_time.min);//minute
	Global_Time.hour = DecToBCD(temp_time.hour);//hour
	Global_Time.day = DecToBCD(temp_time.day);//day
	Global_Time.month = DecToBCD(temp_time.month);//month
	Global_Time.year = DecToBCD((uint8_t)temp_time.year);//year
	Global_Time.week  = get_day_of_week(Global_Time);
}

/************************************************* 
@Description:�������
@Input:��
@Output:���������տ�ʼ����0��ʼ���������������ն�Ӧ0����������Ӧ6
@Return:��
*************************************************/ 
DAY_OF_WEEK get_day_of_week(rtc_typedef RTCTime)
{
	RTCTime.day = BCDToDec(RTCTime.day);
	RTCTime.month = BCDToDec(RTCTime.month);
	RTCTime.year = 2000 + BCDToDec((uint8_t)RTCTime.year);
	return (DAY_OF_WEEK)(((RTCTime.day+=(RTCTime.month<3?(RTCTime.year--):(RTCTime.year-2))), (23*RTCTime.month/9+RTCTime.day+4+RTCTime.year/4-RTCTime.year/100+RTCTime.year/400) ) % 7);
}

#else 


#define RTC_Sec_Pos	0
#define RTC_Sec_Msk (0x3f<<RTC_Sec_Pos)
#define RTC_Min_Pos 6
#define RTC_Min_Msk (0x3f<<RTC_Min_Pos)
#define RTC_Hour_Pos 12
#define RTC_Hour_Msk (0x1f<<RTC_Hour_Pos)
#define RTC_Day_Pos 17
#define RTC_Day_Msk (0x1f << RTC_Day_Pos)
#define RTC_Month_Pos 22
#define RTC_Month_Msk ( 0x0f << RTC_Month_Pos )
#define RTC_Year_Pos 26
#define RTC_Year_Msk (0x1f << RTC_Year_Pos)
/************************************************* 
@Description:rtcʱ���ۼӣ�����ʱ��
@Input:��
@Output:��
@Return:��
*************************************************/ 
static uint8_t CalendarInc(uint8_t *ucByte, uint8_t ucMin, uint8_t ucMax)
{
	if(*ucByte<ucMin||*ucByte>ucMax) *ucByte=ucMin;
	if(*ucByte==ucMax)
	{
		*ucByte=ucMin;
		return 1;
	}
	(*ucByte)++;
	return 0;
}

/************************************************* 
@Description:�����ṩ���ꡢ�¼��㵱�����һ�գ�֧������,����ֻ����20xx��
@Input:��
@Output:��
@Return:��
*************************************************/ 
static uint8_t DateMaxCalc21Cn(uint8_t Yeah, uint8_t Month)
{
	uint8_t tmp1;
	switch(Month)
	{
		case 1:case 3:case 5:case 7:case 8:case 10:case 12: //31��
			tmp1 = 31;
			break;
		case 4:case 6:case 9:case 11://30��
			tmp1 = 30;
			break;
		case 2:
			if(0 == (Yeah%4))//����ݲ���������ʱ������ܱ�4�����������꣬������ƽ��
				tmp1 = 29;//����29��
			else
				tmp1 = 28;//ƽ��28��
		default:
			break;
	}			
	return tmp1;
}



/************************************************* 
@Description:ʱ�����
@Input:��
@Output:��
@Return:��
*************************************************/ 
void Calendar21Century(rtc_typedef* pRTCtime)	//20xx
{
	if(CalendarInc(&pRTCtime->sec,0,59))		//second
	{
		if(CalendarInc(&pRTCtime->min,0,59))		//minute
		{
			if(CalendarInc(&pRTCtime->hour,0,23))		//hour
			{
				CalendarInc(&pRTCtime->week,1,7);//����
				if(CalendarInc(&pRTCtime->day,1,DateMaxCalc21Cn(pRTCtime->year, pRTCtime->month)))	//date
				{
					if(CalendarInc(&pRTCtime->month,1,12))		//month
					{
						CalendarInc(&pRTCtime->year,0,99);		//year	
					}
				}
			}
		}
	}								
}
/************************************************* 
@Description:rtcʱ������
@Input:��
@Output:��
@Return:��
*************************************************/ 
void RTC_Time_Set(uint32_t RTCtime)
{
	Global_Time.sec = ((RTCtime&RTC_Sec_Msk)>>RTC_Sec_Pos);//second
	Global_Time.min = ((RTCtime&RTC_Min_Msk)>>RTC_Min_Pos);//minute
	Global_Time.hour = ((RTCtime&RTC_Hour_Msk)>>RTC_Hour_Pos);//hour
	Global_Time.day = ((RTCtime&RTC_Day_Msk)>>RTC_Day_Pos);//day
	Global_Time.month = ((RTCtime&RTC_Month_Msk)>>RTC_Month_Pos);//month
	Global_Time.year = ((RTCtime&RTC_Year_Msk)>>RTC_Year_Pos);//year
}
#endif


/************************************************* 
@Description:rtc�жϺ���
@Input:��
@Output:��
@Return:��
*************************************************/ 
void RTC0_IRQHandler(void)
{
	if(NRF_RTC0->EVENTS_COMPARE[0])
	{
		NRF_RTC0->EVENTS_COMPARE[0]=0UL;	//clear event
		NRF_RTC0->TASKS_CLEAR=1UL;	//clear count
//		rtc_update_interval();
		rtc_flag=1;
		Calendar21Century(&Global_Time);
	}
}

