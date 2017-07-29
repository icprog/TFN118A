#ifndef M_RTC_H__
#define M_RTC_H__

#include"nrf.h"

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

void RTC_Time_Set(uint32_t RTCtime,uint8_t Week);
void Calendar21Century(rtc_typedef* pRTCtime);
DAY_OF_WEEK get_day_of_week(rtc_typedef RTCTime);
#endif

