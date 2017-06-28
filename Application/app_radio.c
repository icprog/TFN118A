#include "app_radio.h"

//��Ҫ�޸ĵĲ���
#define win_interval 1//��ǩ�����մ��ڼ��
//Я������
typedef enum
{
	WithoutCMD = 0,
	WithCMD
}CMD_Typedef;
//Я�����մ�
typedef enum
{
	WithoutWin = 0,
	WithWin
}WIN_Typedef;



extern uint8_t packet[PACKET_PAYLOAD_MAXSIZE];
extern uint8_t DeviceID[4];
extern uint8_t para_record[PARA_RECORD_LEN];

extern uint8_t ActiveMode;//���ڷ������־
//��Ƶ���շ������
uint8_t radio_rcvok = 0;
uint8_t radio_sndok = 0;
uint8_t m_packet[PACKET_PAYLOAD_MAXSIZE];
//��ǩ״̬��
uint8_t State_LP_Alarm;//�͵籨��
uint8_t State_Key_Alram;//��������
uint8_t State_WithSensor;
//uint8_t State_WithWin;
uint8_t State_Mode;//ģʽ
//��������
uint8_t M_Seq;//��Ϣ���к�0~7

void radio_pwr(uint8_t txpower);
static void Radio_Period_Send(uint8_t cmdflag,uint8_t winflag);
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
	uint8_t ot;
	radio_on();//��������
	channel = (ch == DATA_CHANNEL)?RADIO_CHANNEL_DATA:RADIO_CHANNEL_CONFIG;
	if(dir == RADIO_TX)
	{
		radio_tx_carrier(RADIO_MODE_MODE_Nrf_1Mbit,channel);
	}
	else if(dir == RADIO_RX)
	{
		radio_rx_carrier(RADIO_MODE_MODE_Nrf_1Mbit,channel);
	}
}


void Raio_Deal(void)
{
	static uint8_t wincount;
//	static uint8_t cmd,page,idx;
	UpdateRunPara();//�����ڲ�����-��Ҫ���·��书��
	wincount++;
	if(wincount >= win_interval)
	{
		wincount = 0;
		Radio_Period_Send(WithoutCMD,WithWin);//���ʹ����մ���
		radio_select(CONFIG_CHANNEL,RADIO_RX);
	}
	else
	{
		if(ActiveMode)
			Radio_Period_Send(WithoutCMD,WithoutWin);//���Ͳ������մ�
	}
	
}

/***********************************************************
Description:���ڷ�����Ƶ��Ϣ
Input��	cmdflag - ����� 1���������� 0�����淢��
				winflag - �Ƿ�Я������
Output����
Return:��
************************************************************/
static void Radio_Period_Send(uint8_t cmdflag,uint8_t winflag)
{
	my_memset(packet,0,PACKET_PAYLOAD_MAXSIZE);
	packet[RADIO_S0_IDX] = S0_DIR_UP;
	packet[RADIO_LENGTH_IDX] = 0; //payload���ȣ���������
	my_memcpy(packet+TAG_ID_IDX,DeviceID,4);//2~5��ǩID
	if(cmdflag)
	{
		
	}
	else
	{
		packet[TAG_STATE_IDX] |= State_LP_Alarm << TAG_LOWPWR_Pos;//�͵�ָʾ
		packet[TAG_STATE_IDX] |= State_Key_Alram << TAG_KEY_Pos;//����ָʾ
		packet[TAG_STATE_IDX] |= State_WithSensor << TAG_WITHSENSOR_Pos;//����ָʾ
		packet[TAG_STATE_IDX] |= winflag << TAG_WITHWIN_Pos;//���մ�ָʾ
		packet[TAG_STATE_IDX] |= State_Mode << TAG_MODE_Pos;//ģʽָʾ
		packet[TAG_VERSION_IDX] |= TAG_HDVER_NUM << TAG_HDVERSION_POS;//Ӳ���汾��
		packet[TAG_VERSION_IDX] |= TAG_SFVER_NUM << TAG_SFVERSION_POS;//����汾��
		packet[TAG_STYPE_IDX] = TAG_SENSORTYPE_SchoolWatch;//��ǩ����
		packet[TAG_SDATA_IDX] |= M_Seq << TAG_MSEQ_Pos;//��������
		packet[RADIO_LENGTH_IDX] = TAG_SDATA_IDX ;//PAYLOAD����
		packet[TAG_SDATA_IDX+1] = Get_Xor(packet,TAG_SDATA_IDX+1);//S0+LENGTH+PAYLOAD
	}
	if(cmdflag)
	{
		radio_select(CONFIG_CHANNEL,RADIO_TX);
	}
	else
	{
		radio_select(DATA_CHANNEL,RADIO_TX);
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
		return 1;
	else
		return 0;
}

void Radio_CMD_Deal(void)
{
	uint8_t cmd;
	if(radio_rcvok)
	{
		if((packet[RADIO_S0_IDX]&RADIO_S0_DIR_Msk) == RADIO_S0_DIR_DOWN)
		{
			memccpy(m_packet,packet,packet[RADIO_LENGTH_IDX]+2)
			switch(cmd)
			{
				case FILE_CMD_READ://�ļ���ȡ
					f_para.mode = m_packet[FILE_MODE_IDX];
					f_para.offset = m_packet[FILE_OFFSET_IDX]<<8|m_packet[FILE_OFFSET_IDX+1];
					f_para.length = m_packet[FILE_LENGTH_IDX];
					if(true == Read_Para(type,m_packet))
						{m_packet[RADIO_LENGTH_IDX] = CMD_FIX_LENGTH + f_para.length + PYLOAD_XOR_LENGTH;
					else
						m_packet[RADIO_LENGTH_IDX] = CMD_FIX_LENGTH;
					break;
				case FILE_CMD_WRITE://�ļ�д��
					
			}				
		}
	}
}
/************************************************* 
Description:RADIO�жϴ������
Input:��
Output:��
Return:��
*************************************************/  
void RADIO_IRQHandler(void)
{
	if(NRF_RADIO->EVENTS_END)//EVENTS_END
	{
		NRF_RADIO->EVENTS_END = 0;
		if(NRF_RADIO->STATE == RADIO_STATE_STATE_RxIdle)
		{	
			if(NRF_RADIO->CRCSTATUS == RADIO_CRCSTATUS_CRCSTATUS_CRCOk)
			{
				if(1 == Xor_Check(packet,packet[2]+2))
				{
					if(memcmp(DeviceID,packet + TAG_ID_IDX,4)== 00)
					{
						radio_rcvok= 1;
					}	
					else if(ID_BROADCAST_MBYTE == packet[TAG_ID_IDX])
					{
						
					}
				}

			}
		}
		else if(NRF_RADIO->STATE == RADIO_STATE_STATE_TxIdle)
		{
			radio_sndok= 1;
		}
	}
}