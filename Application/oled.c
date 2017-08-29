/*******************************************************************************
** 版权:		杭州檀木科技股份有限公司
** 文件名: 		iic.c
** 版本：  		1.0
** 工作环境: 	MDK-ARM 5.23
** 作者: 		cc
** 生成日期: 	2017-07-07
** 功能:		  
** 相关文件:	iic.h
** 修改日志：	
** 版权所有   
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
extern bat_typedef battery;//电池电量
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... V_Pixels	
//[1]0 1 2 3 ... V_Pixels	
//[2]0 1 2 3 ... V_Pixels	
//[3]0 1 2 3 ... V_Pixels	
//[4]0 1 2 3 ... V_Pixels	
//[5]0 1 2 3 ... V_Pixels	
//[6]0 1 2 3 ... V_Pixels	
//[7]0 1 2 3 ... V_Pixels 		   
u8 OLED_GRAM[SSD1316_LCDWIDTH][PAGE];	 

//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	u8 page_number,n;	
	for(page_number = 0;page_number<PAGE;page_number++)  
	{  
		OLED_WR_Byte (START_PAGE+page_number,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (START_LOW_BIT,OLED_CMD);     	 //设置显示位置—列低地址
		OLED_WR_Byte (START_HIGH_BIT,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<SSD1316_LCDWIDTH;n++)
		{
			OLED_WR_Byte(OLED_GRAM[n][page_number],OLED_DATA);
		}			
	}   
}		
#if OLED_MODE==1

/************************************************* 
@Description:IIC初始化 
@Input:
@Output:无
@Return:无
*************************************************/  
void OLED_IIC_Init(void)
{
	twi_master_init();
}

/************************************************* 
@Description:IIC写字节函数
	设备地址+Control byte(决定下一字节是命令0x00/数据0x40)+数据
@Input:
@Output:无
@Return:无
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
@Description:开启OLED显示，内部升压使能
@Input:
@Output:无
@Return:无
*************************************************/     
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}


/************************************************* 
@Description:关闭OLED显示
@Input:
@Output:无
@Return:无
*************************************************/     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	


/* void Column_set(unsigned char column)
{
	OLED_WR_Byte(0x10|(column>>4),OLED_CMD);
	OLED_WR_Byte(0x00|(column&0x0f),OLED_CMD);	 
}*/   			 

/************************************************* 
@Description:清屏函数
	清除整个屏幕,0X00,清除整个屏幕，0xff，点亮整个屏幕
	清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
@Input:
@Output:无
@Return:无
*************************************************/  	  
void FilleScreen(uint8_t color)
{
	u8 i,n;  
	for(i=0;i<PAGE;i++)
	{
		for(n=0;n<SSD1316_LCDWIDTH;n++)
			OLED_GRAM[n][i]=color;  
	}
		
//	OLED_Refresh_Gram();//更新显示
}

/************************************************* 
@Description:列扫描
	每隔一段时间点亮一列
@Input:
@Output:无
@Return:无
*************************************************/  	
void OLED_Column_Scan(void)
{
	u8 page_number,column_number_1,column_number_2;
	for(column_number_1=OLED_MIN;column_number_1<SSD1316_LCDWIDTH;column_number_1++)
	{
	  for(page_number=OLED_MIN;page_number<PAGE;page_number++)
	  {
		 OLED_WR_Byte(START_PAGE+page_number,OLED_CMD);
		 OLED_WR_Byte(START_HIGH_BIT,OLED_CMD);
		 OLED_WR_Byte(START_LOW_BIT,OLED_CMD);
		 for(column_number_2=OLED_MIN;column_number_2<SSD1316_LCDWIDTH;column_number_2++)
		 {
			if(column_number_2==column_number_1)
			OLED_WR_Byte(COLOR_WHITE,OLED_DATA);
			else OLED_WR_Byte(COLOR_BLACK,OLED_DATA);
		 }
	  } 
//	  nrf_delay_ms(500);
	}
}

/************************************************* 
@Description:画点函数
	x:0~Column-1
	y:0~Row-1
	t:1 填充 0,清空		
@Input:
@Output:无
@Return:无
*************************************************/  
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>(SSD1316_LCDWIDTH-1)||y>(SSD1316_LCDHEIGHT-1))return;//超出范围了 
	pos=(PAGE-1)-(y>>3);//7-y/8->3-Y/8 计算所在PAGE页
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}


/************************************************* 
@Description:区域对角填充
	x1、x2:0~V_Pixels-1    x1<x2
	y1、y2:0~H_Pixels-1	   yi<y2
	t:1 填充 0,清空		
@Input:
@Output:无
@Return:无
*************************************************/  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
//	OLED_Refresh_Gram();//更新显示
}


/************************************************* 
@Description:对角填充
	x1、x2:0~V_Pixels-1    x1<x2
	y1、y2:0~H_Pixels-1	   yi<y2
	t:1 填充 0,清空		
@Input:
@Output:无
@Return:无
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
@Description:显示字符-水平显示
x,y为字符左上角
	x:0~V_Pixels-1  0~127
	y:0~H_Pixels-1	0~31
	size:选择字体 12*6/16*8/24*12 
	chr:字符
	mode:0,反白显示;1,正常显示				 
	csize 不足8个点补满8个点，例如字体 12*6 ,（1+1）*6共12字节  24*12,(3)*12
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
@Input:
@Output:无
@Return:无
*************************************************/  
#ifdef horizontal
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	const u8 *addr;//对应的点阵起始地址
	u8 temp;//*addr
	u8 t=0;//总字节数
	u8 t1;
	u8 y0=y;
	u8 csize;//字体对应的字节数
	chr=chr-' ';//得到偏移后的值,因为字符集是从" "开始	
	switch(size)
	{
		case ascii_1206:csize = 12; addr=&asc2_1206[chr][t];break;//调用1206字体
		case ascii_1608:csize = 16; addr=&asc2_1608[chr][t];break;//调用1608字体
		case ascii_2412:csize = 36; addr=&asc2_2412[chr][t];break;//调用2412字体
		default:return;//没有的字库
	} 
	for(t=0;t<csize;t++)//总字节数
	{   		
		temp = *addr;//获取字节
		for(t1=0;t1<8;t1++)//画点
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)//先画垂直的点
			{
				y=y0;
				x++;
				break;
			}
		}
		addr++;
	}          
}
#else

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	const u8 *addr;//对应的点阵起始地址
	u8 temp;//*addr
	u8 t=0;//总字节数
	u8 t1;
	u8 x0 = x;
	u8 csize;//字体对应的字节数
	chr=chr-' ';//得到偏移后的值,因为字符集是从" "开始	
	switch(size)
	{
		case ascii_1206:csize = 12; addr=&asc2_1206[chr][t];break;//调用1206字体
		case ascii_1608:csize = 16; addr=&asc2_1608[chr][t];break;//调用1608字体
//		case ascii_1616:csize = 32; addr=&asc2_1616[chr][t];size = 16;break;//调用1616字体
//		case ascii_2412:csize = 36; addr=&asc2_2412[chr][t];break;//调用2412字体
		default:return;//没有的字库
	} 
	for(t=0;t<csize;t++)//总字节数
	{   		
		temp = *addr;//获取字节
		for(t1=0;t1<8;t1++)//画点
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			x++;
			if((x-x0) == size)
			{
				x=x0;
				y++;
				break;
			}
		}
		addr++;
	}          
}

/************************************************* 
@Description:显示字符串
	x,y:起点坐标 
	p:字符串起始地址
	size:字体大小
@Input:
@Output:无
@Return:无
*************************************************/  
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(SSD1316_LCDWIDTH-(size>>1))){x=0;y+=size/2;}
        if(y>(SSD1316_LCDHEIGHT-size/2)){y=x=0;FilleScreen(COLOR_BLACK);}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size;
        p++;
    }  
	
}	
#endif
//void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
//{      			    
//	u8 temp,t,t1;
//	u8 y0=y;
//	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
//	chr=chr-' ';//得到偏移后的值,因为字符集是从" "开始		 
//	for(t=0;t<csize;t++)
//	{   
//		if(size==12)temp=asc2_1206[chr][t]; 	 	//调用1206字体
//		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
//		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
//		else return;								//没有的字库
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
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  

/************************************************* 
@Description:显示数字
	x:0~V_Pixels-1  0~127
	y:0~H_Pixels-1	0~31
	num:数值(0~4294967295);
	len :数字的位数
	size:字体大小，ascii_1206，ascii_1608，ascii_2412
@Input:
@Output:无
@Return:无
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
@Description:显示时钟、星期、月份
	x,y:起点坐标 
	glass_h:符号高度
	byte总字节数
@Input:
@Output:无
@Return:无
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
@Description:显示时钟、星期、月份
	x,y:起点坐标 
	rtc:rtc
@Input:
@Output:无
@Return:无
*************************************************/ 
#define hour_min_high  32   //字高
#define hour_min_bytes 64  //点阵16*32  4*16
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
	//周
	symbol_l = rtc.week;x0 += hour_min_high;
	OLED_Show_Symbol(x0,y,week_high,week_bytes,&rtc_week[7][0],1);//周
	OLED_Show_Symbol(x0,y+16,week_high,week_bytes,&rtc_week[symbol_l][0],1);//"一~日" -low 4bits	
	//month
	symbol_h = rtc.month>>4;symbol_l = rtc.month&0x0f;x0 += week_high;
	OLED_Show_Symbol(x0,y+2,mon_day_high,mon_day_byte,&rtc_mon_day[symbol_h][0],1);//mon -high 4bits
	OLED_Show_Symbol(x0,y+8,mon_day_high,mon_day_byte,&rtc_mon_day[symbol_l][0],1);//mon -low 4bits	
	
	OLED_Show_Symbol(x0,y+14,mon_day_high,mon_day_byte,&rtc_mon_day[10][0],1);//"/"

	symbol_h = rtc.day>>4;symbol_l = rtc.day&0x0f;
	OLED_Show_Symbol(x0,y+20,mon_day_high,mon_day_byte,&rtc_mon_day[symbol_h][0],1);//day -high 4bits
	OLED_Show_Symbol(x0,y+26,mon_day_high,mon_day_byte,&rtc_mon_day[symbol_l][0],1);//day -low 4bits	
}


/************************************************* 
@Description:电池电量
	x,y:起点坐标 
	bat.bat_capacity：0~4 对应着0格-4格
@Input:
@Output:无
@Return:无
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
@Description:充电指示
	x,y:起点坐标 
	mode：1
@Input:
@Output:无
@Return:无
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
@Description:显示消息
x,y:中文字左上角
	x,y:起点坐标 
	mode：1
@Input:
@Output:无
@Return:无
字库数据排列竖置横排
byte 	0	1	~~~	15			
		b0	b0	~~~	b0
		~	~	~~~ ~
		b7	b7	~~~	b7
byte    16	17		31
		b0	b0	~~~	b0
		~	~	~~~ ~
		b7	b7	~~~	b7
*************************************************/ 

void OLED_Show_Chinese(u8 x,u8 y,unsigned int code,u8 mode)
{
	uint8_t x0,y0;
	uint8_t t,t1;
	uint8_t temp;//临时缓存
	uint8_t DZ_Data[32];
	hzbmp16(SEL_GB, code, 0, 16,DZ_Data);//读取GB18030编码的点阵数据
	//显示上半部分
	x0 = x;
	y0 = y;	
	for(t=0;t<16;t++)
	{   
		temp = DZ_Data[t];
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			x++;
			if((x-x0) == 8)
			{
				x=x0;
				y++;
				break;
			}
		}  	 
	}  
	x0 = x0+8;
	y0 = y0;
	x = x0;
	y = y0;
	//显示下半部分
	for(t=16;t<32;t++)
	{   
		temp = DZ_Data[t];
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			x++;
			if((x-x0) == 8)
			{
				x=x0;
				y++;
				break;
			}
		}  	 
	}  
}

/************************************************* 
@Description:显示字符串
	x,y:起点坐标 
	p:字符串起始地址
	num:汉子个数
@Input:
@Output:无
@Return:无
*************************************************/  
void OLED_Show_ChineseS(u8 x,u8 y,const u16 *p,u8 num)
{	
	uint8_t i=0;
    for(i=0;i<num;i++)
    {       
        if(x>(SSD1316_LCDWIDTH-(16>>1)))//切换到第二列
		{
			x=0;
			y=y+16;
		}
        if(y>(SSD1316_LCDHEIGHT-16))
		{
			y=x=0;
			FilleScreen(COLOR_BLACK);
		}
        OLED_Show_Chinese(x,y,*p,1);	 
        x+=16;
        p++;
    }  
}	

/************************************************* 
@Description:显示消息
	x,y:起点坐标 
	mode：1
@Input:
@Output:无
@Return:无
*************************************************/ 
#define ASCII_RANGE 0X7F
#define Chinese_Width 16
const uint16_t Notice_Buf[2] = {0xCDA8,0XD6AA};//通知
extern GPIO_IntSource_Typedef GPIO_IntSource;//GPIO中断来源
void OLED_SHOW_MSG(u8 x,u8 y,uint8_t *pBuff)
{
	uint8_t i;
	uint8_t x0,y0,i0;
	uint8_t show_state=0;//1:显示过
	unsigned int code;//gb18030
	uint8_t Msg_Len = pBuff[1];
	FilleScreen(COLOR_BLACK);
	OLED_Show_ChineseS(0,0,Notice_Buf,2);
	x0 = x;
	y0 = y;
	pBuff = &pBuff[2];
	for(i=0;i<Msg_Len;)
	{
		if(pBuff[i] < ASCII_RANGE)//ASCII
		{
//			OLED_ShowChar(x,y,' ',ascii_1608,1);
//			OLED_ShowChar(x,y+8,' ',ascii_1608,1);
			OLED_Fill(x,y,x+15,y+15,0);
			OLED_ShowChar(x,y+4,pBuff[i],ascii_1608,1);		
			i++;//下个字
		}
		else //gb18030
		{
			code = (pBuff[i]<<8|pBuff[i+1]);
			OLED_Show_Chinese(x,y,code,1);
			i=i+2;
		}

		
		x = x + Chinese_Width;//下个字起始坐标
		//滚动效果
		if(x == x0+16)//记录每页第二个字
		{
			i0 = i;
		}
		if(x >= SSD1316_LCDWIDTH)//大于等于最大，第二个字移到X0，Y0开始的起始坐标
		{
			x = x0;
			y = y0;
			i = i0;
			nrf_delay_ms(200);
			OLED_Refresh_Gram();
			show_state = 1;
		}
		if(i >= Msg_Len|| 1 == GPIO_IntSource.Key_Int) //显示完或者按键事件发生
			break;
	}
	if(0 == show_state)
	{
		nrf_delay_ms(200);
		OLED_Refresh_Gram();
	}
}
//void OLED_SHOW_MSG(u8 x,u8 y,uint8_t *pBuff)
//{
//	uint8_t i;
//	uint8_t x0,y0,i0;
//	unsigned int code;//gb18030
//	uint8_t Msg_Len = pBuff[1];
//	FilleScreen(COLOR_BLACK);
//	OLED_Show_ChineseS(0,0,Notice_Buf,2);
//	x0 = x;
//	y0 = y;
//	pBuff = &pBuff[2];
//	for(i=0;i<Msg_Len;)
//	{
//		if(pBuff[i] < ASCII_RANGE)//ASCII
//		{
//			OLED_ShowChar(x,y,pBuff[i],ascii_1608,1);
//			i++;//下个字
//		}
//		else //gb18030
//		{
//			
//			code = (pBuff[i]<<8|pBuff[i+1]);
//			OLED_Show_Chinese(x,y,code,1);
//			i=i+2;
//		}
//		if(i > Msg_Len) 
//			break;
//		x = x + Chinese_Width;//下个字起始坐标
//		//滚动效果
//		if(x == 16)//记录每页第二个字
//		{
//			i0 = i;
//		}
//		if(x >= SSD1316_LCDWIDTH)//大于等于最大，第二个字移到X0，Y0开始的起始坐标
//		{
//			x = x0;
//			y = y0;
//			i = i0;
//			OLED_Refresh_Gram();
//		}
//	}
//}
/************************************************* 
@Description:清屏函数
	清除整个屏幕,0X00,清除整个屏幕，0xff，点亮整个屏幕
@Input:
@Output:无
@Return:无
*************************************************/ 
void OLED_IO_Init(void)
{
	//电源口初始化
	NRF_GPIO->PIN_CNF[OLED_PWR_Pin_Num] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
	//复位口初始化
	NRF_GPIO->PIN_CNF[OLED_RES_Pin_Num] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
									| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
									| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
									| (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
									| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
	OLED_RES_HIGH();
}


/************************************************* 
@Description:OLED初始化
@Input:
@Output:无
@Return:无
*************************************************/ 	
uint8_t OLED_Power_Flag = 0;//OLED是否上电
void OLED_Init(void)
{ 	 				 	 					    
	//内部Charge流程 vdd上电->VBAT上电->RES拉低3us->RES拉高3us->寄存器配置
	if(0 == OLED_Power_Flag)
	{
	#if OLED_MODE==1
		OLED_IIC_Init();
		OLED_IO_Init();
		
		OLED_VDD_BAT_ON();//VDD、VBAT上电					
		nrf_delay_us(10);
		OLED_RES_LOW();	//复位
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
		GT24L24A2Y_Spi_Init();
		OLED_Power_Flag = 1;
	}
//	GT24L24A2Y_Test();
	
}  

/************************************************* 
@Description:OLED测试
@Input:
@Output:无
@Return:无
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

void OLED_SHOW_Clock(void)
{
	//电量显示
	FilleScreen(COLOR_BLACK);//清屏
	if(battery.CHR_Flag)
	{
		OLED_ShowBat(5,1,1,battery);//电量
		OLED_ShowCHR(5,24,1);//充电标识
	}
	else
	{
		OLED_ShowBat(5,9,1,battery);
	}
	OLED_ShowRTC(28,0,Global_Time,1);//时间显示
	OLED_Refresh_Gram();
}
/************************************************* 
@Description:OLED卸载
@Input:
@Output:无
@Return:无
*************************************************/ 
extern OLED_Typedef OLED1;//oled状态
void OLED_DeInit(void)
{
	if(1 == OLED_Power_Flag)
	{
		OLED1.OLED_PowerOn = empty_page;
		GT24L24A2Y_Spi_DeInit();
		FilleScreen(COLOR_BLACK);
		OLED_WR_Byte(0xAE,OLED_CMD);     	//Set Display Off 
		OLED_WR_Byte(0x8D,OLED_CMD);     	//DC-DC Control Mode Set 
		OLED_WR_Byte(0x10,OLED_CMD);     	//DC-DC ON/OFF Mode Set 
		
		nrf_delay_ms(100);
		OLED_VDD_BAT_OFF();
		OLED_Power_Flag = 0;
	}
}


























