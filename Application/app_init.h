#ifndef _APP_INIT_H
#define _APP_INIT_H
#include "nrf.h"
#include "sys.h"
#include "app_key.h"
#include "radio_config.h"

//io定义

//串口定义
#define RX_PIN_NUMBER  11    // UART RX pin number.
#define TX_PIN_NUMBER 9   // UART TX pin number.
#define CTS_PIN_NUMBER 6   // UART Clear To Send pin number. Not used if HWFC is set to false
#define RTS_PIN_NUMBER 7    // Not used if HWFC is set to false 
#define HWFC           false // UART hardware flow control


#ifdef TFN118A

#define IO_LP_State 0x00000002	
#define IO_OUTPUT  	0X00000003
#define IO_INPUT  	0X00000000
#define IO_INPUT_Pulldown  	0X00000004
#define IO_INPUT_Pullup  	0X0000000C
#define Motor_Pin_Num 13
#define Motor_Run() do{NRF_GPIO->OUTSET = (1 << Motor_Pin_Num);}while(0)
#define Motor_Stop() do{NRF_GPIO->OUTCLR = (1 << Motor_Pin_Num);}while(0)

#define ADC_Pin_Num ADC_CONFIG_PSEL_AnalogInput7
//OLED
#define OLED_PWR_Pin_Num  14 
#define OLED_PWR_ON() do{NRF_GPIO->OUTSET = (1 << OLED_PWR_Pin_Num);}while(0)//OLED电源开启
#define OLED_PWR_OFF() do{NRF_GPIO->OUTCLR = (1 << OLED_PWR_Pin_Num);}while(0)//OLED电源关闭
#define OLED_RES_Pin_Num  8
#define OLED_RES_LOW()	do{NRF_GPIO->OUTCLR = (1 << OLED_RES_Pin_Num);}while(0) //复位
#define OLED_RES_HIGH()	do{NRF_GPIO->OUTSET = (1 << OLED_RES_Pin_Num);}while(0)
//充电指示
#define USB_CHR_Pin_Num  9
#define Read_CHR	((NRF_GPIO->IN >> USB_CHR_Pin_Num)&1)    //0:表示正在充电
//按键
#define KEY_Pin_Num		25
//#define KEY_Pin_Num		16
#define Read_KEY 	((NRF_GPIO->IN >> KEY_Pin_Num)&1)		//0：表示有按键按下
#define Key_Read_Disable_Interrupt()  do {NRF_GPIO->PIN_CNF[KEY_Pin_Num] &= (~GPIO_PIN_CNF_SENSE_Msk); }while(0)
#define Key_Read_Enable_Interrupt() do {NRF_GPIO->PIN_CNF[KEY_Pin_Num] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);}while(0)

//其它引脚，twi_master_config.h\spi_master_config.h -spi0文件中
//电量采集参数定义
#define ZeroThreshold 725
#define OneThreshold 789     //
#define TwoThreshold 810
#define ThreeThreshold 843
#define FourThreshold 896
#define FiveThreshold 5
typedef enum
{
    bat_ZeroFourth = 0,//0格
	bat_OneFourth=1,//1格
    bat_TwoFourth=2,//2格
    bat_ThreeFourth=3,//3格
	bat_FourFourth=4//4格
} battery_typedef;

typedef struct
{
	uint8_t bat_capacity;//电量
	uint8_t CHR_Flag;//1:表示正在充电   0:表示未在充电
	uint8_t Bat_Full;//1:已充满
}bat_typedef;
#endif
//OLED显示
#define OLED_PowerOn_Time 5  //oled亮4s
typedef struct
{
	uint8_t OLED_PowerOn;//1:表示OLED亮
	uint8_t OLED_TimeCnt;//计数值
	uint8_t key_short_cnt;//按键短按次数
}OLED_Typedef;
typedef enum
{
	empty_page=0,
	clock_page=1,
	msg1_page=2,
	msg2_page=3,
	msg3_page=4
}OLED_Page_Typedef;
//中断优先级定义
typedef enum
{
    APP_IRQ_PRIORITY_HIGHEST = 0,
    APP_IRQ_PRIORITY_HIGH    = 1,
    APP_IRQ_PRIORITY_MID     = 2,
    APP_IRQ_PRIORITY_LOW     = 3
} app_irq_priority_t;
//GPIO中断事件发生
typedef struct
{
	uint8_t Key_Int;//按键中断
	uint8_t AS3933_Wake_Int;//3933中断
}GPIO_IntSource_Typedef;


#define RADIO_PRIORITY		APP_IRQ_PRIORITY_HIGHEST
#define RTC0_PRIORITY		APP_IRQ_PRIORITY_HIGH
#define PORT_PRIORITY    	APP_IRQ_PRIORITY_HIGH
#define RTC1_PRIORITY		APP_IRQ_PRIORITY_HIGH
#define UART0_PRIORITY     	APP_IRQ_PRIORITY_HIGH

//ext function
extern void rtc0_init(void);//1s定时，用来时间定时和射频周期发送
extern void rtc_update_interval(void);//增加随机时间
extern void xosc_hfclk_start(void);//射频发送，需要启动外部16M晶振
extern void xosc_hfclk_stop(void);//停止射频发送时，关闭外部晶振
extern void app_init(void);//硬件初始化
void UART_Init(void);
#ifdef TFN118A
extern u8 battery_check_read(void);
extern void battery_check_init(void);//电量采集初始化
extern u8 battery_check_read(void);//电量采集
void rtc1_init(void);//rtc1 用来按键消抖
void rtc1_deinit(void);
void Motor_Work(void);
#endif

#endif

