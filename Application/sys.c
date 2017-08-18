#include "sys.h"

/*--------------清空BUFF-----------------*/
void my_memset(u8 *dest,u8 ch,u16 size)
{
	u16 i;
	for(i=0;i<size;i++)
	{
		*(dest+i) = ch;
	}
}

/*
Description:拷贝
Input:state : 
Output:
Return:无
*/
//void my_memcpy(u8 *dest,const u8 * src,u16 size)
//{
//	u16 i;
//	for(i = 0 ; i < size ; i++)
//	{
//		*(dest+i) = *(src+i);
//	}
//}
void *my_memcpy(void* dest,const void* src,u16 size)
{
	char *tmp = dest;
	const char *s = src;
	while(size--)
		*tmp++ = *s++;
	return dest;
}

/*
Description:得到异或值
Input:src:原数组，长度
Output:
Return:无
*/
uint8_t Get_Xor(u8 *src,u16 size)
{
	uint8_t temp = 0,i;
	for(i = 0;i<size;i++)
	{
		temp ^= src[i];
	}
	return temp;
}

/************************************************* 
@Description:4个数去掉最大值，最小值，然后取平均。
@Input:无
@Output:无
@Return:无
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
/*---------------比较函数---------------------*/
/*
if Return value < 0 then it indicates str1 is less than str2.
if Return value > 0 then it indicates str2 is less than str1.
if Return value = 0 then it indicates str1 is equal to str2.
*/
//int memcmp(const void *str1, const void *str2, size_t n)
//{
//		const char* pSrc1 = (char*)src1;
//		const char* pSrc2 = (char*)src2;
//		while (len-- > 0)
//		{
//			if (*pSrc1++ != *pSrc2++) 
//			{
//				return *pSrc1 < *pSrc2 ? -1 : 1;
//			}
//		}
//		return 0; 
//}
//not equal return 1
uint32_t my_memcmp_const(int32 *src,int32 const_value,u32 size)
{
	u32 i;
	for(i=0;i<size;i++)
	{
		if(const_value != *(src++))
		{
			return 1;
		}
	}
	return 0;
}
