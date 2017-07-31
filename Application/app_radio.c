#include "app_radio.h"
#include "string.h" 
#include "app_msg.h"
#include "Debug_log.h"
//��Ҫ�޸ĵĲ���
#define win_interval 3//��ǩ�����մ��ڼ��
//Я������
typedef enum
{
	WithoutCmd = 0,
	WithCmd
}CMD_Typedef;
//Я�����մ�
typedef enum
{
	WithoutWin = 0,
	WithWin
}WIN_Typedef;

//�Ƿ�ȴ��������
typedef enum
{
	SendNoWait=0,
	SendWait=1
}Send_Wait_Typedef;

extern uint8_t packet[PACKET_PAYLOAD_MAXSIZE];
extern uint8_t DeviceID[4];
extern uint8_t para_record[PARA_RECORD_LEN];

extern uint8_t ActiveMode;//���ڷ������־
//��Ƶ���շ������
uint8_t radio_rcvok = 0;
uint8_t radio_sndok = 0;
Payload_Typedef cmd_packet;//������Ƶ����
extern uint8_t radio_status;//��Ƶ����״̬
uint8_t radio_run_channel;//��Ƶ����ͨ�� 


//uint8_t State_WithWin;
uint8_t State_Mode;//ģʽ
//��ǩ״̬��
TAG_STATE_Typedef TAG_STATE;//��ǩ
const static uint8_t State_WithSensor = 1;//1:���б�ǩ 0���Ǵ��б�ǩ
//��������
extern MSG_Store_Typedef MSG_Store;//��Ϣ������Ϣ���к�0~7
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

//�ļ�����
File_Typedef f_para;//�ļ������������ݻ���

//��Ƶ����ģʽ
extern Radio_Work_Mode_Typedef Radio_Work_Mode;

void Radio_Cmd_Deal(void);
/*
Description:��Ƶ����
Input:state :
Output:��
Return:��
*/
static void radio_on(void)
{
	xosc_hfclk_start();//�ⲿ��������
}
/*
Description:��Ƶ�ر�
Input:state :
Output:��
Return:��
*/
static void radio_off(void)
{
	radio_disable();
	xosc_hfclk_stop();
}

/*
Description:��Ƶѡ��
Input:state :
Output:��
Return:��
*/
static void radio_select(uint8_t ch,uint8_t dir)
{
	uint8_t channel;
	radio_on();//��������
	channel = (ch == DATA_CHANNEL)?RADIO_CHANNEL_DATA:RADIO_CHANNEL_CONFIG;
	radio_run_channel = (ch == DATA_CHANNEL)?RADIO_RUN_DATA_CHANNEL:RADIO_RUN_CONFIG_CHANNEL;
	NRF_RADIO->DATAWHITEIV = channel;//���ݰ׻�
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
@Description:�ش���Ƶִ��״̬���ش�����Ϊtimes
@Input:state :
@Output:��
@Return:��
*/
void Message_Radio_Rx(uint8_t times)
{
	uint32_t ot;
	ot = RADIO_MESSAGE_OT;//���մ�ʱ��
	
	while(Message_Rx == Radio_Work_Mode) //��Ϣ���գ���Ҫ����ͨ��
	{
		radio_select(CONFIG_CHANNEL,RADIO_RX);//�л��ɽ���
		ot = RADIO_MESSAGE_OT;//���մ�ʱ��
		while(--ot)
		{
			if(radio_rcvok)
				break;
		}
		if(0 == ot)
		{
			if(0 ==  times)
			{
				Radio_Work_Mode = Stand_Send;//��ʱ���˳�
				Msg_Packet.MSG_FLAG = MSG_IDLE;
				debug_printf("\r\n���ճ�ʱ");
			}
			else
			{
				ot = RADIO_MESSAGE_OT;//���մ�ʱ��
				Radio_Period_Send(WithCmd,WithWin,SendWait);//�ش�����
				debug_printf("\r\n�ش�����%d",times);
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
Description:��Ƶ���ڷ���
Input:state :
Output:��
Return:��
*/
void Raio_Deal(void)
{
	static uint8_t wincount;
	uint32_t ot;
	wincount++;
	if(wincount >= win_interval)//Я�����մ���
	{
		wincount = 0;
		Radio_Period_Send(WithoutCmd,WithWin,SendWait);//���ʹ����մ���
		radio_select(CONFIG_CHANNEL,RADIO_RX);
//		while(1);
//		debug_printf("\r\n");
		ot = RADIO_RX_OT;//���մ�ʱ��
		while(--ot)
		{
			if(radio_rcvok)
				break;
				
		}

		#ifdef LOG_ON
		if(0 == ot)
		{
			debug_printf("\r\n���ճ�ʱ");
		}
		#endif

	}
	else
	{
		if(ActiveMode)
			Radio_Period_Send(WithoutCmd,WithoutWin,SendWait);//���Ͳ������մ�
	}
	if(radio_rcvok)
	{
		radio_rcvok = 0;
		Radio_Cmd_Deal();//�����
		Message_Radio_Rx(1);//��Ϣ����
	}
	radio_off();
}

/***********************************************************
@Description:���ڷ�����Ƶ��Ϣ,���ȴ��������
@Input��	cmdflag - ����� 1���������� 0�����淢��
				winflag - �Ƿ�Я������
				wait_send_finish:1:�ȴ�������ɣ�0:���ȴ�
@Output����
@Return:��
************************************************************/
static void Radio_Period_Send(uint8_t cmdflag,uint8_t winflag,uint8_t wait_send_finish)
{
	uint32_t ot = 0;
	my_memset(packet,0,PACKET_PAYLOAD_MAXSIZE);

	if(cmdflag)
	{
		memcpy(packet,cmd_packet.packet,cmd_packet.length+RADIO_HEAD_LENGTH);//�������а�
	}
	else
	{
		packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;
		packet[RADIO_LENGTH_IDX] = 0; //payload���ȣ���������
		my_memcpy(packet+TAG_ID_IDX,DeviceID,4);//2~5��ǩID
		TAG_STATE.State_Mode = ActiveMode;//ģʽ
		packet[TAG_STATE_IDX] |= TAG_STATE.State_LP_Alarm << TAG_LOWPWR_Pos;//�͵�ָʾ
		packet[TAG_STATE_IDX] |= TAG_STATE.State_Key_Alram << TAG_KEY_Pos;//����ָʾ
		packet[TAG_STATE_IDX] |= State_WithSensor << TAG_WITHSENSOR_Pos;//����ָʾ
		packet[TAG_STATE_IDX] |= winflag << TAG_WITHWIN_Pos;//���մ�ָʾ
		packet[TAG_STATE_IDX] |= TAG_STATE.State_Mode << TAG_MODE_Pos;//ģʽָʾ
		packet[TAG_VERSION_IDX] |= TAG_HDVER_NUM << TAG_HDVERSION_POS;//Ӳ���汾��
		packet[TAG_VERSION_IDX] |= TAG_SFVER_NUM << TAG_SFVERSION_POS;//����汾��
		packet[TAG_STYPE_IDX] = TAG_SENSORTYPE_SchoolWatch;//��ǩ����
		packet[TAG_SDATA_IDX] |= MSG_Store.MSG_Seq << TAG_MSEQ_Pos;//��������
		cmd_packet.length = TAG_SDATA_IDX;//PAYLOAD���� 
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
	//�Ƿ�ȴ��������
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
Description:��Ƶ����ѡ��
Input:���뷢�书��
Output:��
Return:��
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
Description:�����
Input:src:ԭ���飬����
Output:
Return:��
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
Description:��Ƶ����������жϺ�����
Input:src:
Output:
Return:��
*/
void Radio_Cmd_Deal(void)
{
	uint8_t cmd;
	uint16_t cmd_state;
	cmd_packet.length = 0;
	// if(radio_rcvok)
	// {
	
		if((packet[RADIO_S0_IDX]&RADIO_S0_DIR_Msk) == RADIO_S0_DIR_DOWN) //����
		{
			if(memcmp(DeviceID,packet + TAG_ID_IDX,4)== 00)//��Ե�
			{
				my_memcpy(cmd_packet.packet,packet,packet[RADIO_LENGTH_IDX]+RADIO_HEAD_LENGTH);
				cmd = cmd_packet.packet[CMD_IDX];
				switch(cmd)
				{
					case FILE_CMD_READ://�ļ���ȡ-��Ե�
						f_para.mode = cmd_packet.packet[FILE_MODE_IDX];
						f_para.offset = cmd_packet.packet[FILE_OFFSET_IDX]<<8|cmd_packet.packet[FILE_OFFSET_IDX+1];
						f_para.length = cmd_packet.packet[FILE_LENGTH_IDX];
						if(TRUE == Read_Para(f_para,cmd_packet.packet))
						{
							cmd_packet.length = CMD_ACK_FIX_LENGTH + f_para.length;
						}
						else//ֻ����ִ��״̬
						{
							cmd_packet.length = CMD_ACK_FIX_LENGTH;
						}
						cmd_packet.packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;//����
						cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
						cmd_packet.packet[cmd_packet.length+RADIO_HEAD_LENGTH-1]=Get_Xor(cmd_packet.packet,cmd_packet.length+1);
						Radio_Period_Send(WithCmd,WithoutWin,SendWait);
						
						break;
					case FILE_CMD_WRITE://�ļ�д��-��Ե�
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
					case  MESSAGE_CMD://��Ϣ����ÿ��ֻ����һ����Ϣ
						RADIO_RX_OT = RADIO_MESSAGE_OT;//�������Ϣ������ӳ����մ���ʱ��
						if(0 == (cmd_packet.packet[MSG_HEAD_IDX]&MSG_HEAD_Msk))//��Ϣ�·�֪ͨ����
						{
							Radio_Work_Mode = Message_Rx;
							cmd_packet.packet[EXCUTE_STATE_IDX] = CMD_RUN_SUCCESS>>8;
							cmd_packet.packet[EXCUTE_STATE_IDX+1] = CMD_RUN_SUCCESS;//���߽��������յ�����
							cmd_packet.packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;
							cmd_packet.length = CMD_ACK_FIX_LENGTH;
							cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
							cmd_packet.packet[cmd_packet.length+RADIO_HEAD_LENGTH-1]=Get_Xor(cmd_packet.packet,cmd_packet.length+1);							
							Radio_Period_Send(WithCmd,WithWin,SendNoWait);
							debug_printf("\r\n�ɹ�������Ϣ�·�֪ͨ����");
							memcpy(Msg_Packet.MSG_PUSH_RID,cmd_packet.packet,RADIO_ID_LENGTH);
							Msg_Packet.MSG_FLAG = MSG_START;
							MSG_Packet_ReSet();
						}
						else if(MSG_START == Msg_Packet.MSG_FLAG)
						{
							//������ͬһ��������
							if(memcmp(Msg_Packet.MSG_PUSH_RID,cmd_packet.packet + READER_ID_IDX,4)== 00)//��Ե������
							{
								cmd_state = Message_Deal(cmd_packet.packet);
								cmd_packet.packet[EXCUTE_STATE_IDX] = cmd_state>>8;
								cmd_packet.packet[EXCUTE_STATE_IDX+1] = cmd_state;//���߽��������յ�����
								cmd_packet.packet[RADIO_S0_IDX] = RADIO_S0_DIR_UP;
								cmd_packet.length = CMD_ACK_FIX_LENGTH;
								cmd_packet.packet[RADIO_LENGTH_IDX] = cmd_packet.length;
								cmd_packet.packet[cmd_packet.length+RADIO_HEAD_LENGTH-1]=Get_Xor(cmd_packet.packet,cmd_packet.length+1);
								Radio_Period_Send(WithCmd,WithWin,SendWait);
								if(cmd_state == MSG_START_END_VALUE)//��Ϣ�������
								{
									Radio_Work_Mode = Stand_Send;
									debug_printf("\r\n��Ϣ�������");
									Msg_Packet.MSG_FLAG = MSG_IDLE;
								}								
							}

						}
					case TIME_SET_CMD:
						
				}							
			}
			else if(READER_ID_MBYTE == packet[TAG_ID_IDX])//�㲥
			{
				
			}

		}
}

/************************************************* 
@Description:RADIO�жϴ������ ���մ���
@Input:��
@Output:��
@Return:��
*************************************************/
void Radio_RX_Deal(void)
{
	if(TRUE == Xor_Check(packet,packet[RADIO_LENGTH_IDX]+2))
	{
		if( RADIO_RUN_DATA_CHANNEL == radio_run_channel )//����Ƶ���ɼ���ǩ��Ϣ
		{
		
		}
		else if( RADIO_RUN_CONFIG_CHANNEL == radio_run_channel)
		{
			radio_rcvok= 1;
			debug_printf("\r\n��Ƶ����ͨ�����ճɹ�");

		}
	}
}
/************************************************* 
@Description:RADIO�жϴ������ ���ʹ���
@Input:��
@Output:��
@Return:��
*************************************************/
void Radio_TX_Deal(void)
{

}
/************************************************* 
Description:RADIO�жϴ������
Input:��
Output:��
Return:��
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
