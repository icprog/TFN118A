#include "app_radio.h"
#include "string.h" 
#include "app_msg.h"
#include "Debug_log.h"
#include "rtc.h"
#include "app_test.h"
#include "as3933.h"
#include "app_old_radio.h"

/*********************************************************************
			标签参数定义
**********************************************************************/
//需要修改的参数
TAG_Sned_Typedef TAG_Sned={0xff,0,0};//发送次数
extern Tag_Mode_Typedef Tag_Mode;//标签模式
extern OLD_PARA_T  OLD_PARA;//老协议参数
//射频工作模式
extern Radio_Work_Mode_Typedef Radio_Work_Mode;//用来接收消息
//消息显示
extern void TAG_Msg_OLED_Show(void);
//边界管理器
extern BASE_Typedef BASE;//边界管理器定义
extern u32 baseStationID;//边界管理器ID
//标签状态字
volatile TAG_STATE_Typedef TAG_STATE = {0,0,0,1};//标签，时间更新
extern Time_Cnt_Typedef Time_Type;//定时结构
#ifdef LOG_ON
#define RADIO_RX_OT_CONST 	10000000
#define RADIO_MESSAGE_OT    20000000
#else
#define RADIO_RX_OT_CONST 	900
#define RADIO_MESSAGE_OT    3000
#endif
uint32_t RADIO_RX_OT = RADIO_RX_OT_CONST;//开接收窗口时间
/*********************************************************************
			公共参数定义
**********************************************************************/
extern uint8_t packet[PACKET_PAYLOAD_MAXSIZE];//射频数据
extern uint8_t DeviceID[RADIO_TID_LENGTH];//设备ID
extern uint8_t para_record[PARA_RECORD_LEN];//参数
extern uint8_t radio_status;//射频运行状态
extern MSG_Store_Typedef MSG_Store;//消息定义消息序列号
extern Message_Typedef Msg_Packet;//消息推送与接收

volatile Radio_State_Typedef Radio_State;//射频工作状态
Payload_Typedef cmd_packet;//射频缓冲区 命令射频处理
Serial_Typedef DEV_SER;//设备流水
//文件操作
File_Typedef f_para;//文件操作命令数据缓存
/*******************************************************************
			公共函数
********************************************************************/
void radio_pwr(uint8_t txpower);
void Tag_RadioCmdDeal(void);

/*
@Description:公共函数-射频启动
@Input:state :
@Output:无
@Return:无
*/
void radio_on(void)
{
	xosc_hfclk_start();//外部晶振起振
}
/*
@Description:公共函数-射频关闭
@Input:state :
@Output:无
@Return:无
*/
static void radio_off(void)
{
	radio_disable();
	xosc_hfclk_stop();
}

/*
@Description:公共函数-射频选择
@Input:state :
@Output:无
@Return:无
*/
static void radio_select(uint8_t ch,uint8_t dir)
{
	uint8_t channel;
	radio_on();//开启晶振
	channel = (ch == DATA_CHANNEL)?RADIO_CHANNEL_DATA:RADIO_CHANNEL_CONFIG;
	Radio_State.radio_run_channel = (ch == DATA_CHANNEL)?RADIO_RUN_DATA_CHANNEL:RADIO_RUN_CONFIG_CHANNEL;
	#if WHITEEN
	NRF_RADIO->DATAWHITEIV = channel;//数据白化
	#endif
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
@Description:标签-消息命令处理
@Input:重传次数为times,
@Output:无
@Return:无
2017.9.7根据新协议，只接收。
*/
void Message_Radio_Rx(uint8_t times)
{
	uint32_t ot;
	ot = RADIO_MESSAGE_OT;//接收窗时间
	
	while(Message_Rx == Radio_Work_Mode) //消息接收，需要持续通信
	{
//		radio_select(CONFIG_CHANNEL,RADIO_RX);//切换成接收
		ot = RADIO_MESSAGE_OT;//接收窗时间
		while(--ot)
		{
			if(Radio_State.radio_rcvok)
				break;
		}
		if(0 == ot)
		{
			if(0 ==  times)
			{
				Radio_Work_Mode = Stand_Send;//超时，退出
			}
			else
			{
				ot = RADIO_MESSAGE_OT;//接收窗时间
				times--;
			}
			
		}
		if(Radio_State.radio_rcvok)
		{
			Radio_State.radio_rcvok = 0;
			Tag_RadioCmdDeal();
		}
	}
}
/*
@Description:标签-射频处理
@Input:state :
@Output:无
@Return:无
*/
void Tag_RadioDeal(void)
{
	uint32_t ot;
	uint8_t WinCnt;	
	if(Time_Type.RadioSendFlag)//正常射频业务
	{
		Time_Type.RadioSendFlag = 0;
		WinCnt++;
		if(WinCnt >= Tag_Mode.WinPeriod)//开接收窗口
		{
			WinCnt = 0;
			Radio_Period_Send(WithoutCmd,DATA_CHANNEL,WithWin,SendWait);//发送带接收窗口
			radio_select(CONFIG_CHANNEL,RADIO_RX);
	//		while(1);
	//		debug_printf("\r\n");
			ot = RADIO_RX_OT;//接收窗时间
			while(--ot)
			{
				if(Radio_State.radio_rcvok)
					break;	
			}
			if(0 == ot)
			{
				debug_printf("\r\n接收超时");
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
			if(Active_Mode == Tag_Mode.WorkMode)//活动模式
			{
				if(Active_Send == Tag_Mode.SendMode)//主动发射
				{
					Radio_Period_Send(WithoutCmd,DATA_CHANNEL,WithoutWin,SendWait);//发送不带接收窗
				}
				else if( Passive_Send == Tag_Mode.SendMode)//被动发射 
				{
					if(1 == Tag_Mode.ActivatedByBase)//被激活后发射
					{
						Radio_Period_Send(WithoutCmd,DATA_CHANNEL,WithoutWin,SendWait);//发送不带接收窗
					}
				}

			}			
		}
		if(Radio_State.radio_rcvok)
		{
			Radio_State.radio_rcvok = 0;
			Tag_RadioCmdDeal();//命令处理
			Message_Radio_Rx(0);//消息处理
		}
	}
	if(Active_Mode == Tag_Mode.WorkMode)//活动模式
	{
		if(1 == OLD_PARA.SendEn)
		{
			OLD_PARA.SendEn = 0;
			Radio_Old_Period_Send(SendWait);
			Radio_Old_ReSend(old_resend_times);//重发
			Radio_Init();
		}			
	}
	if(NRF_RADIO->STATE!=RADIO_STATE_STATE_Disabled)
	{
		radio_off();
	}

}


/***********************************************************
@Description:公共函数-周期发送射频信息,并等待发送完成
@Input：	cmdflag - 命令返回 1：返回命令 0：常规发送
			winflag - 是否携带窗口
			wait_send_finish:1:等待发送完成，0:不等待
@Output：无
@Return:无
************************************************************/
void Radio_Period_Send(uint8_t cmdflag,uint8_t Channel,uint8_t winflag,uint8_t wait_send_finish)
{
	uint32_t ot = 0;
	while(1 == radio_tx_isbusy())
	{
		ot++;
		if(ot > RADIO_OVER_TIME)
			return;
	}
	my_memset(packet,0,PACKET_PAYLOAD_MAXSIZE);

	if(cmdflag)
	{
		memcpy(packet,cmd_packet.packet,cmd_packet.length+RADIO_HEAD_LENGTH);//更新所有包
	}
	else
	{
		packet[RADIO_LENGTH_IDX] = 0; //payload长度，后续更新
		packet[RADIO_S1_IDX] = 0x00;
		packet[TAG_SER_IDX] = (RADIO_DIR_UP << RADIO_DIR_POS);//方向上行
		
		packet[TAG_SER_IDX] |= ((DEV_SER.State_Ser_Num << TAG_STATESER_Pos)&TAG_STATESER_Msk);//状态流水
		packet[TAG_SER_IDX] |= ((DEV_SER.Send_Ser_Num << TAG_SENDSER_Pos)&TAG_SENDSER_Msk);//发射流水
		my_memcpy(packet+TAG_ID_IDX,DeviceID,RADIO_TID_LENGTH);//ID
		
		//边界管理器
		if(TAG_Sned.BaseID_Cnt > BaseID_Const)
		{
			baseStationID |= 0xffff;//清除边界管理器ID
		}
		//报警次数
		if(Tag_Clear == Tag_Mode.AlarmClr_Mode)
		{
			if(TAG_STATE.State_Key_Alram&&TAG_Sned.KeyAlram_Cnt < Key_Alram_Const)
			{
				TAG_Sned.KeyAlram_Cnt++;
			}
			else
			{
				TAG_Sned.KeyAlram_Cnt = 0;
				TAG_STATE.State_Key_Alram = 0;
			}
		}
		TAG_STATE.State_Mode = Tag_Mode.WorkMode;//模式
		TAG_STATE.State_Base_Activated = Tag_Mode.ActivatedByBase;
		packet[TAG_STATE_IDX] = 0;
		packet[TAG_STATE_IDX] |= ((TAG_STATE.State_LP_Alarm << TAG_LOWPWR_Pos)&TAG_LOWPWR_Msk);//低电指示
		packet[TAG_STATE_IDX] |= ((TAG_STATE.State_Key_Alram << TAG_KEY_Pos)&TAG_KEY_Msk);//按键指示
		packet[TAG_STATE_IDX] |= ((TAG_STATE.State_Base_Activated << TAG_BASEACT_Pos)&TAG_BASEACT_Msk);//被边界管理器激活指示
		packet[TAG_STATE_IDX] |= ((winflag << TAG_WITHWIN_Pos)&TAG_WIHTWIN_Msk);//接收窗指示
		packet[TAG_STATE_IDX] |= ((TAG_STATE.State_Mode << TAG_WORKMODE_Pos)&TAG_WORKMODE_Msk);//模式指示
		packet[TAG_VERSION_IDX] |= TAG_SFVER_NUM << TAG_SFVERSION_POS;//软件版本号
		//分类信息
		packet[TAG_TYPE_IDX] = TAG_TYPE_SchoolWatch;//；类型
		packet[TAG_RSSIBASE_IDX] = 0;//RSSI
		packet[TAG_BASEDOOR_IDX] = BASE.BaseDoor_ID[0];
		packet[TAG_BASEDOOR_IDX+1] = BASE.BaseDoor_ID[1];
		packet[TAG_BASENORMAL_IDX] = BASE.BaseNormal_ID[0];
		packet[TAG_BASENORMAL_IDX+1] = BASE.BaseNormal_ID[1];
		packet[TAG_MSG_IDX] = ((MSG_Store.T_MSG1_Seq<<TAG_MSG1_Pos)&TAG_MSG1_Msk);//消息1
		packet[TAG_MSG_IDX] |= ((MSG_Store.T_MSG2_Seq<<TAG_MSG2_Pos)&TAG_MSG2_Msk);//消息2

		cmd_packet.length = TAG_MSG_IDX-1;//PAYLOAD长度 
		packet[RADIO_LENGTH_IDX] = cmd_packet.length ;
		packet[PYLOAD_XOR_IDX] = Get_Xor(packet+TAG_SER_IDX,cmd_packet.length-1);//PAYLOAD-XOR
		if((packet[TAG_STATE_IDX]&(~TAG_WIHTWIN_Msk))!= DEV_SER.State_Pre)//标签状态发生变化,接收窗口除外
		{
			DEV_SER.State_Pre = packet[TAG_STATE_IDX]&(~TAG_WIHTWIN_Msk);
			DEV_SER.State_Ser_Num++;
			if(DEV_SER.State_Ser_Num > TAG_STATESER_MAX_VALUE)
			{
				DEV_SER.State_Ser_Num = 0;
			}
		}
		DEV_SER.Send_Ser_Num++;//发射流水+1
		if(DEV_SER.Send_Ser_Num > TAG_SENDSER_MAX_VALUE)
		{
			DEV_SER.Send_Ser_Num = 0;
		}
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
		while(NRF_RADIO->STATE != RADIO_STATE_STATE_TxIdle)
		{
			ot++;
			if(ot > RADIO_OVER_TIME)
				break;			
		}
		if(NRF_RADIO->EVENTS_END)
			NRF_RADIO->EVENTS_END = 0;
	}
}

/************************************************* 
@Description:公共函数-射频功率选择
@Input:输入发射功率
@Output:无
@Return:无
*************************************************/  
void radio_pwr(uint8_t txpower)
{
	switch(txpower)
	{
		case P_PWR_N30DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg30dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case P_PWR_N20DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg20dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case P_PWR_N16DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg16dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case P_PWR_N12DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg12dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case P_PWR_N8DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg8dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case P_PWR_N4DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Neg4dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case P_PWR_P0DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		case P_PWR_P4DBM:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
		default:
			NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);
			break;
	}
}




/*
@Description:标签-射频命令处理
@Input:
@Output:
@Return:无
*/
void Tag_RadioCmdDeal(void)
{
	uint8_t cmd;
	uint16_t cmd_state;
	cmd_packet.length = 0;
	uint32_t rtc_time;
	// if(radio_rcvok)
	// {
	
		if( ((packet[TAG_SER_IDX]&RADIO_DIR_Msk)>>RADIO_DIR_POS) == RADIO_DIR_DOWN ) //下行
		{
			if(memcmp(DeviceID,packet + TAG_ID_IDX,RADIO_TID_LENGTH)== 00)//点对点
			{
				my_memcpy(cmd_packet.packet,packet,packet[RADIO_LENGTH_IDX]+RADIO_HEAD_LENGTH);
				cmd = (cmd_packet.packet[CMD_IDX]&CMD_Msk);
				switch(cmd)
				{
					case FILE_READ_CMD://文件读取-点对点
					{
						f_para.mode = cmd_packet.packet[FILE_MODE_IDX];
						f_para.offset = cmd_packet.packet[FILE_OFFSET_IDX];
						f_para.length = cmd_packet.packet[FILE_LENGTH_IDX];
						cmd_state = Read_Para(f_para,&cmd_packet.packet[CMD_PARA_IDX],&cmd_packet.packet[EXCUTE_STATE_IDX+2]);//读文件
						if(CMD_RUN_SUCCESS == cmd_state)
						{
							cmd_packet.length = CMD_ACK_FIX_LENGTH + f_para.length;
						}
						else//只返回执行状态
						{
							cmd_packet.length = CMD_ACK_FIX_LENGTH;
						}
						cmd_packet.packet[CMD_IDX] = (RADIO_DIR_UP<<RADIO_DIR_POS)&RADIO_DIR_Msk;//上行
						cmd_packet.packet[CMD_IDX] |= FILE_READ_CMD;//命令
						cmd_packet.packet[EXCUTE_STATE_IDX] = cmd_state >>8;
						cmd_packet.packet[EXCUTE_STATE_IDX+1] = cmd_state;						
						cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
						cmd_packet.packet[PYLOAD_XOR_IDX] = Get_Xor(cmd_packet.packet+CMD_IDX,cmd_packet.length-1);
						Radio_Period_Send(WithCmd,CONFIG_CHANNEL,WithoutWin,SendWait);
					}
					break;
					case FILE_WRITE_CMD://文件写入-点对点
					{
						f_para.mode = cmd_packet.packet[FILE_MODE_IDX];
						f_para.offset = cmd_packet.packet[FILE_OFFSET_IDX];
						f_para.length = cmd_packet.packet[FILE_LENGTH_IDX];
						cmd_state = Write_Para(f_para,&cmd_packet.packet[CMD_PARA_IDX]);
						
						cmd_packet.packet[CMD_IDX] = (RADIO_DIR_UP<<RADIO_DIR_POS)&RADIO_DIR_Msk;//上行
						cmd_packet.packet[CMD_IDX] |= FILE_WRITE_CMD;//命令
						cmd_packet.length = CMD_ACK_FIX_LENGTH;
						cmd_packet.packet[EXCUTE_STATE_IDX] = cmd_state >>8;
						cmd_packet.packet[EXCUTE_STATE_IDX+1] = cmd_state;
						cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
						cmd_packet.packet[PYLOAD_XOR_IDX]=Get_Xor(cmd_packet.packet+CMD_IDX,cmd_packet.length-1);
						Radio_Period_Send(WithCmd,CONFIG_CHANNEL,WithoutWin,SendWait);		
					}	
					break;
					case FILE_ERASE_CMD://文件擦除
					{
						f_para.mode = cmd_packet.packet[FILE_MODE_IDX];
						cmd_state = Erase_Para(f_para);//擦除
						
						cmd_packet.packet[CMD_IDX] = (RADIO_DIR_UP<<RADIO_DIR_POS)&RADIO_DIR_Msk;//上行
						cmd_packet.packet[CMD_IDX] |= FILE_ERASE_CMD;//命令
						cmd_packet.packet[EXCUTE_STATE_IDX] = cmd_state >>8;
						cmd_packet.packet[EXCUTE_STATE_IDX+1] = cmd_state;
						cmd_packet.length = CMD_ACK_FIX_LENGTH;
						cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
						cmd_packet.packet[PYLOAD_XOR_IDX]=Get_Xor(cmd_packet.packet+CMD_IDX,cmd_packet.length-1);
						Radio_Period_Send(WithCmd,CONFIG_CHANNEL,WithoutWin,SendWait);							
					}
					break;
					#if 0
					case RECORD_READ_CMD://读运行参数
					{
						if(TRUE == Read_Record(cmd_packet.packet))//读运行参数
						{
							cmd_packet.length = CMD_ACK_FIX_LENGTH + PARA_RECORD_LEN;
						}
						else//只返回执行状态
						{
							cmd_packet.length = CMD_ACK_FIX_LENGTH;
						}
						cmd_packet.packet[CMD_IDX] = (RADIO_DIR_UP<<RADIO_DIR_POS)&RADIO_DIR_Msk;//上行
						cmd_packet.packet[CMD_IDX] |= RECORD_READ_CMD;//命令
						cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
						cmd_packet.packet[PYLOAD_XOR_IDX] = Get_Xor(cmd_packet.packet+CMD_IDX,cmd_packet.length-1);
						Radio_Period_Send(WithCmd,CONFIG_CHANNEL,WithoutWin,SendWait);					
					}
					break;
					case RECORD_WRITE_CMD://写运行参数
					{
						Write_Record(cmd_packet.packet);//写运行参数
						
						cmd_packet.packet[CMD_IDX] = (RADIO_DIR_UP<<RADIO_DIR_POS)&RADIO_DIR_Msk;//上行
						cmd_packet.packet[CMD_IDX] |= FILE_ERASE_CMD;//命令
						cmd_packet.length = CMD_ACK_FIX_LENGTH;
						cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
						cmd_packet.packet[PYLOAD_XOR_IDX]=Get_Xor(cmd_packet.packet+CMD_IDX,cmd_packet.length-1);
						Radio_Period_Send(WithCmd,CONFIG_CHANNEL,WithoutWin,SendWait);								
					}
					break;
					#endif
					case DEVICE_TEST_CMD:
					{
						function_test();
					}
					break;
					case  MSG_PUSH_CMD://消息处理，每次只接收一条消息
					{
						if(((cmd_packet.packet[CMD_PARA_IDX]&READER_MSG_TYPE_Msk)>> READER_MSG_TYPE_Pos) == msg_news)//消息处理
						{
							Radio_Work_Mode = Message_Rx; //标签切换到消息接收模式
							RADIO_RX_OT = RADIO_MESSAGE_OT;//如果是消息命令，则延长接收窗口时间
							//上位机下发消息前数据处理需要时间，隐含增加消息开始指令，但是不对该指令进行响应
							if(cmd_packet.packet[RADIO_LENGTH_IDX] > MSG_CMD_LEN)
							{
								if(1 == Message_Deal(cmd_packet.packet))
								{
									MSG_Store.New_Msg_Flag = 1;
								}
							}
							
						}
						else//时间设置
						{
							rtc_time = (cmd_packet.packet[MSG_DATA_IDX] << 24)|(cmd_packet.packet[MSG_DATA_IDX+1] <<16)
							|(cmd_packet.packet[MSG_DATA_IDX+2]<<8)|(cmd_packet.packet[MSG_DATA_IDX+3]); 
							MSG_Store.T_MSG2_Seq = ((cmd_packet.packet[CMD_PARA_IDX] & READER_MSG_SEQ_Msk)>>READER_MSG_SEQ_Pos);//获取下发的消息序号
							RTC_Time_Set(rtc_time);
						}

						
//						if(0 == (cmd_packet.packet[MSG_HEAD_IDX]&MSG_HEAD_Msk))//消息下发通知命令
//						{
//							Radio_Work_Mode = Message_Rx;
//							cmd_packet.packet[EXCUTE_STATE_IDX] = CMD_RUN_SUCCESS>>8;
//							cmd_packet.packet[EXCUTE_STATE_IDX+1] = CMD_RUN_SUCCESS;//告诉接收器，收到命令
//							cmd_packet.packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;
//							cmd_packet.length = CMD_ACK_FIX_LENGTH;
//							cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
//							cmd_packet.packet[cmd_packet.length+RADIO_HEAD_LENGTH-1]=Get_Xor(cmd_packet.packet,cmd_packet.length+1);							
//							Radio_Period_Send(WithCmd,WithWin,SendNoWait);
//							debug_printf("\r\n成功接收消息下发通知命令");
//							memcpy(Msg_Packet.MSG_PUSH_RID,cmd_packet.packet+READER_ID_IDX,RADIO_ID_LENGTH);
//							Msg_Packet.MSG_FLAG = MSG_START;
//							MSG_Packet_ReSet();
//						}
//						else if(MSG_START == Msg_Packet.MSG_FLAG)
//						{
//							//必须是同一个接收器
//							if(memcmp(Msg_Packet.MSG_PUSH_RID,cmd_packet.packet + READER_ID_IDX,4)== 0)//点对点接收器
//							{
//								cmd_state = Message_Deal(cmd_packet.packet);
//								cmd_packet.packet[EXCUTE_STATE_IDX] = cmd_state>>8;
//								cmd_packet.packet[EXCUTE_STATE_IDX+1] = cmd_state;//告诉接收器，收到命令
//								cmd_packet.packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;
//								cmd_packet.length = CMD_ACK_FIX_LENGTH;
//								cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
//								cmd_packet.packet[cmd_packet.length+RADIO_HEAD_LENGTH-1]=Get_Xor(cmd_packet.packet,cmd_packet.length+1);
//								Radio_Period_Send(WithCmd,WithWin,SendWait);
//								if(cmd_state == MSG_START_END_VALUE)//消息接收完成
//								{
//									Radio_Work_Mode = Stand_Send;
//									debug_printf("\r\n消息接收完成");
//									Msg_Packet.MSG_FLAG = MSG_IDLE;
////									MSG_Store.New_Msg_Flag = 1;
//									TAG_Msg_OLED_Show();
//	
//								}								
//							}

//						}
					}
					break;
					case ALARM_CLEAR_CMD:
					{
						TAG_STATE.State_Key_Alram = 0;
					}
					break;
					default:break;
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
	if(TRUE == Xor_Check(&packet[PYLOAD_XOR_IDX],packet[RADIO_LENGTH_IDX]))
	{
		if( RADIO_RUN_DATA_CHANNEL == Radio_State.radio_run_channel )//数据频道采集标签信息
		{
		
		}
		else if( RADIO_RUN_CONFIG_CHANNEL == Radio_State.radio_run_channel)
		{
			Radio_State.radio_rcvok= 1;
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
@Description:RADIO中断处理程序
@Input:无
@Output:无
@Return:无
*************************************************/  
uint8_t tx_cnt;
uint8_t rx_cnt;
void RADIO_IRQHandler(void)
{
	if(NRF_RADIO->EVENTS_END)//EVENTS_END
	{
		NRF_RADIO->EVENTS_END = 0;
		if(NRF_RADIO->STATE == RADIO_STATE_STATE_RxIdle)
		{	
			if(NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCOk)
			{
				rx_cnt++;
				Radio_RX_Deal();
			}
		}
		else if(NRF_RADIO->STATE == RADIO_STATE_STATE_TxIdle)
		{
			tx_cnt++;
			Radio_State.radio_sndok= 1;
		}
		if(RADIO_STATUS_RX == radio_status)
		{	
			NRF_RADIO->TASKS_START = 1U;
		}
	}
	
}
