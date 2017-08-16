/*******************************************************************************
** 版权:		
** 文件名: 		as3933.c
** 版本：  		1.0
** 工作环境: 	MDK-ARM 5.23
** 作者: 		cc
** 生成日期: 	2017-07-13
** 功能:		  
** 相关文件:	as3933.h
** 修改日志：	
** 版权所有   
*******************************************************************************/
#include "as3933.h"
#include "stdio.h"

#include "as3933_spi.h"
#include "nrf_delay.h"

#define LC_RCO				//通过LC，测量通道一的频率来调节内部RC频率
// #define SPI_RCO					//通过SPI调节内部RC频率
#define XXXX_DEBUG                  0 /*!< set this to 1 to enable debug output */
//验证写入的寄存器
#define AS3933_DEBUG_IO
#ifdef AS3933_DEBUG_IO             
s8 as3933DebugRegs (void);
#endif



/*
******************************************************************************
* CONSTANTS
******************************************************************************
*/
//const u8 as3933RegisterDefaults[][2] =
//{
//    {0x00, 0x6E},/*b7: 16bits,dat mask, b5: ON/OFF mode, b4~2 :channel1~channel3 enable*/
//    {0x01, 0x4A}, /* b6 AGC acting only on the first carrier burst;b3 enable manchester decoding b1 enable pattern detection*/
//    {0x02, 0x20}, /* gain boost ON */
//    {0x03, 0x3F},
//    {0x04, 0xF0}, /* set damper to 27kR (default for auto-tuning) */
//    {0x05, 0x69},
//    {0x06, 0x96},
//    {0x07, 0x3F}, /* was 0x2B */
//    {0x08, 0x00},
//    {0x09, 0x00},
//    {0x10, 0x00},
//    {0x11, 0x00}, /* will be set by antenna tuning routine */
//    {0x12, 0x00}, /* will be set by antenna tuning routine */
//    {0x13, 0x00}  /* will be set by antenna tuning routine */
//};

//const u8 as3933RegisterDefaults[][2] =
//{
//		{0x00, 0x5E},/*b7: 16bits,b6:掩码 b5: ON/OFF mode, b4:scan mode b3~1 :channel1~channel3 enable*/
//    {0x01, 0x4A}, /* b6 AGC acting only on the first carrier burst;b3 enable manchester decoding b1 enable pattern detection*/
//    {0x02, 0x00}, /* b5:gain boost OFF */
//    {0x03, 0xf9}, /*b7迟滞，b6边沿，b<5:3>premble长度 b<2:0>时间常数越长，抗干扰性越强*/
//    {0x04, 0xe0}, /*b7-6 offtime in ON/OFF mode=8ms b11,b5-4天线阻尼电阻器，可以调节Q值，R越小，Q越小，b3-0:reduce gain 0dbm~-24dbm，衰减到-16dbm开始就会出现干扰 */
//    {0x05, 0x69},	//2nd BYTE of wakeup Pattern
//    {0x06, 0x96},	//1st BYTE of wakeup Pattern
//    {0x07, 0x6F}, /* b<7:5>time out时间到后，自动进入监听模式 011：150ms b<4:0>位持续时间 b01111 1bit/16周期*/
//    {0x08, 0x00}, /*b<7:5>工作频率范围 00 95-150 N=4;b<2:0>不自动唤醒*/
//    {0x09, 0x00}, /*如果RSSI检测，需要使能AGC,否则RSSI值为0*/
//    {0x10, 0x00}, /*R16,*/
//    {0x11, 0x00}, /* will be set by antenna tuning routine */
//    {0x12, 0x00}, /* will be set by antenna tuning routine */
//    {0x13, 0x00}  /* will be set by antenna tuning routine */
//};


const u8 as3933RegisterDefaults[][2] =
{
    {0x00, 0x56},/*0 1 0 1 1 1 1 0 b7: Pattern 16bits,dat mask, b5: ON/OFF mode, b4:Scan mode enable b3~1 :Ch2 Ch3 Ch1 enable*/
    {0x01, 0x4A}, /*0 1 0 0 1 0 1 0 b6 AGC acting only on the first carrier burst;b3 enable manchester decoding b1 enable pattern detection*/
    {0x02, 0x00}, /*0 0 0 0 00 00gain boost ON */
    {0x03, 0x3b},//0 0 111 011 b<2:0>symbol rate b111 512 b011 1130 b001 2184, premble b<5:3> 111
    {0x04, 0xF0}, /*11 11 0000 set damper to 27kR (default for auto-tuning) */
    {0x05, 0x69},
    {0x06, 0x96},
    {0x07, 0x6F}, /*011  01111 b<7:5>time out b'001,50ms b'011 150ms  b'010 100ms B<4:0>01111 16周期 b<4:0>b'11111 32周期*/
    {0x08, 0x00},//000 00 000
    {0x09, 0x00},//0 0000000
    {0x10, 0x00},
    {0x11, 0x00}, /* will be set by antenna tuning routine */
    {0x12, 0x00}, /* will be set by antenna tuning routine */
    {0x13, 0x00}  /* will be set by antenna tuning routine */
};
/*
******************************************************************************
* GLOBAL VARIABLES
******************************************************************************
*/
//u8 as3933RegBackup[AS393X_NR_OF_REGISTERS];	/*!< AS3933 register backup memory */
//spiConfig_t spi1Config;
u8 as3933DebugBuffer[64];     //临时缓存寄存器
/* for debugging, these values are used in function as3933TuneCapacitors() */
u16 rawValueArray[RAW_VALUE_ARRAY_SIZE];//采集4次DAT引脚的频率
u16 cBothEdgeCount, rawValue, delayLoop;//边沿个数，记录边沿个数，延时
/*
 * as3933TuneResults[0] ... results for X-coil
 * as3933TuneResults[1] ... results for Y-coil
 * as3933TuneResults[2] ... results for Z-coil
 */
as3933AntennaTuningResults as3933TuneResults[AS3933_NR_OF_ANTENNAS];
//volatile u32 as3933LfSampleData;
//u8 as3933LfCurrentBit;
//volatile u16 as3933LfSampleBitCount;
//volatile BOOL as3940LfSampleActive;

   
#ifdef AS3933_DEBUG_RSSI			//显示信号强度
#define AS3933_NUM_TOTAL_WAKEUPS    256
u16 as3933TotalWakeups;
u16 as3933TotalNumRssiX;
u16 as3933TotalNumRssiY;
u16 as3933TotalNumRssiZ;
#endif


/*
******************************************************************************
* GLOBAL FUNCTION
******************************************************************************
*/
s8 as3933Initialize()
{
    s8 retVal = ERR_NONE;
    u8 i;
    //如果使用spi，要设置spi速度

#ifdef AS3933_DEBUG_RSSI
    as3933TotalWakeups  = 0;
    as3933TotalNumRssiX = 0;
    as3933TotalNumRssiY = 0;
    as3933TotalNumRssiZ = 0;
#endif
    // get the register defaults on AS3933
    as3933SendCommand(preset_default);
    // clear any pending wake IRQ
    as3933SendCommand(clear_wake);
    // write register defaults to relevant registers
    for (i = 0; i < sizeof(as3933RegisterDefaults)/sizeof(as3933RegisterDefaults[0]); i++)
    {
        as3933WriteRegister(as3933RegisterDefaults[i][0], as3933RegisterDefaults[i][1]);
    }
#ifdef AS3933_DEBUG_IO
    if (as3933DebugRegs() < 0)
    {
        retVal = ERR_IO;
    }
#endif
    return retVal;
}

#ifdef AS3933_DEBUG_IO
s8 as3933DebugRegs (void)
{
    u8 i;
    u8 regVal = 0;
    s8 retVal = 0;

    for (i = 0; i < sizeof(as3933RegisterDefaults)/sizeof(as3933RegisterDefaults[0]); i++)
    {
        as3933ReadRegister(as3933RegisterDefaults[i][0], &regVal);
        if (regVal != as3933RegisterDefaults[i][1])
        {
            retVal = -1;
        }
    }

    return retVal;
}
#endif

s8 as3933Deinitialize()
{
    return ERR_NONE;
}

s8 as3933Reinitialize(void)
{
    return ERR_NONE;
}

void msSleep(u8 cnt)
{
	nrf_delay_ms(cnt);
}
/*
******************************************************************************
* 通过LC调节内部RC
******************************************************************************
*/
s8 as3933CalibrateRCOViaLCO (void)
{
#ifdef LC_RCO
    u8 regVal = 0;
    u8 registerFour, registerOne;
    s8 retVal = -1;

    /*
     * before we start the calibration of the RCO with the LC antenna
     * make sure that the antenna damper is enabled on 27kOhm
     */
    as3933ReadRegister(0x04, &registerFour); //读取R4
    as3933DebugBuffer[0] = registerFour;
	as3933ModifyRegister(0x04, 0x30, 0x30); //修改R4<5:4>位为11
    as3933ReadRegister(0x04, &as3933DebugBuffer[1]);

    as3933ReadRegister(0x01, &registerOne);//读取R1寄存器
    as3933DebugBuffer[2] = registerOne;//
    as3933ModifyRegister(0x01, 0x10, 0x10);//修改R1<4>=1使能天线阻尼电阻
    as3933ReadRegister(0x01, &as3933DebugBuffer[3]);

    /* wait until the damper is effective, how long do we have to wait? */
    msSleep(5);
    /* send direct command calib_rco_lc */
    as3933SendCommand(Calib_RCO_LC);
    /* wait until the damper is effective, how long do we have to wait? */
    msSleep(15);

    /* revert registers 0x01 and 0x04 to their previous values */
    as3933WriteRegister(0x01, registerOne); //写回原值
    as3933WriteRegister(0x04, registerFour);//写回原值
    
    /* check status of calibration procedure in R14[6] (OK) and R14[7] (NOK) */
    /* check status of calibration procedure in R14[6] (NOK) and R14[7] (OK) */
		
    as3933ReadRegister(0x0e, &regVal);
    as3933DebugBuffer[4] = regVal;
    //检测R14<7>是否为高，R14<6>是否为低 
    if ((regVal & 0x80) && !(regVal & 0x40))
    {
        retVal = 0;
    }
    as3933DebugBuffer[5] = retVal;
    return retVal;
#endif
}


/*
******************************************************************************
* 通过RC调节内部RC
******************************************************************************
*/
s8 as3933CalibrateRCOViaSPI (void)
{
#ifdef SPI_RCO
//    extern void asmFunction(void);   // located in as3933_helper.s
    u8 outbuf;
		u32 i;
		u8 regVal;
//	  u8 retVal;
//    spiInitialize(SPI1, &spi1Config);

//    outbuf = 0xC0 | (Calib_RCosc & 0x3F);   // command mode
//		AS3933_CS_Set;   // set CS
//    spiTransferNBytes(SPI1, &outbuf, NULL, 1, TRUE);
		outbuf = 0xC0 | (Calib_RCosc & 0x3F);   // command mode
		AS3933_CS_Set;//CS = 1;
		SPI_RW(outbuf);
	
		for(i=0;i<70;i++)
		{
			nrf_delay_us(16);
			AS3933_SCLK_Set;
			nrf_delay_us(16);
			AS3933_SCLK_Clr;
		}
		as3933ReadRegister(0x0e, &regVal);
   
//    /* free SCK1 pin to have GPIO access to it */
//    RPOR5bits.RP11R = 0;
//    REFERENCE_CLOCK_PIN = 0;
//    //LATBbits = 0;

//    /* disable interrupts here. we must not get interrupted */
//    IRQ_INC_DISABLE();

//    // send 32.768 kHz reference clock on SCL
//    // XXX: assume we are running on 16 MHz
//    asmFunction();
      


//    /* enable interrupts again */
//    IRQ_DEC_ENABLE();

//    CS = 0;   // clear CS

//    spiDeinitialize(SPI1);

//    /* route pin RP11 (RB11 / Pin 19) to SCK1OUT */
//    RPOR5bits.RP11R = 8;
#endif
    return ERR_NONE;
}

/*
******************************************************************************
* 获取信号强度
******************************************************************************
*/
s8 as3933GetStrongestRssi(u8 *rssiX,u8 *rssiY,u8 *rssiZ)
{
    u8 rssiMax;   // rssi is only 5 bit, so signed data type is ok

    as3933ReadRegister(10, rssiX);
    *rssiX &= 0x1F;   // 5 bit value in register
    //SET_DBG_BUF(rssiX);

    as3933ReadRegister(11, rssiY);
    *rssiY &= 0x1F;   // 5 bit value in register
    //SET_DBG_BUF(rssiY);

    as3933ReadRegister(12, rssiZ);
    *rssiZ &= 0x1F;   // 5 bit value in register
    //SET_DBG_BUF(rssiZ);

    as3933SendCommand(clear_wake);
    // calculate strongest RSSI
    rssiMax = MAX(*rssiX, *rssiY);
    rssiMax = MAX(*rssiZ, rssiMax);
    //SET_DBG_BUF(rssiMax);

#ifdef AS3933_DEBUG_RSSI
    as3933TotalWakeups++;
    if (rssiX) as3933TotalNumRssiX++;
    if (rssiY) as3933TotalNumRssiY++;
    if (rssiZ) as3933TotalNumRssiZ++;
    if (as3933TotalWakeups > AS3933_NUM_TOTAL_WAKEUPS)
    {
//        while (1);
			as3933TotalWakeups = 0;
    }
#endif
    
    return rssiMax;
}



/*
 * XXX: the GUI relies on measureResonanceFrequency to be ran @ 16 MHz
 * since this function is only called via GUI when connected to USB this function is running when PIC is @ 16 MHz
 * measureResonanceFrequency itself has no dependency on the frequency
 */
//u8 measureResonanceFrequency(u8 channel, u16 *resonanceFrequency)
//{
//    u8 retVal;
//    u16 cBothEdgeCount, delayLoop, rawValue, rawValueArray[RAW_VALUE_ARRAY_SIZE];
//    u32 cIC2TmrOverflowValue, noTimeout, currSysFreq;
//    u8 rawValueCounter;

//    retVal = ERR_NONE;
//    *resonanceFrequency = 0;
//    rawValue = 0;
//    rawValueArray[0] = 0;
//    rawValueArray[1] = 0;
//    rawValueArray[2] = 0;
//    rawValueArray[3] = 0;

//    getCurrentSystemFrequency(&currSysFreq);
//    cIC2TmrOverflowValue = currSysFreq >> 7;
//    cBothEdgeCount = currSysFreq / 125000;   // 125000 Hz is the desired frequency
//    cBothEdgeCount <<= 6;   // we take 16 samples * 4 times (Capture 1 Module rising edges)

//    as3933WriteRegister(16, 1 << (channel-1));   // connects LF1P to the LCO (display on DAT pin)

//    // clear the FIFO
//    while(IC2CONbits.ICBNE)
//        rawValue = IC2BUF;
//    IC2CONbits.ICTMR = 0;

//    // wait 64 pulses until the oscillator of the AS3933 is stable
//    delayLoop = cBothEdgeCount;
//    while(delayLoop--);

//    IFS0bits.IC2IF = 0;   // clear pending interrupts
//    IC2CON = 0x1C65;   // turn on Input Capture 1 Module, 16 samples, 4 times
//    //IC2CON2 = 0x0040;   // start timer
//    noTimeout = cIC2TmrOverflowValue;

//    while((IFS0bits.IC2IF != 1) && (noTimeout--));   // wait for FIFO to be filled, or overflow if no frequency can be detected

//    if(noTimeout)   // no overflow, all ok (IC2CON1bits.ICOV can not be used for this, it seems as if reading IC2TMR is also not working)
//    {
//        if(IC2CONbits.ICBNE)
//        {
//            rawValueCounter = 0;
//            while(IC2CONbits.ICBNE)
//            {
//                if(rawValueCounter < RAW_VALUE_ARRAY_SIZE)
//                {
//                    rawValueArray[rawValueCounter] = IC2BUF;
//                }
//                else
//                {
//                    // to get the ICBNE bit cleared we do a dummy readout into index 0 which we anyhow never use since it is not synced, and thus worthless
//                    rawValueArray[0] = IC2BUF;
//                }
//                rawValueCounter++;
//            }

//            *resonanceFrequency = rawValueArray[RAW_VALUE_ARRAY_SIZE - 1] - rawValueArray[RAW_VALUE_ARRAY_SIZE - 2];
//        }
//        else   // why empty? no frequency? but then overflow is already set, so is this needed here too? or could be something else?
//        {
//            retVal = ERR_NO_FREQ_DETECTED;
//        }
//    }
//    else   // we detected an overflow
//    {
//        retVal = ERR_NO_FREQ_DETECTED;   // there is no frequency at CL_DAT pin -> hardware problem? (antenna not connected?)
//    }

//    IC2CON = 0;   // turn off Input Capture 2 Module

//    // we leave the LF1P on CL_DAT

//    return retVal;
//}








//4个数去掉最大值，最小值，然后取平均。
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
	sum = sum>>1;
	average1 = sum;
	return average1;
}



#define tim0_5ms
#define tim0Value 20000   //20ms
#define adjust_frq  120    //最终调谐的频率，单位K
#define Single 1
#if Single
/************************************************* 
@Description:单路天线调弦，增加电容，减少频率
@Input:无
@Output:
@Return:无
*************************************************/ 
void as3933TuneCapacitors(u8 capacitor)
{
    u8 capacitor;//通道数
//	u8 capacity;
    s8 retVal = 0;
	s8 origFreqDone,dat_level,old_AS3933_DAT;
    u8 capacitorValue, shadowRegisterOne, shadowRegisterFour;
    u32 cIC2TmrOverflowValue, noTimeout;
//	u32 currSysFreq;
	u8 rawValueCounter;// 采样值计数
	u16* prawValueArray;//四次采样值
	u16 cBothEdgeCount1,tim0_cc;//采样边沿个数
	u8 fre_pll;
	u8 test_reg;
    /* we need to enable antenna damper at 27 kOhm to avoid cross coupling */
    as3933ReadRegister(4, &shadowRegisterFour);
    as3933WriteRegister(4, shadowRegisterFour | 0x30); //设置阻尼电阻为27K
    as3933ReadRegister(1, &shadowRegisterOne);
    as3933WriteRegister(1, shadowRegisterOne | 0x10); //使能阻尼电阻
	msSleep(15);
	#ifdef tim0_10ms
		NRF_TIMER0->PRESCALER=4UL;	
		NRF_TIMER0->MODE=TIMER_MODE_MODE_Timer;
		cBothEdgeCount1 = adjust_frq*20;
		tim0_cc = 10000;
		fre_pll=50;
	#endif
	#ifdef tim0_5ms
		NRF_TIMER0->PRESCALER=4UL;	
		NRF_TIMER0->MODE=TIMER_MODE_MODE_Timer;
		cBothEdgeCount1 = adjust_frq*10; //5ms，共有1180个边沿
		tim0_cc = 5000;//定时器5ms
		fre_pll=100;//采样的采样频率倍数
	#endif
		
	origFreqDone = 0;

	cIC2TmrOverflowValue = adjust_frq*1000;//118Khz

	//NRF51822采用20ms内检测到的边沿个数，125K 20MS/4 =5000
	//边沿计数			
	cBothEdgeCount = cBothEdgeCount1;//118K 20000US/(1000/118US)*2 4720个边沿
	capacitorValue = 0x00;
	as3933WriteRegister(17 + capacitor, capacitorValue);

	as3933WriteRegister(16, 1 << capacitor);   // Connects LFxP to the LCO (display on DAT pin)
	as3933ReadRegister(16, &test_reg);
	retVal = ERR_BUSY;
	while (retVal == ERR_BUSY)   // we start the calibration
	{
		
		rawValue = 0;
		// when antenna damper is enabled it takes quite long to start/change the oscillator
		msSleep(10);//等待晶振稳定
					
		noTimeout = cIC2TmrOverflowValue;

		old_AS3933_DAT = AS3933_DAT;
		while(old_AS3933_DAT == AS3933_DAT && noTimeout--);//电平相等
				
		if(noTimeout) 
		{
			rawValueArray[0] = 0;
			rawValueArray[1] = 0;
			rawValueArray[2] = 0;
			rawValueArray[3] = 0;
			//采样四次，取平均值
			for(rawValueCounter=0;rawValueCounter < RAW_VALUE_ARRAY_SIZE;rawValueCounter++)
			{
				NRF_TIMER0->TASKS_STOP    = 1; // Stop timer.
				NRF_TIMER0->TASKS_CLEAR    = 1; 
				NRF_TIMER0->CC[0]          = tim0_cc;//5ms
				
				rawValue = 0;
				while(1 == AS3933_DAT); //等待电平置0
				
				dat_level=1;	
				NRF_TIMER0->EVENTS_COMPARE[0]=0;//clear
				NRF_TIMER0->TASKS_START    = 1; // Start timer.
				while(NRF_TIMER0->EVENTS_COMPARE[0]==0)
				{
					if(AS3933_DAT==dat_level)//if(DAT==dat_level)
					{
						if(dat_level)dat_level=0;
						else dat_level=1;
						rawValue++; //边沿个数
					}	
				}		
				NRF_TIMER0->TASKS_STOP    = 1; // Stop timer.
				rawValueArray[rawValueCounter] = rawValue;
			}
			prawValueArray = rawValueArray;
			rawValue = average(prawValueArray);
			as3933TuneResults[capacitor].rawValue = rawValue;
			//未添加电容时的，频率
			if ((origFreqDone == 0) && (rawValue > 0))
			{
					origFreqDone = 1;
					//rawValue*1000/(tim0Value*2)*1000
					as3933TuneResults[capacitor].resonanceFrequencyOrig = rawValue*fre_pll;
			}
			if(rawValue > cBothEdgeCount)  //频率大
			{
				capacitorValue++;
				if(capacitorValue > 0x1F)
				{
						retVal = ERR_START_FREQ_TOO_HIGH;   // we were not able to tune, antenna trimming out of range
						capacitorValue = 0x1F;
						as3933WriteRegister(17 + capacitor, capacitorValue);
				}
				else
				{
						as3933WriteRegister(17 + capacitor, capacitorValue);//增大电容减少频率
				}
			}
			else if(capacitorValue == 0)
				retVal = ERR_START_FREQ_TOO_LOW;   // we were not able to tune, antenna trimming out of range
			else
				retVal = ERR_NONE;
		}
		else   // we detected an overflow
		{
			retVal = ERR_NO_FREQ_DETECTED;   // there is no frequency at CL_DAT pin -> hardware problem? (antenna not connected?)
		}
	}

//		capacity = capacitorValue;
	/* store results in result struct */
	//添加电容之后的频率
	if (rawValue > 0)
	{
			//rawValue*1000/(tim0Value*2)*1000
			as3933TuneResults[capacitor].resonanceFrequencyTuned = rawValue*fre_pll;
	}
	as3933TuneResults[capacitor].capacitance  = capacitorValue;
	as3933TuneResults[capacitor].returnValue  = retVal;
	// make sure to remove LFxP from the LCO, otherwise wake up won't work anymore
	as3933WriteRegister(16, 0x00);
	/* set antenna damper back to user defined value */
	as3933WriteRegister(1, shadowRegisterOne);
	as3933WriteRegister(4, shadowRegisterFour);
    return retVal;
}

/************************************************* 
@Description:对指定的通道进行调弦
@Input:无
@Output:
@Return:无
*************************************************/ 
s8 as3933AntennaTuning (void)
{
   u8 retVal = 0;
   as3933TuneCapacitors(0);//通道1
   as3933TuneCapacitors(2);//通道3 
   return retVal;
}

#else 
/************************************************* 
@Description:3通道天线调弦，增加电容，减少频率
@Input:无
@Output:
@Return:无
*************************************************/ 
s8 as3933AntennaTuning (void)
{
    u8 capacitor;//通道数
//	u8 capacity;
    s8 retVal = 0;
	s8 origFreqDone,dat_level,old_AS3933_DAT;
    u8 capacitorValue, shadowRegisterOne, shadowRegisterFour;
    u32 cIC2TmrOverflowValue, noTimeout;
//	u32 currSysFreq;
	u8 rawValueCounter;// 采样值计数
	u16* prawValueArray;//四次采样值
	u16 cBothEdgeCount1,tim0_cc;//采样边沿个数
	u8 fre_pll;
	u8 test_reg;
    /* we need to enable antenna damper at 27 kOhm to avoid cross coupling */
    as3933ReadRegister(4, &shadowRegisterFour);
    as3933WriteRegister(4, shadowRegisterFour | 0x30); //设置阻尼电阻为27K
    as3933ReadRegister(1, &shadowRegisterOne);
    as3933WriteRegister(1, shadowRegisterOne | 0x10); //使能阻尼电阻
	msSleep(15);
	#ifdef tim0_10ms
		NRF_TIMER0->PRESCALER=4UL;	
		NRF_TIMER0->MODE=TIMER_MODE_MODE_Timer;
		cBothEdgeCount1 = adjust_frq*20;
		tim0_cc = 10000;
		fre_pll=50;
	#endif
	#ifdef tim0_5ms
		NRF_TIMER0->PRESCALER=4UL;	
		NRF_TIMER0->MODE=TIMER_MODE_MODE_Timer;
		cBothEdgeCount1 = adjust_frq*10; //5ms，共有1180个边沿
		tim0_cc = 5000;//定时器5ms
		fre_pll=100;//采样的采样频率倍数
	#endif
	for (capacitor = 0; capacitor < 3; capacitor++)
    {
		origFreqDone = 0;

		cIC2TmrOverflowValue = adjust_frq*1000;//118Khz

		//NRF51822采用20ms内检测到的边沿个数，125K 20MS/4 =5000
        //边沿计数			
		cBothEdgeCount = cBothEdgeCount1;//118K 20000US/(1000/118US)*2 4720个边沿
		capacitorValue = 0x00;
		as3933WriteRegister(17 + capacitor, capacitorValue);

		as3933WriteRegister(16, 1 << capacitor);   // Connects LFxP to the LCO (display on DAT pin)
		as3933ReadRegister(16, &test_reg);
		retVal = ERR_BUSY;
		while (retVal == ERR_BUSY)   // we start the calibration
		{
			
			rawValue = 0;
			// when antenna damper is enabled it takes quite long to start/change the oscillator
			msSleep(10);//等待晶振稳定
						
			noTimeout = cIC2TmrOverflowValue;

			old_AS3933_DAT = AS3933_DAT;
			while(old_AS3933_DAT == AS3933_DAT && noTimeout--);//电平相等
					
			if(noTimeout) 
			{
				rawValueArray[0] = 0;
				rawValueArray[1] = 0;
				rawValueArray[2] = 0;
				rawValueArray[3] = 0;
				//采样四次，取平均值
				for(rawValueCounter=0;rawValueCounter < RAW_VALUE_ARRAY_SIZE;rawValueCounter++)
				{
					NRF_TIMER0->TASKS_STOP    = 1; // Stop timer.
					NRF_TIMER0->TASKS_CLEAR    = 1; 
					NRF_TIMER0->CC[0]          = tim0_cc;//5ms
					
					rawValue = 0;
					while(1 == AS3933_DAT); //等待电平置0
					
					dat_level=1;	
					NRF_TIMER0->EVENTS_COMPARE[0]=0;//clear
					NRF_TIMER0->TASKS_START    = 1; // Start timer.
					while(NRF_TIMER0->EVENTS_COMPARE[0]==0)
					{
						if(AS3933_DAT==dat_level)//if(DAT==dat_level)
						{
							if(dat_level)dat_level=0;
							else dat_level=1;
							rawValue++; //边沿个数
						}	
					}		
					NRF_TIMER0->TASKS_STOP    = 1; // Stop timer.
					rawValueArray[rawValueCounter] = rawValue;
				}
				prawValueArray = rawValueArray;
				rawValue = average(prawValueArray);
				as3933TuneResults[capacitor].rawValue = rawValue;
				//未添加电容时的，频率
				if ((origFreqDone == 0) && (rawValue > 0))
				{
						origFreqDone = 1;
						//rawValue*1000/(tim0Value*2)*1000
						as3933TuneResults[capacitor].resonanceFrequencyOrig = rawValue*fre_pll;
				}
				if(rawValue > cBothEdgeCount)  //频率大
				{
					capacitorValue++;
					if(capacitorValue > 0x1F)
					{
							retVal = ERR_START_FREQ_TOO_HIGH;   // we were not able to tune, antenna trimming out of range
							capacitorValue = 0x1F;
							as3933WriteRegister(17 + capacitor, capacitorValue);
					}
					else
					{
							as3933WriteRegister(17 + capacitor, capacitorValue);//增大电容减少频率
					}
				}
				else if(capacitorValue == 0)
					retVal = ERR_START_FREQ_TOO_LOW;   // we were not able to tune, antenna trimming out of range
				else
					retVal = ERR_NONE;
			}
			else   // we detected an overflow
			{
				retVal = ERR_NO_FREQ_DETECTED;   // there is no frequency at CL_DAT pin -> hardware problem? (antenna not connected?)
			}
		}

//		capacity = capacitorValue;
		/* store results in result struct */
		//添加电容之后的频率
		if (rawValue > 0)
		{
				//rawValue*1000/(tim0Value*2)*1000
				as3933TuneResults[capacitor].resonanceFrequencyTuned = rawValue*fre_pll;
		}
		as3933TuneResults[capacitor].capacitance  = capacitorValue;
		as3933TuneResults[capacitor].returnValue  = retVal;
		// make sure to remove LFxP from the LCO, otherwise wake up won't work anymore
		as3933WriteRegister(16, 0x00);
	}
	/* set antenna damper back to user defined value */
	as3933WriteRegister(1, shadowRegisterOne);
	as3933WriteRegister(4, shadowRegisterFour);
    return retVal;
}
#endif
//u8 as3933SampleData (u32 * sampleData)
//{
//    u8 retVal = ERR_NONE;

//    SetPriorityIntInputChange(5);
//    EnableCN13;
//    EnablePullUpCN13;
//    InputChange_Clear_Intr_Status_Bit;
//    EnableIntInputChange;

//    as3933LfSampleBitCount = 0;
//    as3933LfSampleData     = 0;
//    as3940LfSampleActive   = TRUE;
//    //SET_DBG_0();
//    while (as3940LfSampleActive);
//    //CLR_DBG_0();
//    *sampleData = as3933LfSampleData;

//    DisableIntInputChange;
//    DisableCN13;
//    DisablePullUpCN13;
//    
//    return retVal;
//}

//void INTERRUPT inputChangeIsr(void)
//{
//    /*
//     * check the polarity of the clock pin;
//     * data can be sampled on rising edge -> when pin is high take the data from
//     * the
//     */
//    if (AS3933_CL_DAT_PIN)
//    {
//        if (as3933LfSampleBitCount < AS3933_NUM_BITS_TO_SAMPLE)
//        {
//            as3933LfSampleData <<= 1;
//            as3933LfCurrentBit = (AS3933_DAT_PIN ? 1 : 0);
//            as3933LfSampleData |= as3933LfCurrentBit;
//            as3933LfSampleBitCount++;
//        }
//        else
//        {
//            as3940LfSampleActive = FALSE;
//        }
//    }
//    else
//    {
//        if (as3933LfSampleBitCount >= AS3933_NUM_BITS_TO_SAMPLE)
//        {
//            as3940LfSampleActive = FALSE;
//        }
//    }
//    InputChange_Clear_Intr_Status_Bit;
//}

extern uint8_t	aucPayload[32];
extern uint8_t TagID[4];
extern void radio_tx_carrier(uint8_t txpower, uint8_t channel);
extern uint8_t data_channel;//射频发射频率
void as3933_Init(void)
{
	  uint8_t i,as3933_j,rf_xor;
    /* initialize the AS3933 wakeup receiver */
    if (as3933Initialize() < 0)
    {
		LED_ON;
		while(1);
    }

    /* calibrate the AS3933 RCO using the (not trimmed and therefore <1% accuracy) PIC RCO for calibration */
    as3933CalibrateRCOViaSPI();
    /* calibrate the AS3933 RCO using LC calibration */
    if (as3933CalibrateRCOViaLCO() < 0)
    {
		LED_ON;
        while(1);	
    }
    /* tune the antennas */
    as3933AntennaTuning();
    /* check tuning results and display error if any antenna is out of range */
	as3933_j = 0;
    for (i = 0; i < AS3933_NR_OF_ANTENNAS; i++)
    {
        if ((as3933TuneResults[i].resonanceFrequencyTuned > AS3933_RES_FREQ_MAX) ||
            (as3933TuneResults[i].resonanceFrequencyTuned < AS3933_RES_FREQ_MIN))
        {
			as3933_j++;
        }
    }
	if(as3933_j>0)
	{
		LED_ON;
		while(1)
		{
			s3933TuneResults[0].resonanceFrequencyOrig/1000;
			as3933TuneResults[1].resonanceFrequencyOrig/1000;
			as3933TuneResults[2].resonanceFrequencyOrig/1000;
		}
	}
	LED_OFF;
}


void INTERRUPT inputChangeIsr(void)
{
    /*
     * check the polarity of the clock pin;
     * data can be sampled on rising edge -> when pin is high take the data from
     * the
     */
    if (AS3933_CL_DAT_PIN)
    {
        if (as3933LfSampleBitCount < AS3933_NUM_BITS_TO_SAMPLE)
        {
            as3933LfSampleData <<= 1;
            as3933LfCurrentBit = (AS3933_DAT_PIN ? 1 : 0);
            as3933LfSampleData |= as3933LfCurrentBit;
            as3933LfSampleBitCount++;
        }
        else
        {
            as3940LfSampleActive = FALSE;
        }
    }
    else
    {
        if (as3933LfSampleBitCount >= AS3933_NUM_BITS_TO_SAMPLE)
        {
            as3940LfSampleActive = FALSE;
        }
    }
    InputChange_Clear_Intr_Status_Bit;
}