#ifndef _APP_RADIO_H
#define _APP_RADIO_H
#include "nrf.h"
#include "radio_config.h"
#include "app_init.h"
#include "app_var.h"


typedef enum
{
	RADIO_RUN_CONFIG_CHANNEL ,
	RADIO_RUN_DATA_CHANNEL
}RADIO_CHANNEL;

//时间更新
typedef enum
{
	Time_NoUpdate=0,
	Time_Update = 1
}Time_Update_Typedef;
//标签状态字
typedef struct
{
	uint8_t State_LP_Alarm;//低电报警 1:低电
	uint8_t State_Key_Alram;//按键报警 1：按键按下，上报10s
	uint8_t State_Mode;//模式 1：活动模式 0：仓储模式
	uint8_t State_Update_Time;//1:允许更新时间 0：不允许更新时间，每天24点置位
}TAG_STATE_Typedef;

extern void radio_pwr(uint8_t txpower);
extern void Raio_Deal(void);//射频周期发送
#endif


