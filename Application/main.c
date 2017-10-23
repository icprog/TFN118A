#include "app_init.h"
#include "nrf_delay.h"
#include "oled.h"
#include "lis3dh.h"
#include "app_key.h"
#include "rtc.h"
#include "app_radio.h"
#include "app_old_radio.h"
#include "app_msg.h"


extern Time_Cnt_Typedef Time_Type;//定时结构
extern MSG_Store_Typedef MSG_Store;//消息定义消息序列号0~7
extern Tag_Mode_Typedef Tag_Mode;//标签模式
extern OLD_PARA_T  OLD_PARA;//老协议参数
uint8_t rtc0_cnt;//定时器计数
#define bat_chr_cycle 1 //1s采集一次
#define bat_cycle 60 //60s采集一次
extern bat_typedef battery;
OLED_Typedef OLED1;//oled状态

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

/************************************************* 
@Description:OLED显示
@Input:无
@Output:
@Return:无
*************************************************/ 
void OLED_SHOW(void)
{
	switch(OLED1.OLED_PowerOn)
	{
		case empty_page:OLED_DeInit();break;
		case clock_page:OLED_Init();OLED_SHOW_Clock();break;
//		case msg1_page:OLED_Init();FilleScreen(COLOR_BLACK);OLED_ShowChar(0,0,'A',16,1);OLED_Refresh_Gram();break;
//		case msg2_page:OLED_Init();FilleScreen(COLOR_BLACK);OLED_ShowChar(0,0,'B',16,1);OLED_Refresh_Gram();break;
//		case msg3_page:OLED_Init();FilleScreen(COLOR_BLACK);OLED_ShowChar(0,0,'C',16,1);OLED_Refresh_Gram();break;
		case msg1_page:OLED_Init();OLED_SHOW_MSG(0,16,&MSG_Store.Tag_Msg_Buf[0][0]);break;
		case msg2_page:OLED_Init();OLED_SHOW_MSG(0,16,&MSG_Store.Tag_Msg_Buf[1][0]);break;
		case msg3_page:OLED_Init();OLED_SHOW_MSG(0,16,&MSG_Store.Tag_Msg_Buf[2][0]);break;
	}
}


/************************************************* 
@Description:标签-有消息来oled显示
@Input:无
@Output:无
@Return:无
*************************************************/ 
void TAG_Msg_OLED_Show(void)
{
	if(1 == MSG_Store.New_Msg_Flag)
	{
		MSG_Store.New_Msg_Flag = 0;
		Motor_Work();
		Tag_Message_Get();//获取消息
		OLED_Init();
		OLED_SHOW_MSG(0,16,&MSG_Store.Tag_Msg_Buf[0][0]);
		OLED1.OLED_PowerOn = msg1_page;
		OLED1.OLED_TimeCnt = 0;
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
	#ifdef Hard 
	//初始电量采集
	nrf_delay_ms(1000);
	battery.bat_capacity = battery_check_read();//读取电量
	RTC_Time_Set(0);//时间设置
	OLED_SHOW_Clock();//显示时间
	#endif
	while(1)
	{
		Key_Deal();//按键
//		OLED_SHOW();//放到按键中
		Bat_Detect();//电量采集
		if(Time_Type.sec_flag)//秒计数器
		{
			Time_Type.sec_flag = 0;
			OLD_PARA.SendCnt++;
			if(OLD_PARA.SendCnt >= old_send_interval)
			{
				OLD_PARA.SendCnt = 0;
				OLD_PARA.SendEn = 1;//插播老协议
			}
			#ifdef Hard 
			//OLED关屏
			OLED1.OLED_TimeCnt++;
			if(OLED1.OLED_TimeCnt > OLED_PowerOn_Time)
			{
				OLED_DeInit();
			}
			#endif
		}
		Tag_RadioDeal();//射频功能	
		__WFI();
	}
}



