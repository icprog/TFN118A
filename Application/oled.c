/*******************************************************************************
** ��Ȩ:		����̴ľ�Ƽ��ɷ����޹�˾
** �ļ���: 		iic.c
** �汾��  		1.0
** ��������: 	MDK-ARM 5.23
** ����: 		cc
** ��������: 	2017-07-07
** ����:		  
** ����ļ�:	iic.h
** �޸���־��	
** ��Ȩ����   
*******************************************************************************/
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 

#include "app_init.h"
#include "twi_master.h"
#include "i2c.h"
#include "sys.h"
#include "GT24L24A2Y.h"
#include "rtc.h"

#include "app_init.h"
extern rtc_typedef Global_Time;
extern bat_typedef battery;//��ص���
//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... V_Pixels	
//[1]0 1 2 3 ... V_Pixels	
//[2]0 1 2 3 ... V_Pixels	
//[3]0 1 2 3 ... V_Pixels	
//[4]0 1 2 3 ... V_Pixels	
//[5]0 1 2 3 ... V_Pixels	
//[6]0 1 2 3 ... V_Pixels	
//[7]0 1 2 3 ... V_Pixels 		   
u8 OLED_GRAM[SSD1316_LCDWIDTH][PAGE];	 

//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	u8 page_number,n;	
	for(page_number = 0;page_number<PAGE;page_number++)  
	{  
		OLED_WR_Byte (START_PAGE+page_number,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (START_LOW_BIT,OLED_CMD);     	 //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (START_HIGH_BIT,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<SSD1316_LCDWIDTH;n++)
		{
			OLED_WR_Byte(OLED_GRAM[n][page_number],OLED_DATA);
		}			
	}   
}		
#if OLED_MODE==1

/************************************************* 
@Description:IIC��ʼ�� 
@Input:
@Output:��
@Return:��
*************************************************/  
void OLED_IIC_Init(void)
{
	twi_master_init();
}

/************************************************* 
@Description:IICд�ֽں���
	�豸��ַ+Control byte(������һ�ֽ�������0x00/����0x40)+����
@Input:
@Output:��
@Return:��
*************************************************/  
void OLED_WR_Byte(u8 data,u8 cmd)
{
	u8 oled_buf[2];
	oled_buf[0] = (cmd == OLED_CMD)?OLED_IIC_CMD:OLED_IIC_DATA;
	oled_buf[1] = data;
	I2C_SendDates(OLED_Slave_Address,oled_buf,2);
}

#else

#endif
	  	  
/************************************************* 
@Description:����OLED��ʾ���ڲ���ѹʹ��
@Input:
@Output:��
@Return:��
*************************************************/     
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}


/************************************************* 
@Description:�ر�OLED��ʾ
@Input:
@Output:��
@Return:��
*************************************************/     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	


/* void Column_set(unsigned char column)
{
	OLED_WR_Byte(0x10|(column>>4),OLED_CMD);
	OLED_WR_Byte(0x00|(column&0x0f),OLED_CMD);	 
}*/   			 

/************************************************* 
@Description:��������
	���������Ļ,0X00,���������Ļ��0xff������������Ļ
	��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
@Input:
@Output:��
@Return:��
*************************************************/  	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<PAGE;i++)
	{
		for(n=0;n<SSD1316_LCDWIDTH;n++)
			OLED_GRAM[n][i]=STATE_MIN;  
	}
		
	OLED_Refresh_Gram();//������ʾ
}

void OLED_Full(void)
{
	u8 i,n;  
	for(i=0;i<PAGE;i++)
	{
		for(n=0;n<SSD1316_LCDWIDTH;n++)
			OLED_GRAM[n][i]=STATE_MAX;  
	}
		
	OLED_Refresh_Gram();//������ʾ	
}

/************************************************* 
@Description:��ɨ��
	ÿ��һ��ʱ�����һ��
@Input:
@Output:��
@Return:��
*************************************************/  	
void OLED_Column_Scan(void)
{
	u8 page_number,column_number_1,column_number_2;
	for(column_number_1=MIN;column_number_1<SSD1316_LCDWIDTH;column_number_1++)
	{
	  for(page_number=MIN;page_number<PAGE;page_number++)
	  {
		 OLED_WR_Byte(START_PAGE+page_number,OLED_CMD);
		 OLED_WR_Byte(START_HIGH_BIT,OLED_CMD);
		 OLED_WR_Byte(START_LOW_BIT,OLED_CMD);
		 for(column_number_2=MIN;column_number_2<SSD1316_LCDWIDTH;column_number_2++)
		 {
			if(column_number_2==column_number_1)
			OLED_WR_Byte(STATE_MAX,OLED_DATA);
			else OLED_WR_Byte(STATE_MIN,OLED_DATA);
		 }
	  } 
//	  nrf_delay_ms(500);
	}
}

/************************************************* 
@Description:���㺯��
	x:0~Column-1
	y:0~Row-1
	t:1 ��� 0,���		
@Input:
@Output:��
@Return:��
*************************************************/  
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>(SSD1316_LCDWIDTH-1)||y>(SSD1316_LCDHEIGHT-1))return;//������Χ�� 
	pos=(PAGE-1)-(y>>3);//7-y/8->3-Y/8 ��������PAGEҳ
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}


/************************************************* 
@Description:����Խ����
	x1��x2:0~V_Pixels-1    x1<x2
	y1��y2:0~H_Pixels-1	   yi<y2
	t:1 ��� 0,���		
@Input:
@Output:��
@Return:��
*************************************************/  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
//	OLED_Refresh_Gram();//������ʾ
}


/************************************************* 
@Description:�Խ����
	x1��x2:0~V_Pixels-1    x1<x2
	y1��y2:0~H_Pixels-1	   yi<y2
	t:1 ��� 0,���		
@Input:
@Output:��
@Return:��
*************************************************/  
void OLED_FILL_A(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)
{
	u8 x,y;
	x=x1;
	for(y=y1;y<=y2;)
	{
		OLED_DrawPoint(x,y,dot);
		x++;y++;
	}
}	

/************************************************* 
@Description:��ʾ�ַ�
	x:0~V_Pixels-1  0~127
	y:0~H_Pixels-1	0~31
	size:ѡ������ 12*6/16*8/24*12 
	chr:�ַ�
	mode:0,������ʾ;1,������ʾ				 
	csize ����8���㲹��8���㣬�������� 12*6 ,��1+1��*6��12�ֽ�  24*12,(3)*12
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
@Input:
@Output:��
@Return:��
*************************************************/  
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	const u8 *addr;//��Ӧ�ĵ�����ʼ��ַ
	u8 temp;//*addr
	u8 t=0;//���ֽ���
	u8 t1;
	u8 y0=y;
	u8 csize;//�����Ӧ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ,��Ϊ�ַ����Ǵ�" "��ʼ	
	switch(size)
	{
		case ascii_1206:csize = 12; addr=&asc2_1206[chr][t];break;//����1206����
		case ascii_1608:csize = 16; addr=&asc2_1608[chr][t];break;//����1608����
		case ascii_2412:csize = 36; addr=&asc2_2412[chr][t];break;//����2412����
		default:return;//û�е��ֿ�
	} 
	for(t=0;t<csize;t++)//���ֽ���
	{   		
		temp = *addr;//��ȡ�ֽ�
		for(t1=0;t1<8;t1++)//����
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)//�Ȼ���ֱ�ĵ�
			{
				y=y0;
				x++;
				break;
			}
		}
		addr++;
	}          
}
//void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
//{      			    
//	u8 temp,t,t1;
//	u8 y0=y;
//	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
//	chr=chr-' ';//�õ�ƫ�ƺ��ֵ,��Ϊ�ַ����Ǵ�" "��ʼ		 
//	for(t=0;t<csize;t++)
//	{   
//		if(size==12)temp=asc2_1206[chr][t]; 	 	//����1206����
//		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
//		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
//		else return;								//û�е��ֿ�
//		for(t1=0;t1<8;t1++)
//		{
//			if(temp&0x80)OLED_DrawPoint(x,y,mode);
//			else OLED_DrawPoint(x,y,!mode);
//			temp<<=1;
//			y++;
//			if((y-y0)==size)
//			{
//				y=y0;
//				x++;
//				break;
//			}
//		}  	 
//	}          
//}
//m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  

/************************************************* 
@Description:��ʾ����
	x:0~V_Pixels-1  0~127
	y:0~H_Pixels-1	0~31
	num:��ֵ(0~4294967295);
	len :���ֵ�λ��
	size:�����С��ascii_1206��ascii_1608��ascii_2412
@Input:
@Output:��
@Return:��
*************************************************/  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 

/************************************************* 
@Description:��ʾ�ַ���
	x,y:������� 
	p:�ַ�����ʼ��ַ
	size:�����С
@Input:
@Output:��
@Return:��
*************************************************/  
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(SSD1316_LCDWIDTH-(size>>1))){x=0;y+=size;}
        if(y>(SSD1316_LCDHEIGHT-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	
/************************************************* 
@Description:��ʾʱ�ӡ����ڡ��·�
	x,y:������� 
	glass_h:���Ÿ߶�
	byte���ֽ���
@Input:
@Output:��
@Return:��
*************************************************/  
static void OLED_Show_Symbol(u8 x,u8 y,u8 glass_h,u8 bytes ,const u8 *p,u8 mode)
{
	u8 t,t1,temp;
	u8 x0 = x;
	for(t=0;t<bytes;t++)
	{   
		temp = *p++;
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			x++;
			if((x-x0) == glass_h)
			{
				x=x0;
				y++;
				break;
			}
		}  	 
	} 
//	return y;
}
/************************************************* 
@Description:��ʾʱ�ӡ����ڡ��·�
	x,y:������� 
	rtc:rtc
@Input:
@Output:��
@Return:��
*************************************************/ 
#define hour_min_high  32   //�ָ�
#define hour_min_bytes 64  //����16*32  4*16
#define week_high  16
#define week_bytes 32
#define mon_day_high 12
#define mon_day_byte 12
void OLED_ShowRTC(u8 x,u8 y,rtc_typedef rtc,u8 mode)
{
	u8 symbol_h,symbol_l;
	u8 x0 = x;
	//hour
	symbol_h = rtc.hour>>4;symbol_l = rtc.hour&0x0f;
	OLED_Show_Symbol(x0,y,hour_min_high,hour_min_bytes,&rtc_hour_min[symbol_h][0],1);//hour -high 4bits
	OLED_Show_Symbol(x0,y+16,hour_min_high,hour_min_bytes,&rtc_hour_min[symbol_l][0],1);//hour -low 4bits
	//min
	symbol_h = rtc.min>>4;symbol_l = rtc.min&0x0f;x0 += hour_min_high;
	OLED_Show_Symbol(x0,y,hour_min_high,hour_min_bytes,&rtc_hour_min[symbol_h][0],1);//min -high 4bits
	OLED_Show_Symbol(x0,y+16,hour_min_high,hour_min_bytes,&rtc_hour_min[symbol_l][0],1);//min -low 4bits
	//��
	symbol_l = rtc.week;x0 += hour_min_high;
	OLED_Show_Symbol(x0,y,week_high,week_bytes,&rtc_week[7][0],1);//��
	OLED_Show_Symbol(x0,y+16,week_high,week_bytes,&rtc_week[symbol_l][0],1);//"һ~��" -low 4bits	
	//month
	symbol_h = rtc.month>>4;symbol_l = rtc.month&0x0f;x0 += week_high;
	OLED_Show_Symbol(x0,y+2,mon_day_high,mon_day_byte,&rtc_mon_day[symbol_h][0],1);//mon -high 4bits
	OLED_Show_Symbol(x0,y+8,mon_day_high,mon_day_byte,&rtc_mon_day[symbol_l][0],1);//mon -low 4bits	
	
	OLED_Show_Symbol(x0,y+14,mon_day_high,mon_day_byte,&rtc_mon_day[10][0],1);//"/"

	symbol_h = rtc.day>>4;symbol_l = rtc.day&0x0f;
	OLED_Show_Symbol(x0,y+20,mon_day_high,mon_day_byte,&rtc_mon_day[symbol_h][0],1);//day -high 4bits
	OLED_Show_Symbol(x0,y+26,mon_day_high,mon_day_byte,&rtc_mon_day[symbol_l][0],1);//day -low 4bits	
}
//void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)


//void OLED_ShowClk(u8 x,u8 y,u8 mode,u8 orders)
//{
//	u8 temp,t,t1;
//	u8 x0 = x;
//	for(t=0;t<64;t++)
//	{   
//		temp = bat_view[orders][t];
//		for(t1=0;t1<8;t1++)
//		{
//			if(temp&0x80)OLED_DrawPoint(x,y,mode);
//			else OLED_DrawPoint(x,y,!mode);
//			temp<<=1;
//			x++;
//			if((x-x0) == 32)
//			{
//				x=x0;
//				y++;
//				break;
//			}
//		}  	 
//	}   
//}

/************************************************* 
@Description:��ص���
	x,y:������� 
	bat.bat_capacity��0~4 ��Ӧ��0��-4��
@Input:
@Output:��
@Return:��
*************************************************/ 
#define bat_cap_bytes 44
#define bat_cap_high  12
static void OLED_ShowBat(u8 x,u8 y,u8 mode,bat_typedef bat)
{
	u8 temp,t,t1;
	u8 x0 = x;
	if(bat.bat_capacity > 4)
		return;
	for(t=0;t<bat_cap_bytes;t++)
	{   
		temp = bat_cap[bat.bat_capacity][t];
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			x++;
			if((x-x0) == bat_cap_high)
			{
				x=x0;
				y++;
				break;
			}
		}  	 
	}   
}
/************************************************* 
@Description:���ָʾ
	x,y:������� 
	mode��1
@Input:
@Output:��
@Return:��
*************************************************/ 
#define bat_chr_bytes 16
#define bat_high 12
static void OLED_ShowCHR(u8 x,u8 y,u8 mode)
{
	u8 temp,t,t1;
	u8 x0 = x;
	for(t=0;t<bat_chr_bytes;t++)
	{   
		temp = bat_chr[0][t];
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			x++;
			if((x-x0) == bat_high)
			{
				x=x0;
				y++;
				break;
			}
		}  	 
	}  
}


/************************************************* 
@Description:��������
	���������Ļ,0X00,���������Ļ��0xff������������Ļ
@Input:
@Output:��
@Return:��
*************************************************/ 
void OLED_IO_Init(void)
{
	//��Դ�ڳ�ʼ��
	NRF_GPIO->PIN_CNF[OLED_PWR_Pin_Num] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
	//��λ�ڳ�ʼ��
	NRF_GPIO->PIN_CNF[OLED_RES_Pin_Num] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
									| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
									| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
									| (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
									| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
	OLED_RES_HIGH();
}


/************************************************* 
@Description:OLED��ʼ��
@Input:
@Output:��
@Return:��
*************************************************/ 			    
void OLED_Init(void)
{ 	 				 	 					    
	//�ڲ�Charge���� vdd�ϵ�->VBAT�ϵ�->RES����3us->RES����3us->�Ĵ�������
#if OLED_MODE==1
	OLED_IIC_Init();
	OLED_IO_Init();
	
	OLED_VDD_BAT_ON();//VDD��VBAT�ϵ�					
	nrf_delay_us(10);
	OLED_RES_LOW();	//��λ
	nrf_delay_us(10);//>3us
	OLED_RES_HIGH();
	nrf_delay_us(10);//>3us
#else					
	
#endif	
	OLED_WR_Byte(0xAE,OLED_CMD);     	//Set Display Off 
	OLED_WR_Byte(0xd5,OLED_CMD);     	//display divide ratio/osc. freq. mode	
	OLED_WR_Byte(0xc1,OLED_CMD);     	// 115HZ

	OLED_WR_Byte(0xA8,OLED_CMD);     	//multiplex ration mode: 
	OLED_WR_Byte(0x1F,OLED_CMD);

	OLED_WR_Byte(0xAD,OLED_CMD);    	//External or Internal VCOMH Selection	/External or internal IREF Selection
	OLED_WR_Byte(0x00,OLED_CMD);		// Internal VCOMH/ External	IREF

	OLED_WR_Byte(0x20,OLED_CMD);		//Set Memory Addressing Mode
	OLED_WR_Byte(0x02,OLED_CMD);		//Page Addressing Mode

	OLED_WR_Byte(0xD3,OLED_CMD);		//Set Display Offset   
	OLED_WR_Byte(0x00,OLED_CMD);

	OLED_WR_Byte(0x40,OLED_CMD);     	//Set Display Start Line 

	OLED_WR_Byte(0x8D,OLED_CMD);     	//DC-DC Control Mode Set 
	OLED_WR_Byte(0x14,OLED_CMD);     	//DC-DC ON/OFF Mode Set 

	OLED_WR_Byte(0xA0,OLED_CMD);     	//Segment Remap	 

	OLED_WR_Byte(0xC8,OLED_CMD);     	//Sst COM Output Scan Direction	

	OLED_WR_Byte(0xDA,OLED_CMD);    	//seg pads hardware: alternative	
	OLED_WR_Byte(0x12,OLED_CMD);

	OLED_WR_Byte(0x81,OLED_CMD);     	//contrast control 
	OLED_WR_Byte(0x53,OLED_CMD);		

	OLED_WR_Byte(0xD9,OLED_CMD);	    //set pre-charge period	  
	OLED_WR_Byte(0x22,OLED_CMD);

	OLED_WR_Byte(0xDB,OLED_CMD);     	//VCOM deselect level mode 
	OLED_WR_Byte(0x00,OLED_CMD);	    

	OLED_WR_Byte(0xA4,OLED_CMD);     	//Set Entire Display On/Off	

	OLED_WR_Byte(0xA6,OLED_CMD);     	//Set Normal Display 
	OLED_WR_Byte(0xAF,OLED_CMD);     	//Set Display On 	
	nrf_delay_ms(100);
	OLED_Clear();
	
}  

/************************************************* 
@Description:OLED����
@Input:
@Output:��
@Return:��
*************************************************/ 
void OLED_Test(void)
{
	OLED_DrawPoint(0,0,1);
	OLED_DrawPoint(0,31,1);
	OLED_DrawPoint(127,0,1);
	OLED_DrawPoint(127,31,1);
	
//	OLED_DrawPoint(32,63,1);
//	OLED_Fill(0,32,64,32,1);
//	OLED_ShowChar(64,0,'A',ascii_1608,1);
//	OLED_ShowBat(3,9,1,0);
	OLED_ShowRTC(28,0,Global_Time,1);
	OLED_Refresh_Gram();
	
//	GT24L24A2Y_Test();
//	OLED_ShowString(64,0,"tee",ascii_1608);
	
//	OLED_Full();
//	nrf_delay_ms(1000);
//	OLED_Clear();
//	nrf_delay_ms(1000);
//	OLED_Column_Scan();
//	OLED_Clear();
//	nrf_delay_ms(4);
}

void OLED_SHOW(void)
{
	//������ʾ
	OLED_Fill(0,0,16,31,0);//�������
	if(battery.CHR_Flag)
	{
		OLED_ShowBat(5,1,1,battery);//����
		OLED_ShowCHR(5,24,1);//����ʶ
	}
	else
	{
		OLED_ShowBat(5,9,1,battery);
	}
	OLED_ShowRTC(28,0,Global_Time,1);//ʱ����ʾ
	OLED_Refresh_Gram();
}
/************************************************* 
@Description:OLEDж��
@Input:
@Output:��
@Return:��
*************************************************/ 
void OLED_DeInit(void)
{
	OLED_WR_Byte(0xAE,OLED_CMD);     	//Set Display Off 
	OLED_WR_Byte(0x8D,OLED_CMD);     	//DC-DC Control Mode Set 
	OLED_WR_Byte(0x10,OLED_CMD);     	//DC-DC ON/OFF Mode Set 
	
	nrf_delay_ms(100);
	OLED_VDD_BAT_OFF();
}


























