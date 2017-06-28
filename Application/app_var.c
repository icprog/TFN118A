#include "app_var.h"
#include "nrf_nvmc.h"
//设备ID
uint8_t DeviceID[4] = {0x33,0x33,0x33,0x33};
/********************************************
					自定义FLASH存储区
********************************************/
//MARK
const uint8_t nvmc_flash_mark[11]={0x54,0x46,0x4E,0x31,0x31,0x38,0x41,0x00,0x00,0x00,0x00};//TFN118A
uint32_t nrf_addr;//flash地址
uint8_t Rom_Record_Offset[4] = {16,16,32,32};//4个扇区对应的记录个数,分别对应参数区、保留区、用户区1、用户区2
uint8_t Rom_Record_Length[4] = {16,16,16,16};//每条记录对应的字节数
uint8_t ActiveMode;//周期发送秒标志

//设备运行默认参数
const uint8_t  para_default[PARA_RECORD_LEN] = 
{
16,//短号
0x60,//b7~4-发射功率0dbm
0xf1,//b7~b4 信息来源，默认不携带信息 b3~b0 工作模式 ，默认活动模式
0x00,//心率传感
0x00,//b7-6,单位，分，b5-0，采样周期值，5分钟
0
};
//以下几个参数用来记录ROM中的16/32块（偏移量）的计数值 值16/32，表示记录个数，0表示未记录
//255扇区打标记0x3fc00
//254扇区ROM0_Pos 存储频道参数  0x3f800
//253扇区ROM1_Pos 保留区 0x3f400
//252扇区ROM2_Pos 用户区1 0x3f00
//251扇区ROM3_Pos 用户区2 0x3ec00
uint8_t RomMark;//倒数第一个区打标机
uint8_t	ROM0_Pos;		//倒数第二个区，内部参数区
uint8_t	ROM1_Pos;		//倒数第三个扇区，保留区
uint8_t	ROM2_Pos;	  	//倒数第四个扇区，用户区1
uint8_t	ROM3_Pos;   	//倒数第五个扇区,用户区2
uint8_t *pROM;			//记录指针
uint8_t	* caucpROM[]={&ROM0_Pos,&ROM1_Pos,&ROM2_Pos,&ROM3_Pos};	
//存储内部参数 
uint8_t para_record[PARA_RECORD_LEN];

//命令
uint16_t cmd_state;//命令执行情况
//文件操作
File_Typedef f_para;//文件操作命令数据缓存

/*
Description:返回最新记录ROM位置
Input:state :
Output:无
Return:无
*/
uint8_t GetValidPara(uint8_t type,)
{
	switch(type)
	{
		case TYPE_PARA: addr = 
	}
}



/*
Description:返回最新记录ROM位置
Input:state : temp_addr存储区地址，temp_size存储区总记录个数，temp_byte每条记录长度
Output:返回最新记录位置 0：表示无记录，空 ；>1 对应的最新记录
Return:无
*/
uint8_t Rom_Pos(uint32_t temp_addr,uint8_t temp_size,uint8_t temp_byte)
{
	uint8_t i,j;
//	temp_addr = (uint8_t*)temp_addr;
	//验证buff是否为空，返回最新记录的位置
	//不相等，继续查找,找到空，则返回rom位置，i =  1~~Rom_record_size。返回0表示配置区全空	
	for(i=0;i<temp_size;i++)
	{
		for(j=0;j<temp_byte;j++)
		{
			if(*(uint8_t*)temp_addr++ != 0xff)
				break;
		}
		if(j>=temp_byte) break;
		i++;
	}
	return i;
}


/****************************************
函数：获取设备ID
输入：
输出：
****************************************/
void GetDeviceID(void)
{
	nrf_nvmc_read_bytes(ID_BEGIN,DeviceID,4);
}


/*
Description:运行内部参数
Input:state : 
Output:
Return:无
*/
void UpdateRunPara(void)
{
	uint16_t unit;
	uint8_t time;
	//工作模式：活动，或保留 1:活动模式
	ActiveMode = (para_record[TAGP_WORKMODE_IDX] & TAGP_WORKMODE_Msk) >> TAGP_WORKMODE_Pos;
	
	if(ActiveMode)
		radio_pwr((para_record[TAGP_PWR_IDX] & TAGP_PWR_Msk) >> TAGP_PWR_Pos);
	else
	radio_pwr(TAGP_PWR_N4DBM);	
}

/*********************************************************
Description:文件读取
Input:state : 
Output:
Return:无
内部文件（flash）操作-读写器下发读文件命令
定义		命令	保留 	模式		偏移  	长度	
数组位置	10		11		13			14		16	
模式：01：内部参数区 02：保留区 03用户区 04用户区
标签应答
定义		命令代码1字节 	执行状态2字节 	数据
数组位置	10				11				13
					
*********************************************************/
uint8_t Read_Para(File_Typedef f1_para,uint8_t *p_packet)
{
	uint8_t n_ROM;
	uint8_t length,offset;
	cmd_state = CMD_RUN_SUCCESS;
	switch(f1_para.mode)
	{
		case FILE_MODE_PARA:
				offset = Rom_Record_Offset[0];
				length = Rom_Record_Length[0];	
				addr = PARA_BASE;//基地址
				n_ROM = ROM0_Pos;
			break;
		case FILE_MODE_RESERVER:
				offset = Rom_Record_Offset[1];
				length = Rom_Record_Length[1];
				addr = RESERVER_BASE;//基地址
				n_ROM = ROM1_Pos;
			break;
		case FILE_MODE_USER1:
				offset = Rom_Record_Offset[2];
				length = Rom_Record_Length[2];			
				addr = USER1_BASE;//基地址
				n_ROM = ROM2_Pos;
			break;
		case FILE_MODE_USER2:
				offset = Rom_Record_Offset[3];
				length = Rom_Record_Length[3];		
				addr = USER2_BASE;//基地址
				n_ROM = ROM3_Pos;
			break;
		case 
		default:
			cmd_state = FILE_ERR <<8 | FILE_MODE_ERR;
			break;
	}
	//长度和偏移量边界检查
	if(f1_para.length>length || (f1_para.offset>offset && f1_para<FILE_OFFSET_NEW))
	{
		cmd_state = FILE_ERR << 8 | FILE_BODER_ERR;
	}
	//命令错误
	if(cmd_state!=CMD_RUN_SUCCESS)
	{
		memccpy(&p_packet[EXCUTE_STATE_IDX],&cmd_state,EXCUTE_STATE_LENGTH);
		return false;
	}
	else
	{
		if(FILE_OFFSET_NEW == f1_para)
			addr += (n_ROM-1)*length; 
		else
			addr += f1_para.offset*length;
	}
	nrf_nvmc_read_bytes(addr, &p_packet[EXCUTE_DATA_IDX],f1_para.length);
	return true;
}
/*
Description:获取系统运行参数，及获取参数区、保留区、用户区最新参数偏移量
Input:state : 
Output:
Return:无
*/
void SystemParaInit(void)
{
	uint32_t nrf_addr;
	uint8_t base_offset;
	uint8_t flash_temp[11];//temp memory
	GetDeviceID();//获取设备ID
	nrf_addr = MARK_BASE;
	nrf_nvmc_read_bytes(nrf_addr,flash_temp,11);
	//最后一个扇区用来打标记，如果空，则清空ROM0_Pos-ROM5存储区,判断是否是新下载的程序
	if((flash_temp[0]!=nvmc_flash_mark[0])||(flash_temp[1]!=nvmc_flash_mark[1])||(flash_temp[2]!=nvmc_flash_mark[2])
		||(flash_temp[3]!=nvmc_flash_mark[3])||(flash_temp[4]!=nvmc_flash_mark[4])||(flash_temp[5]!=nvmc_flash_mark[5])
		||(flash_temp[6]!=nvmc_flash_mark[6])||(flash_temp[7]!=nvmc_flash_mark[7])||(flash_temp[8]!=nvmc_flash_mark[8])
		||(flash_temp[9]!=nvmc_flash_mark[9])||(flash_temp[10]!=nvmc_flash_mark[10]))
	{			 
		nrf_addr = PARA_BASE;		
		nrf_nvmc_page_erase(nrf_addr);

		nrf_addr = RESEVER_BASE;
		nrf_nvmc_page_erase(nrf_addr);

		nrf_addr = USER1_BASE;
		nrf_nvmc_page_erase(nrf_addr);

		nrf_addr = USER2_BASE;
		nrf_nvmc_page_erase(nrf_addr);

		nrf_addr = MARK_BASE;
		nrf_nvmc_page_erase(nrf_addr);
				
		nrf_nvmc_write_bytes(nrf_addr,nvmc_flash_mark,11);
	}
	else
	{
		//打过标记读取上次存储的信息
		//内部参数区
		nrf_addr = PARA_BASE;
		ROM0_Pos = Rom_Pos(nrf_addr,Rom_Record_Offset[0],Rom_Record_Length[0]);
		//reserver area
		nrf_addr = RESEVER_BASE;
		ROM1_Pos = Rom_Pos(nrf_addr,Rom_Record_Offset[1],Rom_Record_Length[1]);
		//user area1
		nrf_addr = USER1_BASE;
		ROM2_Pos = Rom_Pos(nrf_addr,Rom_Record_Offset[2],Rom_Record_Length[2]);
		//user area2
		nrf_addr = USER2_BASE;
		ROM3_Pos = Rom_Pos(nrf_addr,Rom_Record_Offset[3],Rom_Record_Length[3]);				
	}
	if(ROM0_Pos)//更新过参数
	{
		nrf_addr = PARA_BASE;
		base_offset = (ROM0_Pos - 1)*Rom_Record_Length[0];
		nrf_nvmc_read_bytes(nrf_addr+base_offset,para_record,Rom_Record_Length[0]);
	}		
	else//否则默认参数
	{
		my_memcpy(para_record,para_default,16);
	}
	UpdateRunPara();
}