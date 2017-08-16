/*
 *****************************************************************************
 * Copyright @ 2011 by austriamicrosystems AG                                *
 * All rights are reserved.                                                  *
 *                                                                           *
 * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
 * THE SOFTWARE.                                                             *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       * 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
 *****************************************************************************
 */
/*
 *      PROJECT:   AS3940 ActiveTag firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file as3933.h
 *
 *  \author Wolfgang Reichart
 *
 *  \brief as3933 driver definition file
 *
 *  This is the definition file for the as3933 driver.
 *  
 */

/*!
 * \defgroup xxxx as3933 driver module
 * some words to describe the module
 */

#ifndef AS3933_H
#define AS3933_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
//#include "platform.h"
#include "sys.h"


/*
******************************************************************************
* DEFINES

******************************************************************************
*/

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

#define AS3933_RES_FREQ_MAX         122000UL
#define AS3933_RES_FREQ_MIN         118000UL

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
extern s8 as3933Initialize (void);
extern s8 as3933Deinitialize (void);
extern s8 as3933Reinitialize (void);
//extern u8 as3933WriteRegister (u8 address, u8 value);
//extern u8 as3933WriteMultiRegister (u8 startAddress, u8 *values, u8 count);
//extern u8 as3933ReadRegister (u8 address, u8 *value);
//extern u8 as3933ReadMultiRegister (u8 startAddress, u8 *values, u8 count);
//extern u8 as3933ModifyRegister (u8 address, u8 mask, u8 value);
//extern u8 as3933SendCommand (as3933Commands_t command);
extern s8 as3933CalibrateRCOViaSPI (void);
extern s8 as3933CalibrateRCOViaLCO (void);
//extern s8 as3933GetStrongestRssi (void);
extern s8 as3933GetStrongestRssi(u8 *rssiX,u8 *rssiY,u8 *rssiZ);

extern s8 as3933DebugRegs (void);
void as3933_Init(void);
/*!
 * capacitor... which bank should be tuned (0... channel 1, 1... channel 2, 2... channel 3)
 * capacity... returns the value of R17, R18 or R19 depending on value of capacitor
 *             the used value for the tuned antenna
 * returns if tuning was ok, or if we were not able to tune the antenna
 */
extern s8 as3933TuneCapacitors (u8 capacitor, u8 *capacity);
extern s8 measureResonanceFrequency (u8 channel, u16 *resonanceFrequency);
extern s8 as3933AntennaTuning (void);

extern s8 as3933SampleData (u32 * sampleData);

#endif /* AS3933_H */
