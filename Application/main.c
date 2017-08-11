#include "app_init.h"
#include "nrf_delay.h"
#include "oled.h"
#include "lis3dh.h"
#include "app_key.h"
#include "rtc.h"
#include "app_radio.h"
#include "app_msg.h"
extern uint8_t rtc_flag;//定时，射频发送
extern MSG_Store_Typedef MSG_Store;//消息定义消息序列号0~7

uint8_t rtc0_cnt;//定时器计数
#define bat_chr_cycle 1 //1s采集一次
#define bat_cycle 60 //60s采集一次
extern bat_typedef battery;


#if TFN118A
/************************************************* 
@Description:电量采集
@Input:无
@Output:
@Return:无
*************************************************/ 
void Bat_Detect(void)
{
	if(battery.CHR_Flag)//充电电量采集周期
	{
		if(rtc0_cnt > bat_chr_cycle )
		{
			battery.bat_capacity = battery_check_read();
			rtc0_cnt = 0;
			if(Read_CHR)
			{
				battery.CHR_Flag  = 0; //未在充电
				battery.Bat_Full = 0;  //充满标志位清空
			}
		}
	}
	else//不充电，电量采集周期
	{
		if(rtc0_cnt> bat_cycle )
		{
			battery.bat_capacity = battery_check_read();//采集电量
			rtc0_cnt = 0;
		}
	}	
}
#endif

/************************************************* 
@Description:主函数
@Input:无
@Output:
@Return:无
*************************************************/ 
uint32_t test_i;
uint8_t test_j = 0;
int main(void)
{
	app_init();
	#if TEST
	function_test();
	#endif
	OLED_Init();
	//初始电量采集
	nrf_delay_ms(1000);
	battery.bat_capacity = battery_check_read();
	RTC_Time_Set(0);
	
	while(1)
	{
//		Key_Deal();//按键
//		Bat_Detect();//电量采集
		//1s定时
		if(rtc_flag)
		{
			rtc0_cnt++;
			rtc_flag = 0;
			test_i++;
//			if(test_i<100)
			Raio_Deal();//射频功能	
			test_j++;
			if(test_j==3)
			{
				OLED_Init();			
				OLED_SHOW();
			}
			else if(test_j==6)
			{
				OLED_DeInit();
				test_j = 0;
			}
			if(MSG_Store.New_Msg_Flag)
			{
				MSG_Store.New_Msg_Flag = 0;
				OLED_Init();
				FilleScreen(COLOR_WHITE);//点亮全屏
				
			}
		}
		__WFI();
	}
}



