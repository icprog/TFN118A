#include "app_old_radio.h"
#include "string.h"
#include "app_radio.h"
#include "radio_config.h"
#include "radio_config.h"
#define OLD_PACKET_BASE_ADDRESS_LENGTH       (4UL)  //!< Packet base address length field size in bytes
#define OLD_PACKET_STATIC_LENGTH             (32UL)  //!< Packet static length in bytes
#define OLD_PACKET_PAYLOAD_MAXSIZE           (OLD_PACKET_STATIC_LENGTH)  //!< Packet payload maximum size in bytes
#define 	OLD_RADIO_OVER_TIME							100000
uint8_t OldPacket[OLD_PACKET_STATIC_LENGTH];
extern uint8_t DeviceID[5];
extern TAG_STATE_Typedef TAG_STATE;//标签
void old_radio_configure(void)
{
    // Radio config
    // Radio address config
	//LOGIC0:0xE7E7E7E7E7         LOGIC1:0xC200C2C2C2
	//LOGIC2:0xC300C2C2C2         LOGIC3:0xC400C2C2C2
	//LOGIC4:0xC800C2C2C2         LOGIC5:0xC700C2C2C2
	//LOGIC6:0xC600C2C2C2         LOGIC7:0xC500C2C2C2
    //通过以下配置，我们得到的逻辑地址即8个通道的地址分别为以上8个地址，
    NRF_RADIO->PREFIX0 = 0xC4C3C2E7UL;  // 逻辑地址 // Prefix byte of addresses 3 to 0
	NRF_RADIO->PREFIX1 = 0xC5C6C7C8UL;  // Prefix byte of addresses 7 to 4
	NRF_RADIO->BASE0   = 0xE7E7E7E7UL;  // 逻辑地址// Base address for prefix 0
    NRF_RADIO->BASE1   = 0x43434343UL;  // 逻辑地址设定 // Base address for prefix 1-7
	  //本射频协议使用通道0传输数据，即地址为0xE7E7E7E7E7
    NRF_RADIO->TXADDRESS   = 0x00UL;      // Set device address 0 to use when transmitting
    NRF_RADIO->RXADDRESSES = 0x01UL;    // Enable device address 0 to use to select which addresses to receive

    // Packet configuration
	//设置S1长度
	//设置S0长度
	//设置LENGTH的长度
	//设置这三个域的长度都为0
    NRF_RADIO->PCNF0 = (0     << RADIO_PCNF0_S1LEN_Pos) |
                       (0     << RADIO_PCNF0_S0LEN_Pos) |
                       (0 << RADIO_PCNF0_LFLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"

    // Packet configuration
	//不使能数据加噪
	//数据高位在先
	//静态地址为4，意味着比LENGTH filed所定义的包长度多四
	//PAYLOAD最大长度为32
    NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos) |    
                       (RADIO_PCNF1_ENDIAN_Big       << RADIO_PCNF1_ENDIAN_Pos)  |
                       (OLD_PACKET_BASE_ADDRESS_LENGTH   << RADIO_PCNF1_BALEN_Pos)   |
                       (OLD_PACKET_STATIC_LENGTH         << RADIO_PCNF1_STATLEN_Pos) |
                       (OLD_PACKET_PAYLOAD_MAXSIZE       << RADIO_PCNF1_MAXLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"
//如果使能相对应的shortcut就不需要将TASK->START设置为1了接收到radio->ready自动会发start事件
#if SHORTCUT_EN == 1
		NRF_RADIO->SHORTS = (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos) |
										(RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos) |
										(RADIO_SHORTS_END_START_Enabled << RADIO_SHORTS_END_START_Pos) |
										(RADIO_SHORTS_DISABLED_RXEN_Enabled << RADIO_SHORTS_DISABLED_RXEN_Pos); //|
//										(RADIO_SHORTS_DISABLED_TXEN_Enabled << RADIO_SHORTS_DISABLED_TXEN_Pos);
#endif		
    // CRC Config
    NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
    if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
    {
        NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value      
        NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
    }
    else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
    {
        NRF_RADIO->CRCINIT = 0xFFUL;        // Initial value
        NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
    }
}


/*射频初始化*/
void Old_Radio_Init(void)
{
    // Set radio configuration parameters.
    radio_configure();
	
	  // Set packet pointer.
    NRF_RADIO->PACKETPTR = (uint32_t)OldPacket;
		
    // Enable END interrupt
    NRF_RADIO->EVENTS_END   = 0;
    NRF_RADIO->INTENCLR     = 0xFFFFFFFFUL;
    NRF_RADIO->INTENSET     |= RADIO_INTENSET_END_Set << RADIO_INTENSET_END_Pos;
	 
    NVIC_ClearPendingIRQ(RADIO_IRQn);
	NVIC_SetPriority(RADIO_IRQn, RADIO_PRIORITY);
    NVIC_EnableIRQ(RADIO_IRQn);
		
}

/*
@Description:公共函数-射频选择
@Input:state :
@Output:无
@Return:无
*/
static void old_radio_select(uint8_t ch)
{
	radio_on();//开启晶振
	radio_tx_carrier(RADIO_MODE_MODE_Nrf_1Mbit,ch);
}


/***********************************************************
@Description:公共函数-周期发送射频信息,并等待发送完成
@Input：wait_send_finish:1:等待发送完成，0:不等待
@Output：无
@Return:无
************************************************************/
void Radio_Old_Period_Send(uint8_t wait_send_finish)
{
	uint32_t ot = 0;
	uint8_t i;
	while(1 == radio_tx_isbusy())
	{
		ot++;
		if(ot > OLD_RADIO_OVER_TIME)
			return;
	}
	Old_Radio_Init();
	memset(OldPacket,0,OLD_PACKET_PAYLOAD_MAXSIZE);

	OldPacket[0] = 0; //payload长度，后续更新
	OldPacket[1] = 0x00;
	memcpy(OldPacket+OLD_TAG_ID_IDX,DeviceID,OLD_RADIO_TID_LENGTH);//ID
	OldPacket[OLD_TAG_STATE_IDX] = ((TAG_STATE.State_LP_Alarm << OLD_TAG_LOWPWR_Pos)&OLD_TAG_LOWPWR_Msk);//低电指示
	OldPacket[7] = 0xFF;
	OldPacket[8] = 0xf0;
	for(i=9;i<32;i++)
	{
		OldPacket[i]= 0x83;
	}
	OldPacket[OLD_PYLOAD_XOR_IDX] = Get_Xor(OldPacket+1,31);//PAYLOAD-XOR
	old_radio_select(2);

	//是否等待发送完成
	if(1 == wait_send_finish)
	{
		while(NRF_RADIO->STATE != RADIO_STATE_STATE_TxIdle)
		{
			ot++;
			if(ot > OLD_RADIO_OVER_TIME)
				break;			
		}
		if(NRF_RADIO->EVENTS_END)
			NRF_RADIO->EVENTS_END = 0;
	}
}
/***********************************************************
@Description:重发
@Input：times：次数
@Output：无
@Return:无
************************************************************/
void Radio_Old_ReSend(uint8_t times)
{
	uint32_t ot = 0;
	while(times--)
	{
		NRF_RADIO->TASKS_TXEN = 1;
		while(NRF_RADIO->STATE != RADIO_STATE_STATE_TxIdle)
		{
			ot++;
			if(ot > OLD_RADIO_OVER_TIME)
				break;			
		}
		if(NRF_RADIO->EVENTS_END)
			NRF_RADIO->EVENTS_END = 0;
	}
}

