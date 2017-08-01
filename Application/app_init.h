#ifndef _APP_INIT_H
#define _APP_INIT_H
#include "nrf.h"
#include "sys.h"
#include "app_key.h"
#include "radio_config.h"

//io����

//���ڶ���
#define RX_PIN_NUMBER  11    // UART RX pin number.
#define TX_PIN_NUMBER 9   // UART TX pin number.
#define CTS_PIN_NUMBER 6   // UART Clear To Send pin number. Not used if HWFC is set to false
#define RTS_PIN_NUMBER 7    // Not used if HWFC is set to false 
#define HWFC           false // UART hardware flow control


#ifdef TFN118A

	
#define Motor_Pin_Num 16
#define Motor_Run() do{NRF_GPIO->OUTSET = (1 << Motor_Pin_Num);}while(0)
#define Motor_Stop() do{NRF_GPIO->OUTCLR = (1 << Motor_Pin_Num);}while(0)

#define ADC_Pin_Num ADC_CONFIG_PSEL_AnalogInput2
//OLED
#define OLED_PWR_Pin_Num  02
#define OLED_PWR_ON() do{NRF_GPIO->OUTSET = (1 << OLED_PWR_Pin_Num);}while(0)//OLED��Դ����
#define OLED_PWR_OFF() do{NRF_GPIO->OUTCLR = (1 << OLED_PWR_Pin_Num);}while(0)//OLED��Դ�ر�
#define OLED_RES_Pin_Num  15
#define OLED_RES_LOW()	do{NRF_GPIO->OUTCLR = (1 << OLED_RES_Pin_Num);}while(0) //��λ
#define OLED_RES_HIGH()	do{NRF_GPIO->OUTSET = (1 << OLED_RES_Pin_Num);}while(0)
//���ָʾ
#define USB_CHR_Pin_Num  13
#define Read_CHR	((NRF_GPIO->IN >> USB_CHR_Pin_Num)&1)    //0:��ʾ���ڳ��
//����
#define KEY_Pin_Num		7
//#define KEY_Pin_Num		16
#define Read_KEY 	((NRF_GPIO->IN >> KEY_Pin_Num)&1)		//0����ʾ�а�������
#define Key_Read_Disable_Interrupt()  do {NRF_GPIO->PIN_CNF[KEY_Pin_Num] &= (~GPIO_PIN_CNF_SENSE_Msk); }while(0)
#define Key_Read_Enable_Interrupt() do {NRF_GPIO->PIN_CNF[KEY_Pin_Num] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);}while(0)

//�����ɼ���������
#define ZeroThreshold 725
#define OneThreshold 789     //
#define TwoThreshold 810
#define ThreeThreshold 843
#define FourThreshold 896
#define FiveThreshold 5
typedef enum
{
    bat_ZeroFourth = 0,//0��
	bat_OneFourth=1,//1��
    bat_TwoFourth=2,//2��
    bat_ThreeFourth=3,//3��
	bat_FourFourth=4//4��
} battery_typedef;

typedef struct
{
	uint8_t bat_capacity;//����
	uint8_t CHR_Flag;//1:��ʾ���ڳ��   0:��ʾδ�ڳ��
	uint8_t Bat_Full;//1:�ѳ���
}bat_typedef;
#endif

//�ж����ȼ�����
typedef enum
{
    APP_IRQ_PRIORITY_HIGHEST = 0,
    APP_IRQ_PRIORITY_HIGH    = 1,
    APP_IRQ_PRIORITY_MID     = 2,
    APP_IRQ_PRIORITY_LOW     = 3
} app_irq_priority_t;


#define RADIO_PRIORITY		APP_IRQ_PRIORITY_HIGHEST
#define RTC0_PRIORITY		APP_IRQ_PRIORITY_HIGH
#define PORT_PRIORITY    	APP_IRQ_PRIORITY_HIGH
#define RTC1_PRIORITY		APP_IRQ_PRIORITY_HIGH
#define UART0_PRIORITY     	APP_IRQ_PRIORITY_HIGH

//ext function
extern void rtc0_init(void);//1s��ʱ������ʱ�䶨ʱ����Ƶ���ڷ���
extern void rtc_update_interval(void);//�������ʱ��
extern void xosc_hfclk_start(void);//��Ƶ���ͣ���Ҫ�����ⲿ16M����
extern void xosc_hfclk_stop(void);//ֹͣ��Ƶ����ʱ���ر��ⲿ����
extern void app_init(void);//Ӳ����ʼ��
void UART_Init(void);
#ifdef TFN118A
extern u8 battery_check_read(void);
extern void motor_run_state(u8 state);//�����״̬
extern void battery_check_init(void);//�����ɼ���ʼ��
extern u8 battery_check_read(void);//�����ɼ�
void rtc1_init(void);//rtc1 ������������
void rtc1_deinit(void);
#endif

#endif

