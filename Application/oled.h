#ifndef __OLED_H
#define __OLED_H			  	 

#include "app_init.h"
#include "nrf_delay.h"
#include "stdlib.h"	 

 
 
typedef enum
{
	ascii_1206=12,
	ascii_1608=16,
	ascii_2412=24
}ASSCII_S_Typedef;

//OLEDģʽ����
//0: 4�ߴ���ģʽ
//1: iicģʽ
#define OLED_MODE 1
		    						  
//-----------------OLED�˿ڶ���----------------  
#ifdef OLED_MODE
#define OLED_Slave_Address 0x78 //������ַ
#define OLED_CMD      0			//D/C 0����һ�ֽڱ�ʾ����
#define OLED_DATA     1
#define OLED_IIC_CMD  0X00
#define OLED_IIC_DATA 0X40
#else 	
#define OLED_CS PAout(0)
#define OLED_RST  PCout(1)
#define OLED_D_C PCout(2)
#define OLED_PWR PCout(5)
#define OLED_PWR_ON() VDD_PWR = 1
#define OLED_PWR_OFF() VDD_PWR = 0
#endif


#define OLED_VDD_BAT_ON() OLED_PWR_ON()
#define OLED_VDD_BAT_OFF() OLED_PWR_OFF()
/*
SSD1316 �������128*39Dot page ��4ҳ
*/
#define SSD1316_128_32
//���½�����ԭ��Ϊ(4,0),���Ͻ�(4,31),���½�(131,0),���Ͻ�(131,63)
#if defined SSD1316_128_64
	#define SSD1316_LCDWIDTH                  	128
	#define SSD1316_LCDHEIGHT                	64
#endif
#if defined SSD1316_128_32     //���귶Χ4~131,0~32��pageҳ��4~7��
	#define SSD1316_LCDWIDTH                  	128
	#define SSD1316_LCDHEIGHT                 	32
	#define PAGE SSD1316_LCDHEIGHT/8
//#define PAGE 1
	#define START_PAGE 0xB0
	#define START_HIGH_BIT 0x10  //�и�4λ
	#define START_LOW_BIT 0x00	 //�е�4λ	
	#define STATE_MAX 0XFF
	#define STATE_MIN 0X00
	#define MIN 0
#endif


  


//OLED�����ú���
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Test(void);
void OLED_DeInit(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);	 
void OLED_SHOW(void);


/*
//ʹ��4�ߴ��нӿ�ʱʹ�� 
#define OLED_SCLK PCout(3)
#define OLED_SDIN PAout(1)
		     

#define SSD1305_SETLOWCOLUMN 0x00
#define SSD1305_SETHIGHCOLUMN 0x10
#define SSD1305_MEMORYMODE 0x20
#define SSD1305_SETCOLADDR 0x21
#define SSD1305_SETPAGEADDR 0x22
#define SSD1305_SETSTARTLINE 0x40

#define SSD1305_SETCONTRAST 0x81
#define SSD1305_SETBRIGHTNESS 0x82

#define SSD1305_SETLUT 0x91

#define SSD1305_SEGREMAP 0xA0
#define SSD1305_DISPLAYALLON_RESUME 0xA4
#define SSD1305_DISPLAYALLON 0xA5
#define SSD1305_NORMALDISPLAY 0xA6
#define SSD1305_INVERTDISPLAY 0xA7
#define SSD1305_SETMULTIPLEX 0xA8
#define SSD1305_DISPLAYDIM 0xAC
#define SSD1305_MASTERCONFIG 0xAD
#define SSD1305_DISPLAYOFF 0xAE
#define SSD1305_DISPLAYON 0xAF

#define SSD1305_SETPAGESTART 0xB0

#define SSD1305_COMSCANINC 0xC0
#define SSD1305_COMSCANDEC 0xC8
#define SSD1305_SETDISPLAYOFFSET 0xD3
#define SSD1305_SETDISPLAYCLOCKDIV 0xD5
#define SSD1305_SETAREACOLOR 0xD8
#define SSD1305_SETPRECHARGE 0xD9
#define SSD1305_SETCOMPINS 0xDA
#define SSD1305_SETVCOMLEVEL 0xDB
*/
#endif  
	 



