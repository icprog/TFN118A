//#include <A2Ylib.h>
//��������������������������������������������
//��         ��ͨ�Ƽ���Ȩ����  ��
//��      GENITOP RESEARCH CO.,LTD.         ��
//��        created on 2015.7.27            ��
//��������������������������������������������
#ifndef HFMA2Yliib_h
#define HFMA2Yliib_h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys.h"
//#include "flash.h"

typedef  unsigned char  u8;
//typedef  unsigned long  u32;
typedef  unsigned short  u16;
typedef  unsigned long   ulong;
typedef  unsigned char   uchar;
typedef  unsigned char   BYTE;
typedef  unsigned short  WORD;
typedef  unsigned long   DWORD;

static DWORD hfmbdd;

extern void r_dat_bat(unsigned long TAB_addr,unsigned int Num,unsigned char *p_arr);



#define ASCII_5X7              1      //ASCII����5X7����
#define ASCII_7X8              2      //ASCII����7X8����
#define ASCII_6X12             3      //ASCII����6X12����
#define ASCII_12_B_A           4      //ASCII����12x12���󲻵ȿ�Arial����ַ����������ο�����飩
#define ASCII_12_B_T           5      //ASCII����12x12���󲻵ȿ�Time news Roman����ַ����������ο�����飩
#define ASCII_8X16             6      //ASCII����8X16����
#define ASCII_16_A             7      //ASCII����16X16���󲻵ȿ�Arial����ַ����������ο�����飩
#define ASCII_16_T             8      //ASCII����16X16���󲻵ȿ�Time news Roman����ַ����������ο�����飩
#define ASCII_12X24            9      //ASCII����12X24�����ӡ�����ַ����������ο�����飩
#define ASCII_24_B             10     //ASCII����24x24���󲻵ȿ��ַ�
#define ASCII_16X32            11     //ASCII����16X32����Arial����ַ����������ο�����飩
#define ASCII_32_B             12     //ASCII����32X32����Arial����ַ����������ο�����飩

#define B_11X16_A              13     //�����ַ�11X16 Arial�������
#define B_18X24_A              14     //�����ַ�18X24 Arial�������
#define B_22X32_A              15     //�����ַ�22X32 Arial�������
#define B_34X48_A              16     //�����ַ�34X48 Arial�������
#define B_40X64_A              17     //�����ַ�40X64 Arial�������
#define B_11X16_T              18     //�����ַ�11X16 Time news Roman�������
#define B_18X24_T              19     //�����ַ�18X24 Time news Roman�������
#define B_22X32_T              20     //�����ַ�22X32 Time news Roman�������
#define B_34X48_T              21     //�����ַ�34X48 Time news Roman�������
#define B_40X64_T              22     //�����ַ�40X64 Time news Roman�������
#define T_FONT_20X24           23     //�����ַ�20X24 ʱ����������
#define T_FONT_24X32           24     //�����ַ�24X32 ʱ����������
#define T_FONT_34X48           25     //�����ַ�34X48 ʱ����������
#define T_FONT_48X64           26     //�����ַ�48X64 ʱ����������
#define F_FONT_816             27     //�����ַ�8X16 ʱ����������
#define F_FONT_1624            28     //�����ַ�16X24 ʱ����������
#define F_FONT_1632            29     //�����ַ�16X32 ʱ����������
#define F_FONT_2448            30     //�����ַ�24X48 ʱ����������
#define F_FONT_3264            31     //�����ַ�32X64 ʱ����������
#define KCD_UI_32              32     //�����ַ�32X32 ʱ����������

#define SEL_GB                 33     //��hzbmp16����������charset������SEL_GB��ʾ��������
#define SEL_JIS                34     //��hzbmp16����������charset������SEL_JIS��ʾ����
#define SEL_KSC                35     //��hzbmp16����������charset������SEL_KSC��ʾ����

/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
    ASCII_GetData(0x41,ASCII_5X7,DZ_Data);      //��ȡ5X7���� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ8 BYTE
	  ASCII_GetData(0x41,ASCII_7X8,DZ_Data);      //��ȡ7X8���� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ8 BYTE
	  ASCII_GetData(0x41,ASCII_6X12,DZ_Data);     //��ȡ6X12���� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ12 BYTE
	  ASCII_GetData(0x41,ASCII_12_B_A,DZ_Data);   //��ȡ12X12���� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ26 BYTE
	  ASCII_GetData(0x41,ASCII_12_B_T,DZ_Data);   //��ȡ12X12���� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ26 BYTE
	  ASCII_GetData(0x41,ASCII_8X16,DZ_Data);     //��ȡ8X16���� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ16 BYTE
	  ASCII_GetData(0x41,ASCII_16_A,DZ_Data);     //��ȡ16X16���� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ34 BYTE
	  ASCII_GetData(0x41,ASCII_16_T,DZ_Data);     //��ȡ16X16���� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ34 BYTE
	  ASCII_GetData(0x41,ASCII_12X24,DZ_Data);    //��ȡ12X24���� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ48 BYTE
	  ASCII_GetData(0x41,ASCII_24_B,DZ_Data);     //��ȡ24X24���󲻵ȿ� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ74 BYTE
	  ASCII_GetData(0x41,ASCII_16X32,DZ_Data);    //��ȡ16X32���� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ64 BYTE
	  ASCII_GetData(0x41,ASCII_32_B,DZ_Data);     //��ȡ32X32���󲻵ȿ� ASCII ����A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ130 BYTE
*************************************************************/
unsigned char  ASCII_GetData(unsigned char ASCIICode,unsigned long ascii_kind,unsigned char *DZ_Data);

/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
    LATIN_GetData(0xa5,DZ_Data); //��ȡ8X16���� �����ı���0xa5�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ16 BYTE
*************************************************************/
unsigned long LATIN_GetData(unsigned int FontCode,unsigned char *DZ_Data);
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
    CYRILLIC_GetData(0x405,DZ_Data); //��ȡ8X16����������ı���0x405�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ16 BYTE
*************************************************************/
unsigned long  CYRILLIC_GetData(unsigned int FontCode,unsigned char *DZ_Data);
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
	  GREECE_GetData(0x375,DZ_Data);  //��ȡ8X16����ϣ���ı���0x375�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ16 BYTE
*************************************************************/
unsigned long GREECE_GetData(unsigned int FontCode,unsigned char *DZ_Data);
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
	  HEBREW_GetData(0x595,DZ_Data);  //��ȡ8X16����ϣ�����ı���0x375�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ16 BYTE
*************************************************************/
unsigned long HEBREW_GetData(unsigned int FontCode,unsigned char *DZ_Data);
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
	  LATIN_B_GetData(0xaA,DZ_Data);  //��ȡ16X16���󲻵ȿ������ı���0xaA�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ34 BYTE
*************************************************************/
unsigned long LATIN_B_GetData(unsigned int FontCode,unsigned char *DZ_Data);
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
		CYRILLIC_B_GetData(0x40A,DZ_Data);  //��ȡ16X16���󲻵ȿ�������ı���0x40A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ34 BYTE
*************************************************************/
unsigned long CYRILLIC_B_GetData(unsigned int FontCode,unsigned char *DZ_Data);
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
	  GREECE_B_GetData(0x37A,DZ_Data); //��ȡ16X16���󲻵ȿ�ϣ���ı���0x37A�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ34 BYTE
*************************************************************/
unsigned long GREECE_B_GetData(unsigned int FontCode,unsigned char *DZ_Data);
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
	  ALB_B_GetData(0x632,DZ_Data); //��ȡ16X16���󲻵ȿ�ϣ���ı���0x632�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ34 BYTE
*************************************************************/
unsigned long ALB_B_GetData(unsigned int unicode_alb,unsigned char *DZ_Data);
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
    THAILAND_GetData(0xe05,DZ_Data);  //��ȡ16X24���󲻵ȿ�ϣ���ı���0x632�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ50 BYTE
*************************************************************/
unsigned long THAILAND_GetData(unsigned int FontCode,unsigned char *DZ_Data);
//Unicode转GB18030转换算法
/*************************************************************
�����÷���
    unsigned int GB_CODE;
		GB_CODE=U2G(0x554a);//���������ֵ�UNICODE����ת����GB������룬����GB_CODE��.����GB_CODE=0xb0a1
*************************************************************/
unsigned int U2G(unsigned int UN_CODE);
/*************************************************************
�����÷���
    unsigned int BIG_CODE;
    BIG5_CODE=BIG52GBK(0xb0,0xda);//���������ֵ�BIG5����ת����GB������룬����BIG_CODE��.����BIG_CODE=0xb0a1
*************************************************************/
unsigned int BIG52GBK( unsigned char h,unsigned char l);
/*************************************************************
�����÷���
    unsigned int JIS_CODE;
    JIS_CODE=U2J(0x6F2B); //ʮ����4401//��0x6F2B��UNICODE����ת����jis0208���룬����JIS_CODE��.����JIS_CODE=0x2C01
    �����������0X2C,ʮ����Ϊ44��λ��Ϊ0X01,ʮ����Ϊ1.
*************************************************************/
unsigned int U2J(WORD Unicode);
/*************************************************************
�����÷���
    unsigned int KSC_CODE;
    KSC_CODE=U2K(0xB27C);//b4bc//�������ַ�unicode����B27C��BIG5����ת����GB������룬����BIG_CODE��.����BIG_CODE=0xb4bc
*************************************************************/
unsigned int U2K(WORD Unicode);
/*************************************************************
�����÷���
    unsigned int JIS_CODE;
    JIS_CODE=SJIS2JIS(0x88ae); //ʮ����1616//��0x88AE��SHIFT-JIS����ת����jis0208���룬����JIS_CODE��.����JIS_CODE=0x1010
*************************************************************/
unsigned int SJIS2JIS(WORD code);
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
		zz_zf(1,B_11X16_A,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�11X16���������ַ�Arial���εĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ24 BYTE
		zz_zf(1,B_18X24_A,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�18X24���������ַ�Arial���εĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ56 BYTE
		zz_zf(1,B_22X32_A,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�22X32���������ַ�Arial���εĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ90 BYTE
		zz_zf(1,B_34X48_A,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�34X48���������ַ�Arial���εĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ206 BYTE
		zz_zf(1,B_40X64_A,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�40X64���������ַ�Arial���εĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ322 BYTE
		zz_zf(1,B_11X16_T,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�11X26���������ַ�Time news Roman�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ24 BYTE
		zz_zf(1,B_18X24_T,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�18X24���������ַ�Time news Roman�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ56 BYTE
		zz_zf(1,B_22X32_T,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�22X32���������ַ�Time news Roman�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ90 BYTE
		zz_zf(1,B_34X48_T,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�34X48���������ַ�Time news Roman�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ206 BYTE
		zz_zf(1,B_40X64_T,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�40X64���������ַ�Time news Roman�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ322 BYTE
		zz_zf(1,T_FONT_20X24,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�20X24���������ַ�ʱ����ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ62 BYTE
		zz_zf(1,T_FONT_24X32,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�24X32���������ַ�ʱ����ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ98 BYTE
		zz_zf(1,T_FONT_34X48,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�34X48���������ַ�ʱ����ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ206 BYTE
		zz_zf(1,T_FONT_48X64,DZ_Data);  //��ȡ˳��Ϊ1�Ĳ��ȿ�48X64���������ַ�ʱ����ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ386 BYTE
	        zz_zf(1,F_FONT_816,DZ_Data);    //��ȡ˳��Ϊ1�Ĳ��ȿ�8X16���������ַ�������ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ18 BYTE
		zz_zf(1,F_FONT_1624,DZ_Data);   //��ȡ˳��Ϊ1�Ĳ��ȿ�16X24���������ַ�������ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ50 BYTE
		zz_zf(1,F_FONT_1632,DZ_Data);   //��ȡ˳��Ϊ1�Ĳ��ȿ�16X32���������ַ�������ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ66 BYTE
		zz_zf(1,F_FONT_2448,DZ_Data);   //��ȡ˳��Ϊ1�Ĳ��ȿ�24X48���������ַ�������ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ146 BYTE
	  zz_zf(1,F_FONT_3264,DZ_Data);   //��ȡ˳��Ϊ1�Ĳ��ȿ�32X64���������ַ�������ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ258 BYTE
	  zz_zf(1,KCD_UI_32,DZ_Data);     //��ȡ˳��Ϊ1�Ĳ��ȿ�32X32����ɴ���UI�ĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ130 BYTE
*************************************************************/
void zz_zf(unsigned char Sequence,unsigned char kind,unsigned char *DZ_Data);

#define ntohs(s)
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
		hzbmp16(SEL_GB, 0xb0a1, 0, 16,DZ_Data);   //��ȡGB18030����Ϊ0xb0a1 16X16���ֵĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ32 BYTE
		hzbmp16(SEL_JIS, 0x0401, 0, 16,DZ_Data);  //��ȡJIS0208����Ϊ0x0401 16X16���ֵĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ32 BYTE
		hzbmp16(SEL_KSC, 0xa1D9, 0, 16,DZ_Data);  //��ȡKSC5601����Ϊ0xA1D9 16X16���ֵĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ32 BYTE
*************************************************************/           
void hzbmp16(unsigned char charset, unsigned int code, unsigned int codeex, unsigned char size,unsigned char *buf);
/*************************************************************
�����÷���
    unsigned char DZ_Data[���鳤�ȿͻ��Զ���];
    hzbmp24(0,0xb0a1,0,24,DZ_Data);   //��ȡGB2312����Ϊ0xb0a1  24X24���ֵĵ������ݣ������������ݴ���DZ_Data�����У����ݳ���Ϊ72 BYTE
*************************************************************/       
void hzbmp24(unsigned char charset, unsigned int code, unsigned int codeex, unsigned char size,unsigned char *buf);

// ����	r_dat_bat ���ֿ��ж����ݺ��� �ú�����ͻ��Լ�ʵ�֣�������ʵ�ָú����Ĳο�����
/****************************************************
u8 r_dat_bat(u32 address,u8 byte_long,u8 *p_arr)
˵����
Address  �� ��ʾ�ַ�������оƬ�е��ֽڵ�ַ��
byte_long�� �Ƕ����������ֽ�����
*p_arr   �� �Ǳ�������ĵ������ݵ����顣
*****************************************************/
/*u8 r_dat_bat(u32 address,u8 byte_long,u8 *p_arr)
{
	u32 j=0;
	CS_L;
	SendByte(address);	 //���������ַ
	for(j=0;j<byte_long;j++)
	{
	 p_arr[j]=ReadByte();//ȡ��������
	}
	CS_H;
	return p_arr[0];	
}

void SendByte(u32 cmd)
{
	u8 i;
	cmd=cmd|0x03000000;
	for(i=0;i<32;i++)
	{
		CLK_L;
		if(cmd&0x80000000)
			SI_H;
		else 
			SI_L;
		CLK_H;
		cmd=cmd<<1;
	}					
}

u8 ReadByte(void)
{
	u8 i;
	u8 dat=0;
	CLK_H;
	for(i=0;i<8;i++)
	{
		CLK_L;
		dat=dat<<1;
		if(SO)
			dat=dat|0x01;
		else 
			dat&=0xfe;
		CLK_H;		
	}	
	return dat;
}*/


		
#endif

