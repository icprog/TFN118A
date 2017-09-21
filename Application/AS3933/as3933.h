/*******************************************************************************
** 版权:		
** 文件名: 		as3933.h
** 版本：  		1.0
** 工作环境: 	MDK-ARM 5.23
** 作者: 		cc
** 生成日期: 	2017-08-17
** 功能:		  
** 相关文件:	as3933.h
** 修改日志：	
** 版权所有   
*******************************************************************************/
#ifndef AS3933_H
#define AS3933_H
#include "sys.h"

/*------------AS3933------------*/
#define AS3933_CS_PIN_NUM 30     //CS OUT
#define AS3933_SCLK_PIN_NUM 4    //SCL OUT
#define AS3933_SDI_PIN_NUM 5    //SDI-MOSI OUT
#define AS3933_SDO_PIN_NUM 7    //SDO-MISO in/cs低，处于三态
#define AS3933_DAT_PIN_NUM 1    //DAT IN
#define AS3933_CLDAT_PIN_NUM 0  //CL_DAT 
#define AS3933_WAKE_PIN_NUM 2

#define AS3933_SDI_Set (NRF_GPIO->OUTSET=0x01UL<<AS3933_SDI_PIN_NUM)  //SDI=1
#define AS3933_SDI_Clr (NRF_GPIO->OUTCLR=0x01UL<<AS3933_SDI_PIN_NUM)  //SDI=0
#define AS3933_SCLK_Set (NRF_GPIO->OUTSET=0x01UL<<AS3933_SCLK_PIN_NUM)  //SCLK =1;	
#define AS3933_SCLK_Clr (NRF_GPIO->OUTCLR=0x01UL<<AS3933_SCLK_PIN_NUM)  //SCLK =0;
#define AS3933_CS_Set (NRF_GPIO->OUTSET=0x01UL<<AS3933_CS_PIN_NUM)  //CS =1;	
#define AS3933_CS_Clr (NRF_GPIO->OUTCLR=0x01UL<<AS3933_CS_PIN_NUM)  //CS =0;
#define Read_AS3933_SDO  ((NRF_GPIO->IN>>AS3933_SDO_PIN_NUM)&1UL)  //SDO
#define Read_AS3933_DAT  ((NRF_GPIO->IN>>AS3933_DAT_PIN_NUM)&1UL)  //DAT
#define Read_AS3933_CLDAT ((NRF_GPIO->IN>>AS3933_CLDAT_PIN_NUM)&1UL)//CL_DAT
#define Read_AS3933_WAKE ((NRF_GPIO->IN>>AS3933_WAKE_PIN_NUM)&1UL) 


//#include "platform.h"


#define OLED_PWR_Pin_Num  14 
#define OLED_PWR_ON() do{NRF_GPIO->OUTSET = (1 << OLED_PWR_Pin_Num);}while(0)//OLED电源开启
#define OLED_PWR_OFF() do{NRF_GPIO->OUTCLR = (1 << OLED_PWR_Pin_Num);}while(0)//OLED电源关闭
#define OLED_RES_Pin_Num  8
#define OLED_RES_LOW()	do{NRF_GPIO->OUTCLR = (1 << OLED_RES_Pin_Num);}while(0) //复位
#define OLED_RES_HIGH()	do{NRF_GPIO->OUTSET = (1 << OLED_RES_Pin_Num);}while(0)


/* AS3933 Internal Register Address  (Please refer to AS3933 Specifications) */
#define AS3933_REG_R0				0x00		  
#define AS3933_REG_R1				0x01		 
#define AS3933_REG_R2				0x02		  
#define AS3933_REG_R3				0x03		  
#define AS3933_REG_R4				0x04		  
#define AS3933_REG_R5				0x05		  
#define AS3933_REG_R6				0x06		  
#define AS3933_REG_R7				0x07		  
#define AS3933_REG_R8				0x08		  
#define AS3933_REG_R9				0x09		  
#define AS3933_REG_R10				0x0A		  
#define AS3933_REG_R11				0x0B		  
#define AS3933_REG_R12				0x0C		  
#define AS3933_REG_R13				0x0D		  
#define AS3933_REG_R14				0x0E		  
#define AS3933_REG_R15				0x0F		  
#define AS3933_REG_R16				0x10		  
#define AS3933_REG_R17				0x11		  
#define AS3933_REG_R18				0x12		  
#define AS3933_REG_R19				0x13		
#define AS3933_REG_R10				0x0A		  
#define AS3933_REG_R11				0x0B		  
#define AS3933_REG_R12				0x0C		

#define RAW_VALUE_ARRAY_SIZE        4   // number of samples of input capture unit
#define MINIMUM_FREQUENCY_DEVIATION 0x20
#define MAXIMUM_FREQUENCY_DEVIATION 0x5F
#define DESIRED_TRIM_VALUE          30   // 1 MHz / 32.768 kHz, the first frequency small enough which has more rising edges is ok
#define AS393X_NR_OF_REGISTERS      20
#define AS3933_NR_OF_ANTENNAS       3

#define AS3933_RES_FREQ_MAX         130000UL
#define AS3933_RES_FREQ_MIN         120000UL

#define AS3933_R0_val 0x5e	//SCAN
#define AS3933_R0_OFF 0x60



#ifndef MAX
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif
/*
******************************************************************************
* ERRORS
******************************************************************************
*/
/*!\defgroup errorcodes application specific errorcodes start at -50 */
#define ERR_START_FREQ_TOO_HIGH -50 /*!< \ingroup errorcodes
                      the desired frequency can not be reached with the internal capacitors error */
#define ERR_START_FREQ_TOO_LOW -51 /*!< \ingroup errorcodes
                        even without internal capacitors we are already below the desired frequency error */
#define ERR_NO_FREQ_DETECTED -52 /*!< \ingroup errorcodes
                      no frequency was seen on the CL_DAT pin, this can be because of hardware problem
                      (no connection of the antenna) and by this the oscillator does not start */

#define ERR_NOTFOUND -60 /*!< \ingroup errorcodes
                       transponder not found */
#define ERR_NOTUNIQUE -61 /*!< \ingroup errorcodes
                       transponder not unique - more than one transponder in field */
#define ERR_NOTSUPP -62 /*!< \ingroup errorcodes
                          requested operation not supported */

/*! \defgroup errorcodes Basic application error codes
 * Error codes to be used within the application.
 * They are represented by an s8
 */
#define ERR_NONE   0 /*!< \ingroup errorcodes
               no error occured */
#define ERR_NOMEM -1 /*!< \ingroup errorcodes
               not enough memory to perform the requested
               operation */
#define ERR_BUSY  -2 /*!< \ingroup errorcodes
               device or resource busy */
#define ERR_IO    -3 /*!< \ingroup errorcodes
                generic IO error */
#define ERR_TIMEOUT -4 /*!< \ingroup errorcodes
                error due to timeout */
#define ERR_REQUEST -5 /*!< \ingroup errorcodes
                invalid request or requested
                function can't be executed at the moment */
#define ERR_NOMSG -6 /*!< \ingroup errorcodes
                       No message of desired type */
#define ERR_PARAM -7 /*!< \ingroup errorcodes
                       Parameter error */

#define ERR_LAST_ERROR -32
/*
******************************************************************************
* GLOBAL DATATYPES
******************************************************************************
*/
/*! \ingroup as3933
 * possible direct commands
 */
typedef enum
{
    clear_wake      = 0x0,  /*!< clears the wake state of the chip. In case the chip has woken up (WAKE pin is high) the chip is set back to listening mode */
    reset_RSSI      = 0x1,  /*!< resets the RSSI measurement */
    Calib_RCosc     = 0x2, /*!< starts the trimming procedure of the internal RC oscillator */
    clear_false     = 0x3, /*!< resets the false wake up register (R13=00) */
    preset_default  = 0x4, /*!< sets all register in the default mode */
    Calib_RCO_LC    = 0x5, /*!< calibration of the RC-oscillator with the external LC tank */
}as3933Commands_t;

/*! \ingroup as3933
 * data structure to hold antenna tuning results.
 */
typedef struct
{
    u32 resonanceFrequencyOrig;
    u32 resonanceFrequencyTuned;
    u16 rawValue;
    u8  capacitance;
    s8  returnValue;
} as3933AntennaTuningResults;

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/
/*! \ingroup as3933
 *****************************************************************************
 *  \brief  Initializes the as3933 driver module
 *
 *  Initializes the as3933 driver.
 *
 *  \return #ERR_NONE : No error
 *****************************************************************************
 */
#define BASE_TYPE_Pos 16
typedef struct
{
	uint8_t Base_Xor;//异或
	uint8_t Base_Data;//数据
	uint8_t BaseDoor_ID[2];//id
	uint8_t BaseNormal_ID[2];//ID
	uint8_t as3933MAXRSSI;//RSSI
	uint8_t channel1_RSSI;
	uint8_t channel3_RSSI;
}BASE_Typedef;
extern u8 as3933GetStrongestRssi(u8 *rssiChannel1,u8 *rssiChannel3);
extern s8 as3933DebugRegs (void);
void as3933_Init(void);
void as3933_TimeOut(void);
void as3933_inputChangeIsr(void);
void as3933_wakeupIsr(void);

#endif /* AS3933_H */
