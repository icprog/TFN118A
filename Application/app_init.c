
#include "app_init.h"
#include "simple_uart.h"
#include "Debug_log.h"
#include "nrf_delay.h"
#include "as3933.h"
#include "oled.h"
//#define rtc_interval 1  //单位s



bat_typedef battery;//电池电量
GPIO_IntSource_Typedef GPIO_IntSource = {0,0};//GPIO中断来源
extern volatile uint8_t Key_Scan_En;//按键扫描使能标志位
/************************************************* 
@Description:配置低频时钟时钟源  
@Input:
（1）、source : 1:选择外部晶振 XOSC 0：内部rc ROSC
@Output:无
@Return:无
*************************************************/  
static void lfclk_init(uint8_t source)
{
	uint8_t lfclksrc;
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
	//选择时钟源
	lfclksrc = source ? CLOCK_LFCLKSRC_SRC_Xtal : CLOCK_LFCLKSRC_SRC_RC;
	NRF_CLOCK->LFCLKSRC = lfclksrc << CLOCK_LFCLKSRC_SRC_Pos;
	NRF_CLOCK->TASKS_LFCLKSTART = 1;
	while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) 
	{
	}	
}

/************************************************* 
@Description:产生0~255随机数 
@Input:无
@Output:输出随机值
@Return:无
*************************************************/ 
//uint8_t rng_value = 0;
static uint8_t random_vector_generate()
{
	static uint8_t rng_value = 0;
	rng_value = NRF_RNG->VALUE;
	NRF_RNG->EVENTS_VALRDY = 0;
	NRF_RNG->SHORTS = RNG_SHORTS_VALRDY_STOP_Enabled << RNG_SHORTS_VALRDY_STOP_Pos;
	NRF_RNG->TASKS_START = 1;
	return rng_value;
//	NRF_RNG->EVENTS_VALRDY = 0;
//	NRF_RNG->TASKS_START = 1;
//	while(NRF_RNG->EVENTS_VALRDY == 0);
//	NRF_RNG->EVENTS_VALRDY = 1;
//	rng_value = NRF_RNG->VALUE;
//	NRF_RNG->TASKS_STOP = 1;
//	return rng_value;
}
/************************************************* 
@Description:rtc初始化  
@Input:
@Output:无
@Return:无
*************************************************/  
void rtc0_init(void)
{
	lfclk_init(1);//1：XOSC 0：ROSC
	NRF_RTC0->PRESCALER = 0;//32.768khz 约等于0.03ms
	NRF_RTC0->CC[0] = rtc_base;//
	NRF_RTC0->EVENTS_COMPARE[0] = 0;//EVENTS_TICK
	NRF_RTC0->INTENSET =  RTC_INTENCLR_COMPARE0_Enabled<<RTC_INTENCLR_COMPARE0_Pos;//
//	NRF_RTC0->TASKS_START = 1;
	
	NVIC_SetPriority(RTC0_IRQn,RTC0_PRIORITY);
	NVIC_ClearPendingIRQ(RTC0_IRQn);
	NVIC_EnableIRQ( RTC0_IRQn );
}
/************************************************* 
@Description:rtc0启动计数  
@Input:
@Output:无
@Return:无
*************************************************/  
static void rtc0_start(void)
{
	NRF_RTC0->TASKS_START = 1;
}
/************************************************* 
@Description:rtc0停止计数
@Input:
@Output:无
@Return:无
*************************************************/  
static void rtc0_stop(void)
{
	NRF_RTC0->TASKS_STOP = 1;
}

/************************************************* 
@Description:rtc1初始化  
@Input:
@Output:无
@Return:无
*************************************************/ 
void rtc1_init(void)
{	
	NRF_RTC1->PRESCALER = 0;//32.768khz 约等于0.03ms
	NRF_RTC1->CC[0] = jitter_delay;//越40ms
	NRF_RTC1->EVENTS_COMPARE[0] = 0;//EVENTS_TICK
	NRF_RTC1->INTENSET =  RTC_INTENCLR_COMPARE0_Enabled<<RTC_INTENCLR_COMPARE0_Pos;//使能中断
	NRF_RTC1->TASKS_START = 1;
	
	NVIC_SetPriority(RTC1_IRQn,RTC1_PRIORITY);
	NVIC_ClearPendingIRQ(RTC1_IRQn);
	NVIC_EnableIRQ( RTC1_IRQn );	
}

/************************************************* 
@Description:rtc1初始化  
@Input:
@Output:无
@Return:无
*************************************************/ 
void rtc1_deinit(void)
{	
	NRF_RTC1->TASKS_STOP = 1;
	NRF_RTC1->INTENCLR = RTC_INTENCLR_COMPARE0_Msk;//中断不使能
	NVIC_DisableIRQ( RTC1_IRQn );	
}

/************************************************* 
@Description:广播间隔增加0~7.65ms的随机延时
@Input:无
@Output:无
@Return:无
*/
uint16_t old_period = 0;//上一个周期
uint16_t new_period;
uint8_t advDelay = 0;//随机数
//uint8_t oldDelay;//旧随机数
uint16_t new_rtc_base;
void rtc_update_interval(void)
{
	new_rtc_base = rtc_base - advDelay;//减去随机数
	advDelay = random_vector_generate();//0~255 0~7.65ms
	NRF_RTC0->CC[0] = new_rtc_base + advDelay;//每次与标准时间只相差随机数
}
/************************************************* 
@Description:启动外部晶振
@Input:无
@Output:无
@Return:无
*************************************************/ 
void xosc_hfclk_start(void)
{
	/*当外部晶振未启动时，才启动外部晶振*/
	if((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_SRC_Xtal)!=1)
	{
		/*clear event*/
		NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
		/* Start 16 MHz crystal oscillator */		
		NRF_CLOCK->TASKS_HFCLKSTART = 1;

		/* Wait for the external oscillator to start up */
		while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) 
		{
			
		}
		//等待HFCLK running
		while((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk) == 0)
		{
		}
	}		
}

/************************************************* 
@Description:关闭外部晶振
@Input:无
@Output:无
@Return:无
*************************************************/ 
void xosc_hfclk_stop(void)
{
	/**/
	if((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_SRC_Xtal) == 1)
	{
		NRF_CLOCK->TASKS_HFCLKSTOP = 1;
	}		
}

#ifdef TFN118A
/************************************************* 
@Description:震动马达初始化
@Input:无
@Output:无
@Return:无
*************************************************/ 
static void motor_init(void)
{
	NRF_GPIO->PIN_CNF[Motor_Pin_Num] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);	
}


/************************************************* 
@Description:电量采集初始化 1-1.05v
@Input:无
@Output:无
@Return:无
*************************************************/ 
void battery_check_init(void)
{
    NRF_ADC->CONFIG = (ADC_CONFIG_RES_10bit << 0)//精度10位
                  | (0 << 2) //ADC测量值等于输入
                  | (0 << 5) //选择内部1.2V为参考电压
                  | (ADC_Pin_Num << 8);//配置采样脚
 
//    NRF_ADC->ENABLE = 0x01; 
}

/************************************************* 
@Description:启动一次ADC采集，初始化延迟1s后，开始采集
@Input:无
@Output:无
@Return:无
*************************************************/ 
static uint16_t adc_convert_single(void)
{
	uint16_t val;
	NRF_ADC->ENABLE = 1;
	if(0 == NRF_ADC->BUSY)
	{
		NRF_ADC->TASKS_START = 1;
		while(0 == NRF_ADC->EVENTS_END);
		NRF_ADC->EVENTS_END = 0;
		val = (uint16_t)NRF_ADC->RESULT;
		NRF_ADC->TASKS_STOP = 1;
	}
	NRF_ADC->ENABLE = 0;
	return val;
}

///************************************************* 
//@Description:4个数去掉最大值，最小值，然后取平均。
//@Input:无
//@Output:无
//@Return:无
//*************************************************/ 
//uint16_t average(u16* data)
//{
//	u8 i = 0;
//	u16 max =*data;
//	u16 min =*data;
//	u16 sum = 0;
//	u16 average1 =0;
//	for(i=0;i<4;i++)
//	{
//		if(max>*(data+i)) max = max; else max = *(data+i);
//		if(min<*(data+i)) min = min; else min = *(data+i);
//		sum =sum+(*(data+i));
//	}
//	sum-=min;
//	sum-=max;
//	sum = sum>>2;
//	average1 = sum;
//	return average1;
//}

/************************************************* 
@Description:多次采集电量,1分钟采集一次电量，考虑是否调用该函数
@Input:无
@Output:无
@Return:返回ADC值
*************************************************/ 
uint16_t adc_convert_times(void)
{
	uint8_t i;
	uint16_t val;
	uint16_t adc_tmp[4];
	NRF_ADC->ENABLE = 1;
	for(i=0;i<4;i++)
	{
		if(0 == NRF_ADC->BUSY)
		{
			NRF_ADC->TASKS_START = 1;
			while(0 == NRF_ADC->EVENTS_END);
			NRF_ADC->EVENTS_END = 0;
			adc_tmp[i] = (uint16_t)NRF_ADC->RESULT;
			NRF_ADC->TASKS_STOP = 1;
		}
	}
	NRF_ADC->ENABLE = 0;
	val = average(adc_tmp);
	return val;
}
/************************************************* 
@Description:电量采集
@Input:无
@Output:
@Return:无
*************************************************/ 
uint16_t adc_value;
u8 battery_check_read(void)
{
	uint16_t bat_range;
	adc_value = adc_convert_single();
	if(adc_value < ZeroThreshold)
	{
		bat_range = bat_ZeroFourth;
	}
	else if(adc_value < OneThreshold )
	{
		bat_range = bat_OneFourth;
	}
	else if(adc_value < TwoThreshold)
	{
		bat_range = bat_TwoFourth;
	}
	else if(adc_value < ThreeThreshold)
	{
		bat_range = bat_ThreeFourth;
	}
	else if(adc_value < FourThreshold)
	{
		bat_range = bat_FourFourth;
	}
	else
	{
		bat_range = bat_FourFourth;
		if(Read_CHR)//充电引脚变成高电平，并且电压最大
			battery.Bat_Full = 1;
	}
	return bat_range;
}

/************************************************* 
@Description:中断引脚初始化
@Input:无
@Output:
@Return:无
*************************************************/ 
static void io_interrupt_config(void)
{
//	//充电指示IO口设置
//	NRF_GPIO->PIN_CNF[USB_CHR_Pin_Num]=GPIO_PIN_CNF_SENSE_Low<<GPIO_PIN_CNF_SENSE_Pos			//low level
//										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
//                                        | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
//                                        | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
//                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);	
NRF_GPIO->PIN_CNF[USB_CHR_Pin_Num] = IO_INPUT;	
	//按键
	NRF_GPIO->PIN_CNF[KEY_Pin_Num]=(GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos)			//low level
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
	NRF_GPIOTE->EVENTS_PORT=0UL;
	NRF_GPIOTE->INTENSET =GPIOTE_INTENSET_PORT_Enabled << GPIOTE_INTENSET_PORT_Pos;	//PORT
	NVIC_SetPriority(GPIOTE_IRQn, PORT_PRIORITY);
	NVIC_EnableIRQ(GPIOTE_IRQn);
}

/*
@Description:串口初始化
@Input:无
@Output:无
@Return:无
*/
void UART_Init(void)
{
    simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);  
	NRF_UART0->INTENSET = (UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos)
						|(UART_INTENSET_ERROR_Enabled << UART_INTENSET_ERROR_Pos);
	NVIC_SetPriority(UART0_IRQn, UART0_PRIORITY);
    NVIC_EnableIRQ(UART0_IRQn);
}

/************************************************* 
@Description:app初始化
@Input:无
@Output:
@Return:无
*************************************************/ 
void app_init(void)
{
	#ifdef LOG_ON
	debug_log_init();
	#endif
	debug_printf("TFN118A Start初始化\r\n");
	SystemParaInit();
//	as3933_Init();
	OLED_Init();
	motor_init();//震动电机初始化	
	Radio_Init();//射频初始化
	rtc0_init();//rtc初始化
	io_interrupt_config();//充电指示、按键io初始化
	battery_check_init();
	rtc0_start();
	
}
/************************************************* 
@Description:电机震动下
@Input:无
@Output:
@Return:无
*************************************************/ 
void Motor_Work(void)
{
	Motor_Run();//电机测试
	nrf_delay_ms(500);
	Motor_Stop();
}
/************************************************* 
@Description:IO中断函数,下降沿
@Input:无
@Output:
@Return:无
*************************************************/ 
uint8_t Port_IT_KEY;//按键
uint8_t Port_IT_CHR;
void GPIOTE_IRQHandler(void)
{
	if(NRF_GPIOTE->EVENTS_PORT)
	{
		NRF_GPIOTE->EVENTS_PORT = 0;
		if(0 == Read_CHR)
		{
			//正在充电
			Port_IT_CHR++;
			battery.CHR_Flag = 1;//正在充电
			
		}
		if(0 == Read_KEY)//按键中断
		{
//			nrf_delay_ms(10);
			onKeyEvent();
			Port_IT_KEY++;
			GPIO_IntSource.Key_Int = 1;
		}
//		as3933_wakeupIsr();
	}
	else if(NRF_GPIOTE->EVENTS_IN[0])
	{
		NRF_GPIOTE->EVENTS_IN[0] = 0;
	}
}


/************************************************* 
@Description:消抖计时器
@Input:无
@Output:无
@Return:无
*************************************************/ 

void RTC1_IRQHandler(void)
{
	if(NRF_RTC1->EVENTS_COMPARE[0])
	{
		NRF_RTC1->EVENTS_COMPARE[0]=0UL;	//clear event
		NRF_RTC1->TASKS_CLEAR=1UL;	//clear count
		if(GPIO_IntSource.Key_Int)//按键扫描
		{
			Key_Scan_En = 1;//按键扫描使能			
		}
		if(GPIO_IntSource.AS3933_Wake_Int)//wakeup中断
		{
			as3933_TimeOut();
		}
	}
}

#endif


