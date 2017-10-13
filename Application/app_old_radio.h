#ifndef _APP_OLD_RADIO
#define _APP_OLD_RADIO
#include "nrf.h"
#define old_resend_times									1
#define old_send_interval                                   5
#define OLD_RADIO_TID_LENGTH 4
#define OLD_PYLOAD_XOR_IDX									0
#define OLD_TAG_ID_IDX										2//标签ID 4字节
#define OLD_TAG_STATE_IDX 									6//状态字
#define OLD_TAG_LOWPWR_Pos									0//低电 1：低电
#define OLD_TAG_LOWPWR_Msk									0x01

void Radio_Old_Period_Send(uint8_t wait_send_finish);
void Radio_Old_ReSend(uint8_t times);
#endif
