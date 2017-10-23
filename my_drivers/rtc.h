#ifndef M_RTC_H__
#define M_RTC_H__

#include"nrf.h"
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

typedef struct
{
	uint16_t year;//20**
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t week;
}rtc_typedef;

typedef enum {
    MOn  = 1,
    Tues  = 2,
    Wed  = 3,
    Thur = 4,
    Fri  = 5,
    Sat  = 6,
    Sun  = 0
}DAY_OF_WEEK;
typedef struct
{
	uint8_t cnt;//RTC计时
	uint8_t sec_cnt;//RTC秒计时
	uint8_t sec_flag;//秒定时标志位
	uint8_t RadioSendFlag;//定时，射频发送
}Time_Cnt_Typedef;
void RTC_Time_Set(uint32_t RTCtime);
void Calendar21Century(rtc_typedef* pRTCtime);
DAY_OF_WEEK get_day_of_week(rtc_typedef RTCTime);
#endif

