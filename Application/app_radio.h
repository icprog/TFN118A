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
//��ǩ״̬��
typedef struct
{
	uint8_t State_LP_Alarm;//�͵籨�� 1:�͵�
	uint8_t State_Key_Alram;//�������� 1���������£��ϱ�10s
	uint8_t State_Mode;//ģʽ 1���ģʽ 0���ִ�ģʽ
	uint8_t State_Update_Time;//1
}TAG_STATE_Typedef;

extern void radio_pwr(uint8_t txpower);
extern void Raio_Deal(void);//��Ƶ���ڷ���
#endif


