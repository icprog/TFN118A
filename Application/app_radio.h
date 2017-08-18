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
	Time_Update = 1//时间需要更新
}Time_Update_Typedef;
//标签状态字
typedef struct
{
	uint8_t State_LP_Alarm;//低电报警 1:低电
	uint8_t State_Key_Alram;//按键报警 1：按键按下，上报10s
	uint8_t State_Mode;//模式 1：活动模式 0：仓储模式
	uint8_t State_Update_Time;//1:允许更新时间 0：不允许更新时间，每天24点置位
}TAG_STATE_Typedef;

//携带命令
typedef enum
{
	WithoutCmd = 0,
	WithCmd
}CMD_Typedef;
//携带接收窗
typedef enum
{
	WithoutWin = 0,
	WithWin
}WIN_Typedef;

//是否等待发送完成
typedef enum
{
	SendNoWait=0,
	SendWait=1
}Send_Wait_Typedef;
//发射次数
#define Key_Alram_Const  10
#define BaseID_Const   5
typedef struct
{
	uint8_t BaseID_TimeOut;//解码超时
	uint8_t BaseID_Cnt;//边界管理器
	uint8_t KeyAlram_Cnt;//按键发送次数
}TAG_Sned_Typedef;
extern void radio_pwr(uint8_t txpower);
extern void Raio_Deal(void);//射频周期发送
#endif


