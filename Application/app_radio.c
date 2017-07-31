#include "app_radio.h"
#include "string.h" 
#include "app_msg.h"
#include "Debug_log.h"
//需要修改的参数
#define win_interval 3//标签开接收窗口间隔
//携带命令
typedef enum
{
	WithoutCmd = 0,
	WithCmd
}CMD_Typedef;
//携带接收窗
typedef enum
{
	WithoutWin = 0,
	WithWin
}WIN_Typedef;

//是否等待发送完成
typedef enum
{
	SendNoWait=0,
	SendWait=1
}Send_Wait_Typedef;

extern uint8_t packet[PACKET_PAYLOAD_MAXSIZE];
extern uint8_t DeviceID[4];
extern uint8_t para_record[PARA_RECORD_LEN];

extern uint8_t ActiveMode;//周期发送秒标志
//射频接收发送完成
uint8_t radio_rcvok = 0;
uint8_t radio_sndok = 0;
Payload_Typedef cmd_packet;//命令射频处理
extern uint8_t radio_status;//射频运行状态
uint8_t radio_run_channel;//射频运行通道 


//uint8_t State_WithWin;
uint8_t State_Mode;//模式
//标签状态字
TAG_STATE_Typedef TAG_STATE;//标签
const static uint8_t State_WithSensor = 1;//1:传感标签 0：非传感标签
//传感数据
extern MSG_Store_Typedef MSG_Store;//消息定义消息序列号0~7
extern Message_Typedef Msg_Packet;


#ifdef LOG_ON
#define RADIO_RX_OT_CONST 	10000000
#define RADIO_MESSAGE_OT    20000000
#else
#define RADIO_RX_OT_CONST 	890
#define RADIO_MESSAGE_OT    3000
#endif
uint32_t RADIO_RX_OT = RADIO_RX_OT_CONST;
void radio_pwr(uint8_t txpower);
static void Radio_Period_Send(uint8_t cmdflag,uint8_t winflag,uint8_t wait_send_finish);

//文件操作
File_Typedef f_para;//文件操作命令数据缓存

//射频工作模式
extern Radio_Work_Mode_Typedef Radio_Work_Mode;

void Radio_Cmd_Deal(void);
/*
Description:射频启动
Input:state :
Output:无
Return:无
*/
static void radio_on(void)
{
	xosc_hfclk_start();//外部晶振起振
}
/*
Description:射频关闭
Input:state :
Output:无
Return:无
*/
static void radio_off(void)
{
	radio_disable();
	xosc_hfclk_stop();
}

/*
Description:射频选择
Input:state :
Output:无
Return:无
*/
static void radio_select(uint8_t ch,uint8_t dir)
{
	uint8_t channel;
	radio_on();//开启晶振
	channel = (ch == DATA_CHANNEL)?RADIO_CHANNEL_DATA:RADIO_CHANNEL_CONFIG;
	radio_run_channel = (ch == DATA_CHANNEL)?RADIO_RUN_DATA_CHANNEL:RADIO_RUN_CONFIG_CHANNEL;
	NRF_RADIO->DATAWHITEIV = channel;//数据白化
	if(dir == RADIO_TX)
	{
		radio_tx_carrier(RADIO_MODE_MODE_Nrf_1Mbit,channel);
	}
	else if(dir == RADIO_RX)
	{
		radio_rx_carrier(RADIO_MODE_MODE_Nrf_1Mbit,channel);
	}
}

/*
@Description:重传射频执行状态，重传次数为times
@Input:state :
@Output:无
@Return:无
*/
void Message_Radio_Rx(uint8_t times)
{
	uint32_t ot;
	ot = RADIO_MESSAGE_OT;//接收窗时间
	
	while(Message_Rx == Radio_Work_Mode) //消息接收，需要持续通信
	{
		radio_select(CONFIG_CHANNEL,RADIO_RX);//切换成接收
		ot = RADIO_MESSAGE_OT;//接收窗时间
		while(--ot)
		{
			if(radio_rcvok)
				break;
		}
		if(0 == ot)
		{
			if(0 ==  times)
			{
				Radio_Work_Mode = Stand_Send;//超时，退出
				Msg_Packet.MSG_FLAG = MSG_IDLE;
				debug_printf("\r\n接收超时");
			}
			else
			{
				ot = RADIO_MESSAGE_OT;//接收窗时间
				Radio_Period_Send(WithCmd,WithWin,SendWait);//重传命令
				debug_printf("\r\n重传命令%d",times);
				times--;
			}
			
		}
		if(radio_rcvok)
		{
			radio_rcvok = 0;
			Radio_Cmd_Deal();
		}
	}
}
/*
Description:射频周期发送
Input:state :
Output:无
Return:无
*/
void Raio_Deal(void)
{
	static uint8_t wincount;
	uint32_t ot;
	wincount++;
	if(wincount >= win_interval)//携带接收窗口
	{
		wincount = 0;
		Radio_Period_Send(WithoutCmd,WithWin,SendWait);//发送带接收窗口
		radio_select(CONFIG_CHANNEL,RADIO_RX);
//		while(1);
//		debug_printf("\r\n");
		ot = RADIO_RX_OT;//接收窗时间
		while(--ot)
		{
			if(radio_rcvok)
				break;
				
		}

		#ifdef LOG_ON
		if(0 == ot)
		{
			debug_printf("\r\n接收超时");
		}
		#endif

	}
	else
	{
		if(ActiveMode)
			Radio_Period_Send(WithoutCmd,WithoutWin,SendWait);//发送不带接收窗
	}
	if(radio_rcvok)
	{
		radio_rcvok = 0;
		Radio_Cmd_Deal();//命令处理
		Message_Radio_Rx(1);//消息处理
	}
	radio_off();
}

/***********************************************************
@Description:周期发送射频信息,并等待发送完成
@Input：	cmdflag - 命令返回 1：返回命令 0：常规发送
				winflag - 是否携带窗口
				wait_send_finish:1:等待发送完成，0:不等待
@Output：无
@Return:无
************************************************************/
static void Radio_Period_Send(uint8_t cmdflag,uint8_t winflag,uint8_t wait_send_finish)
{
	uint32_t ot = 0;
	my_memset(packet,0,PACKET_PAYLOAD_MAXSIZE);

	if(cmdflag)
	{
		memcpy(packet,cmd_packet.packet,cmd_packet.length+RADIO_HEAD_LENGTH);//更新所有包
	}
	else
	{
		packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;
		packet[RADIO_LENGTH_IDX] = 0; //payload长度，后续更新
		my_memcpy(packet+TAG_ID_IDX,DeviceID,4);//2~5标签ID
		TAG_STATE.State_Mode = ActiveMode;//模式
		packet[TAG_STATE_IDX] |= TAG_STATE.State_LP_Alarm << TAG_LOWPWR_Pos;//低电指示
		packet[TAG_STATE_IDX] |= TAG_STATE.State_Key_Alram << TAG_KEY_Pos;//按键指示
		packet[TAG_STATE_IDX] |= State_WithSensor << TAG_WITHSENSOR_Pos;//传感指示
		packet[TAG_STATE_IDX] |= winflag << TAG_WITHWIN_Pos;//接收窗指示
		packet[TAG_STATE_IDX] |= TAG_STATE.State_Mode << TAG_MODE_Pos;//模式指示
		packet[TAG_VERSION_IDX] |= TAG_HDVER_NUM << TAG_HDVERSION_POS;//硬件版本号
		packet[TAG_VERSION_IDX] |= TAG_SFVER_NUM << TAG_SFVERSION_POS;//软件版本号
		packet[TAG_STYPE_IDX] = TAG_SENSORTYPE_SchoolWatch;//标签类型
		packet[TAG_SDATA_IDX] |= MSG_Store.MSG_Seq << TAG_MSEQ_Pos;//传感数据
		cmd_packet.length = TAG_SDATA_IDX;//PAYLOAD长度 
		packet[RADIO_LENGTH_IDX] = cmd_packet.length ;
		packet[TAG_SDATA_IDX+1] = Get_Xor(packet,cmd_packet.length+1);//S0+max_length+PAYLOAD
	}
	if(cmdflag)
	{
		radio_select(CONFIG_CHANNEL,RADIO_TX);
	}
	else
	{
		radio_select(DATA_CHANNEL,RADIO_TX);
	}
	//是否等待发送完成
	if(1 == wait_send_finish)
	{
		radio_sndok = 0;
		while(radio_sndok == 0)
		{
			ot++;
			if(ot > RADIO_OVER_TIME)
				break;
		}		
	}
}

/************************************************* 
Description:射频功率选择
Input:输入发射功率
Output:无
Return:无
*************************************************/  
void radio_pwr(uint8_t txpower)
{
	switch(txpower)
	{
		case TAGP_PWR_N30DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg30dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case TAGP_PWR_N20DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg20dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case TAGP_PWR_N16DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg16dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case TAGP_PWR_N12DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg12dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case TAGP_PWR_N8DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg8dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case TAGP_PWR_N4DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg4dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case TAGP_PWR_P0DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case TAGP_PWR_P4DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		default:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
	}
}

/*
Description:异或检查
Input:src:原数组，长度
Output:
Return:无
*/
uint8_t Xor_Check(uint8_t *src,uint8_t size)
{
	uint8_t sum=0;
	uint8_t i;
	
	for(i = 0;i<size;i++)
	{
		sum ^= src[i];
	}
	if(sum == 0)
		return TRUE;
	else
		return FALSE;
}


/*
Description:射频命令处理，放在中断函数中
Input:src:
Output:
Return:无
*/
void Radio_Cmd_Deal(void)
{
	uint8_t cmd;
	uint16_t cmd_state;
	cmd_packet.length = 0;
	// if(radio_rcvok)
	// {
	
		if((packet[RADIO_S0_IDX]&RADIO_S0_DIR_Msk) == RADIO_S0_DIR_DOWN) //下行
		{
			if(memcmp(DeviceID,packet + TAG_ID_IDX,4)== 00)//点对点
			{
				my_memcpy(cmd_packet.packet,packet,packet[RADIO_LENGTH_IDX]+RADIO_HEAD_LENGTH);
				cmd = cmd_packet.packet[CMD_IDX];
				switch(cmd)
				{
					case FILE_CMD_READ://文件读取-点对点
						f_para.mode = cmd_packet.packet[FILE_MODE_IDX];
						f_para.offset = cmd_packet.packet[FILE_OFFSET_IDX]<<8|cmd_packet.packet[FILE_OFFSET_IDX+1];
						f_para.length = cmd_packet.packet[FILE_LENGTH_IDX];
						if(TRUE == Read_Para(f_para,cmd_packet.packet))
						{
							cmd_packet.length = CMD_ACK_FIX_LENGTH + f_para.length;
						}
						else//只返回执行状态
						{
							cmd_packet.length = CMD_ACK_FIX_LENGTH;
						}
						cmd_packet.packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;//上行
						cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
						cmd_packet.packet[cmd_packet.length+RADIO_HEAD_LENGTH-1]=Get_Xor(cmd_packet.packet,cmd_packet.length+1);
						Radio_Period_Send(WithCmd,WithoutWin,SendWait);
						
						break;
					case FILE_CMD_WRITE://文件写入-点对点
						f_para.mode = cmd_packet.packet[FILE_MODE_IDX];
						f_para.offset = cmd_packet.packet[FILE_OFFSET_IDX]<<8|cmd_packet.packet[FILE_OFFSET_IDX+1];
						f_para.length = cmd_packet.packet[FILE_LENGTH_IDX];
						Write_Para(f_para,cmd_packet.packet);
						cmd_packet.packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;
						cmd_packet.length = CMD_ACK_FIX_LENGTH;
						cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
						cmd_packet.packet[cmd_packet.length+RADIO_HEAD_LENGTH-1]=Get_Xor(cmd_packet.packet,cmd_packet.length+1);
						Radio_Period_Send(WithCmd,WithoutWin,SendWait);		
						break;
					default:break;
					case  MESSAGE_CMD://消息处理，每次只接收一条消息
						RADIO_RX_OT = RADIO_MESSAGE_OT;//如果是消息命令，则延长接收窗口时间
						if(0 == (cmd_packet.packet[MSG_HEAD_IDX]&MSG_HEAD_Msk))//消息下发通知命令
						{
							Radio_Work_Mode = Message_Rx;
							cmd_packet.packet[EXCUTE_STATE_IDX] = CMD_RUN_SUCCESS>>8;
							cmd_packet.packet[EXCUTE_STATE_IDX+1] = CMD_RUN_SUCCESS;//告诉接收器，收到命令
							cmd_packet.packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;
							cmd_packet.length = CMD_ACK_FIX_LENGTH;
							cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
							cmd_packet.packet[cmd_packet.length+RADIO_HEAD_LENGTH-1]=Get_Xor(cmd_packet.packet,cmd_packet.length+1);							
							Radio_Period_Send(WithCmd,WithWin,SendNoWait);
							debug_printf("\r\n成功接收消息下发通知命令");
							memcpy(Msg_Packet.MSG_PUSH_RID,cmd_packet.packet,RADIO_ID_LENGTH);
							Msg_Packet.MSG_FLAG = MSG_START;
							MSG_Packet_ReSet();
						}
						else if(MSG_START == Msg_Packet.MSG_FLAG)
						{
							//必须是同一个接收器
							if(memcmp(Msg_Packet.MSG_PUSH_RID,cmd_packet.packet + READER_ID_IDX,4)== 00)//点对点接收器
							{
								cmd_state = Message_Deal(cmd_packet.packet);
								cmd_packet.packet[EXCUTE_STATE_IDX] = cmd_state>>8;
								cmd_packet.packet[EXCUTE_STATE_IDX+1] = cmd_state;//告诉接收器，收到命令
								cmd_packet.packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;
								cmd_packet.length = CMD_ACK_FIX_LENGTH;
								cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
								cmd_packet.packet[cmd_packet.length+RADIO_HEAD_LENGTH-1]=Get_Xor(cmd_packet.packet,cmd_packet.length+1);
								Radio_Period_Send(WithCmd,WithWin,SendWait);
								if(cmd_state == MSG_START_END_VALUE)//消息接收完成
								{
									Radio_Work_Mode = Stand_Send;
									debug_printf("\r\n消息接收完成");
									Msg_Packet.MSG_FLAG = MSG_IDLE;
								}								
							}

						}
					case TIME_SET_CMD:
						
				}							
			}
			else if(READER_ID_MBYTE == packet[TAG_ID_IDX])//广播
			{
				
			}

		}
}

/************************************************* 
@Description:RADIO中断处理程序 接收处理
@Input:无
@Output:无
@Return:无
*************************************************/
void Radio_RX_Deal(void)
{
	if(TRUE == Xor_Check(packet,packet[RADIO_LENGTH_IDX]+2))
	{
		if( RADIO_RUN_DATA_CHANNEL == radio_run_channel )//数据频道采集标签信息
		{
		
		}
		else if( RADIO_RUN_CONFIG_CHANNEL == radio_run_channel)
		{
			radio_rcvok= 1;
			debug_printf("\r\n射频配置通道接收成功");

		}
	}
}
/************************************************* 
@Description:RADIO中断处理程序 发送处理
@Input:无
@Output:无
@Return:无
*************************************************/
void Radio_TX_Deal(void)
{

}
/************************************************* 
Description:RADIO中断处理程序
Input:无
Output:无
Return:无
*************************************************/  
uint8_t tx_cnt;
void RADIO_IRQHandler(void)
{
	if(NRF_RADIO->EVENTS_END)//EVENTS_END
	{
		NRF_RADIO->EVENTS_END = 0;
		if(NRF_RADIO->STATE == RADIO_STATE_STATE_RxIdle)
		{	
			if(NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCOk)
			{
				Radio_RX_Deal();
			}
		}
		else if(NRF_RADIO->STATE == RADIO_STATE_STATE_TxIdle)
		{
			tx_cnt++;
			radio_sndok= 1;
		}
	}
	
}
