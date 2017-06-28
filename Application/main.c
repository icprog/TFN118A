#include "app_init.h"
#include "radio_config.h"
uint8_t ucFlag_RTC;


#define RADIO_BUFFER_SIZE 32



void RTC0_IRQHandler(void)
{
	if(NRF_RTC0->EVENTS_COMPARE[0])
	{
		NRF_RTC0->EVENTS_COMPARE[0]=0UL;	//clear event
		NRF_RTC0->TASKS_CLEAR=1UL;	//clear count
		rtc_update_interval();
		ucFlag_RTC=1;
	}
}

	

/****************************************
函数：清除射频缓冲区
输入：无
输出：无
****************************************/
void ClearRadioBuffer(uint8_t* pdata)
{
	uint8_t i;
	for(i = 0;i<RADIO_BUFFER_SIZE;i++)
	{
		pdata[i] = 0;
	}
}



uint8_t tx_cnt;
int main(void)
{
		key_init();//按键初始化
		motor_init();//震动电机初始化	
		Radio_Init();//射频初始化		
		while(1)
		{
			if(ucFlag_RTC)
			{
				tx_cnt++;
				ucFlag_RTC = 0;
				
	//			radio_send();
			}
			__WFI();
		}
}


void RADIO_IRQHandler(void)
{
	if(NRF_RADIO->EVENTS_END)
	{
		NRF_RADIO->EVENTS_END=0;
		radio_disable();
//		if(NRF_RADIO->STATE==10UL)//TXIDLE
//		{
//			NRF_RADIO->EVENTS_DISABLED=0;
//			NRF_RADIO->TASKS_DISABLE =1;
//		}
	}
}
