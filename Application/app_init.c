
#include "app_init.h"
#include "simple_uart.h"
#include "Debug_log.h"
#define rtc_interval 1  //��λs
#define rtc_base ((32768*rtc_interval) - 1)


bat_typedef battery;//��ص���
/************************************************* 
Description:���õ�Ƶʱ��ʱ��Դ  
Input:
��1����source : 1:ѡ���ⲿ���� XOSC 0���ڲ�rc ROSC
Output:��
Return:��
*************************************************/  
static void lfclk_init(uint8_t source)
{
	uint8_t lfclksrc;
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
	//ѡ��ʱ��Դ
	lfclksrc = source ? CLOCK_LFCLKSRC_SRC_Xtal : CLOCK_LFCLKSRC_SRC_RC;
	NRF_CLOCK->LFCLKSRC = lfclksrc << CLOCK_LFCLKSRC_SRC_Pos;
	NRF_CLOCK->TASKS_LFCLKSTART = 1;
	while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) 
	{
	}	
}

/************************************************* 
Description:����0~255����� 
Input:��
Output:������ֵ
Return:��
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
@Description:rtc��ʼ��  
@Input:
@Output:��
@Return:��
*************************************************/  
void rtc0_init(void)
{
	lfclk_init(1);//1��XOSC 0��ROSC
	NRF_RTC0->PRESCALER = 0;//32.768khz Լ����0.03ms
	NRF_RTC0->CC[0] = rtc_base;//
	NRF_RTC0->EVENTS_COMPARE[0] = 0;//EVENTS_TICK
	NRF_RTC0->INTENSET =  RTC_INTENCLR_COMPARE0_Enabled<<RTC_INTENCLR_COMPARE0_Pos;//
//	NRF_RTC0->TASKS_START = 1;
	
	NVIC_SetPriority(RTC0_IRQn,RTC0_PRIORITY);
	NVIC_ClearPendingIRQ(RTC0_IRQn);
	NVIC_EnableIRQ( RTC0_IRQn );
}
/************************************************* 
@Description:rtc0��������  
@Input:
@Output:��
@Return:��
*************************************************/  
static void rtc0_start(void)
{
	NRF_RTC0->TASKS_START = 1;
}
/************************************************* 
@Description:rtc0ֹͣ����
@Input:
@Output:��
@Return:��
*************************************************/  
static void rtc0_stop(void)
{
	NRF_RTC0->TASKS_STOP = 1;
}

/************************************************* 
Description:rtc1��ʼ��  
Input:
Output:��
Return:��
*************************************************/ 
void rtc1_init(void)
{	
	NRF_RTC1->PRESCALER = 0;//32.768khz Լ����0.03ms
	NRF_RTC1->CC[0] = jitter_delay;//Խ40ms
	NRF_RTC1->EVENTS_COMPARE[0] = 0;//EVENTS_TICK
	NRF_RTC1->INTENSET =  RTC_INTENCLR_COMPARE0_Enabled<<RTC_INTENCLR_COMPARE0_Pos;//ʹ���ж�
	NRF_RTC1->TASKS_START = 1;
	
	NVIC_SetPriority(RTC1_IRQn,RTC1_PRIORITY);
	NVIC_ClearPendingIRQ(RTC1_IRQn);
	NVIC_EnableIRQ( RTC1_IRQn );	
}

/************************************************* 
Description:rtc1��ʼ��  
Input:
Output:��
Return:��
*************************************************/ 
void rtc1_deinit(void)
{	
	NRF_RTC1->TASKS_STOP = 1;
	NRF_RTC1->INTENSET &=  (~RTC_INTENCLR_COMPARE0_Msk);//�жϲ�ʹ��
	NVIC_DisableIRQ( RTC1_IRQn );	
}

/************************************************* 
Description:�㲥�������0~7.65ms�������ʱ
Input:��
Output:��
Return:��
*/
void rtc_update_interval(void)
{
	uint8_t state=0;
	uint8_t advDelay = random_vector_generate();//0~255 0~7.65ms
	if(state)
	{
		NRF_RTC0->CC[0] = rtc_base + advDelay;
		state = 0;
	}
	else
	{
		NRF_RTC0->CC[0] = rtc_base - advDelay;
		state = 1;
	}
	
}
/************************************************* 
Description:�����ⲿ����
Input:��
Output:��
Return:��
*************************************************/ 
void xosc_hfclk_start(void)
{
	/*���ⲿ����δ����ʱ���������ⲿ����*/
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
		//�ȴ�HFCLK running
		while((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk) == 0)
		{
		}
	}		
}

/************************************************* 
Description:�ر��ⲿ����
Input:��
Output:��
Return:��
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
Description:������ʼ��
Input:��
Output:��
Return:��
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
Description:�����ɼ���ʼ�� 1-1.05v
Input:��
Output:��
Return:��
*************************************************/ 
void battery_check_init(void)
{
    NRF_ADC->CONFIG = (ADC_CONFIG_RES_10bit << 0)//����10λ
                  | (0 << 2) //ADC����ֵ��������
                  | (0 << 5) //ѡ���ڲ�1.2VΪ�ο���ѹ
                  | (ADC_Pin_Num << 8);//���ò�����
 
//    NRF_ADC->ENABLE = 0x01; 
}

/************************************************* 
Description:����һ��ADC�ɼ�����ʼ���ӳ�1s�󣬿�ʼ�ɼ�
Input:��
Output:��
Return:��
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

/************************************************* 
@Description:4����ȥ�����ֵ����Сֵ��Ȼ��ȡƽ����
@Input:��
@Output:��
@Return:��
*************************************************/ 
uint16_t average(u16* data)
{
	u8 i = 0;
	u16 max =*data;
	u16 min =*data;
	u16 sum = 0;
	u16 average1 =0;
	for(i=0;i<4;i++)
	{
		if(max>*(data+i)) max = max; else max = *(data+i);
		if(min<*(data+i)) min = min; else min = *(data+i);
		sum =sum+(*(data+i));
	}
	sum-=min;
	sum-=max;
	sum = sum>>2;
	average1 = sum;
	return average1;
}

/************************************************* 
@Description:��βɼ�����,1���Ӳɼ�һ�ε����������Ƿ���øú���
@Input:��
@Output:��
@Return:����ADCֵ
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
Description:�����ɼ�
Input:��
Output:
Return:��
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
		if(Read_CHR)//������ű�ɸߵ�ƽ�����ҵ�ѹ���
			battery.Bat_Full = 1;
	}
	return bat_range;
}

/************************************************* 
Description:�ж����ų�ʼ��
Input:��
Output:
Return:��
*************************************************/ 
static void io_interrupt_config(void)
{
	//���ָʾIO������
	NRF_GPIO->PIN_CNF[USB_CHR_Pin_Num]=GPIO_PIN_CNF_SENSE_Low<<GPIO_PIN_CNF_SENSE_Pos			//low level
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);								
	//����
	NRF_GPIO->PIN_CNF[KEY_Pin_Num]=(GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos)			//low level
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
	NRF_GPIOTE->EVENTS_PORT=0UL;
	NRF_GPIOTE->INTENSET=GPIOTE_INTENSET_PORT_Enabled << GPIOTE_INTENSET_PORT_Pos;	//PORT
	NVIC_SetPriority(GPIOTE_IRQn, PORT_PRIORITY);
	NVIC_EnableIRQ(GPIOTE_IRQn);
}

/*
@Description:���ڳ�ʼ��
@Input:��
@Output:��
@Return:��
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
Description:app��ʼ��
Input:��
Output:
Return:��
*************************************************/ 
void app_init(void)
{
	#ifdef LOG_ON
	debug_log_init();
	#endif
	debug_printf("TFN118A Start��ʼ��\r\n");
	SystemParaInit();
	motor_init();//�𶯵����ʼ��	
	Radio_Init();//��Ƶ��ʼ��
	rtc0_init();//rtc��ʼ��
	io_interrupt_config();//���ָʾ������io��ʼ��
	battery_check_init();
	rtc0_start();
}
/************************************************* 
Description:IO�жϺ���
Input:��
Output:
Return:��
*************************************************/ 
uint8_t Port_IT_KEY;//����
uint8_t Port_IT_CHR;
void GPIOTE_IRQHandler(void)
{
	if(NRF_GPIOTE->EVENTS_PORT)
	{
		NRF_GPIOTE->EVENTS_PORT = 0;
		if(0 == Read_CHR)
		{
			//���ڳ��
			Port_IT_CHR++;
			battery.CHR_Flag = 1;//���ڳ��
			
		}
		if(0 == Read_KEY)//�����ж�
		{
			onKeyEvent();
			Port_IT_KEY++;
		}
	}
}


#endif


