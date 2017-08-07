#include "app_init.h"
#include "nrf_delay.h"
#include "oled.h"
#include "lis3dh.h"
#include "app_key.h"
#include "rtc.h"
#include "app_radio.h"
extern uint8_t rtc_flag;//��ʱ����Ƶ����


uint8_t rtc0_cnt;//��ʱ������
#define bat_chr_cycle 1 //1s�ɼ�һ��
#define bat_cycle 60 //60s�ɼ�һ��
extern bat_typedef battery;


#if TFN118A
/************************************************* 
@Description:�����ɼ�
@Input:��
@Output:
@Return:��
*************************************************/ 
void Bat_Detect(void)
{
	if(battery.CHR_Flag)//�������ɼ�����
	{
		if(rtc0_cnt > bat_chr_cycle )
		{
			battery.bat_capacity = battery_check_read();
			rtc0_cnt = 0;
			if(Read_CHR)
			{
				battery.CHR_Flag  = 0; //δ�ڳ��
				battery.Bat_Full = 0;  //������־λ���
			}
		}
	}
	else//����磬�����ɼ�����
	{
		if(rtc0_cnt> bat_cycle )
		{
			battery.bat_capacity = battery_check_read();//�ɼ�����
			rtc0_cnt = 0;
		}
	}	
}
#endif

/************************************************* 
@Description:������
@Input:��
@Output:
@Return:��
*************************************************/ 
uint32_t test_i;
int main(void)
{
	app_init();
	#if TEST
	function_test();
	#endif
	OLED_Init();
	//��ʼ�����ɼ�
	nrf_delay_ms(1000);
	battery.bat_capacity = battery_check_read();
	RTC_Time_Set(0);
	while(1)
	{
//		Key_Deal();//����
//		Bat_Detect();//�����ɼ�
		//1s��ʱ
		if(rtc_flag)
		{
			rtc0_cnt++;
			rtc_flag = 0;
			test_i++;
//			if(test_i<100)
				Raio_Deal();//��Ƶ����		

		}
		OLED_SHOW();
		__WFI();
	}
}



