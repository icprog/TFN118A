/*
 *****************************************************************************
 * Copyright by ams AG                                                       *
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
 *      PROJECT:   AS7000
 *      LANGUAGE:  ANSI C
 */

/*! \file lis2dh12.c
 *
 *  \author M. Zimmerman
 *
 *  \brief lis2dh12 accelerometer-control
 *
 */

/*
 *****************************************************************************
 * INCLUDES
 *****************************************************************************
 */
#include "lis2dh12.h"
#include "i2c.h"
#include "sys.h"


/*
 *****************************************************************************
 * DEFINES
 *****************************************************************************
 */

/* the ADXL345 provides the data in 2 8-bit values -> need 1 16-bit value */
#define CONVERT_MSB_LSB_TO_SINT16( msb, lsb ) \
    ( (int16_t)( (((uint16_t)(msb)) << 8 ) | (((uint16_t)(lsb)) & 0xFF) ) )

#define LIS2DH12_I2C_SLAVE_ADDRESS      0x18    /* SA0 pin of LIS2DH12 must be low */
#define LIS2DH12_I2C_SLAVE_ADDRESS_ALT  0x19    /* alt address: SA0 pin of LIS2DH12 must be high */
//#define LIS2DH12_I2C_SLAVE_ADDRESS 0x30
//#define LIS2DH12_I2C_SLAVE_ADDRESS_ALT  0x32

/*
 *****************************************************************************
 * VARIABLES
 *****************************************************************************
 */

/* default is the lis2dh12I2cAddress - try alternate only when this one fails */
static uint8_t lis2dh12I2cAddress = LIS2DH12_I2C_SLAVE_ADDRESS<<1;

/*
 *****************************************************************************
 * FUNCTIONS
 *****************************************************************************
 */

static uint8_t lis2dh12SendCommand(uint8_t reg, uint8_t value)
{
	
	  uint8_t txData[2];
    txData[0] = reg;
    txData[1] = value;
		return (ERR_NONE==I2C_SendDates(lis2dh12I2cAddress,txData,2));
}

static uint8_t lis2dh12ReadRegisters(uint8_t reg, uint8_t count, uint8_t *data)
{
		return (ERR_NONE==I2C_ReadDatas(lis2dh12I2cAddress,reg,data,count));   
}

/* LIS IC returns accelerometer data with one unit of ~ 0.75mg (we want to
   standardize the units of the accelerometer data to be ~0.5mg 
   And indicate this change of original data by setting the lsb bit to 1 and
   clearing bits 1,2,3 to 0 */
static int16_t lis2dh12Normalise ( int16_t sample )
{
    int32_t s = sample;
    s = ( s * 3 ) / 2; /* convert from 0.75 mg to 0.5 mg (=multiply by 1.5)*/ 
    if ( s < INT16_MIN ) /* saturate */
    {
        s = INT16_MIN;
    } 
    else if ( s > INT16_MAX ) /* saturate */
    {
        s = INT16_MAX;
    }
    sample = (int16_t)s;
    sample = ( sample & 0xFFF0 ) | 1; /* indicate sample change by lsb bit 1 */
    return sample;
}

static uint8_t lis2dh12OpenInternal(uint8_t sampleReadMode, uint16_t *sampleFrequencyMilliHz)
{
    uint8_t rxData; 

    /* configure the lis2dh12 */
    *sampleFrequencyMilliHz = 0;
    if (lis2dh12ReadRegisters(LIS2DH12_WHO_AM_I, 1, &rxData) != 1)
        return 0;
    if (rxData != LIS2DH12_WHO_AM_I_VALUE) /* validate the correct device is present */
        return 0;
    if (lis2dh12SendCommand(LIS2DH12_CTRL_REG1, 0x00) != 1) /* no measurement -> to allow configuration of lis2dh12 */
        return 0;
    if (lis2dh12SendCommand(LIS2DH12_CTRL_REG4, 0x80 | 0x30 | 0x08) != 1)  /* BDU, 16g, high resolution mode */
        return 0;
    if (lis2dh12SendCommand(LIS2DH12_CTRL_REG5, 0x00) != 1) /* FIFO-disabled */
        return 0;
    if (lis2dh12SendCommand(LIS2DH12_FIFO_CTRL_REG, 0x00) != 1) /* FIFO-bypass (clears the FIFO) */
        return 0;
    if (sampleReadMode)
    {
        if (lis2dh12SendCommand(LIS2DH12_CTRL_REG1, (LIS2DH12_DATA_RATE_200_HZ) | 0x7) != 1) /* select bandwith, all 3 axis */
            return 0;
    }
    else
    {
        if (lis2dh12SendCommand(LIS2DH12_CTRL_REG5, 0x40) != 1) /* FIFO-enabled */
            return 0;
        if (lis2dh12SendCommand(LIS2DH12_FIFO_CTRL_REG, 0x40) != 1) /* FIFO-mode enabled */
            return 0;
        if (lis2dh12SendCommand(LIS2DH12_CTRL_REG1, (LIS2DH12_DATA_RATE_10_HZ) | 0x7) != 1) /* select bandwith, all 3 axis */
            return 0;
        *sampleFrequencyMilliHz = 10000; // 10.000Hz
    }
    return 1;
}

uint8_t lis2dh12Open(uint8_t sampleReadMode, uint16_t *sampleFrequencyMilliHz)
{
    lis2dh12I2cAddress = LIS2DH12_I2C_SLAVE_ADDRESS; /* initial slave-address */
    if (lis2dh12OpenInternal(sampleReadMode, sampleFrequencyMilliHz) == 1)
        return 1;
    lis2dh12I2cAddress = LIS2DH12_I2C_SLAVE_ADDRESS_ALT; /* alternate slave-address */
    return lis2dh12OpenInternal(sampleReadMode, sampleFrequencyMilliHz);
}    

void lis2dh12Close(void)
{
    lis2dh12SendCommand(LIS2DH12_CTRL_REG1, 0x00); /* no measurements */
}

uint8_t lis2dh12ReadNumSamplesInFifo(uint8_t *numSamplesInFifo)
{
    uint8_t retval;
    uint8_t rxData;
    retval = lis2dh12ReadRegisters(LIS2DH12_FIFO_SRC_REG, 1, &rxData);
    if (retval == 0)
        return 0;
    if (rxData & 0x40)
        *numSamplesInFifo = 32;
    else
        *numSamplesInFifo = rxData & 0x1F;
    return 1;
}

uint8_t lis2dh12ReadData(accelerometerData_t *data)
{
    uint8_t retval;
    uint8_t rxData[6];
        
    if (data == NULL)
        return 0;
    retval = lis2dh12ReadRegisters((LIS2DH12_OUT_X_L | LIS2DH12_REG_ADDR_AUTO_INCREMENT), 6, rxData);
    if (retval == 1)
    {
        data->xAxis = CONVERT_MSB_LSB_TO_SINT16(rxData[1], rxData[0]);
        data->yAxis = CONVERT_MSB_LSB_TO_SINT16(rxData[3], rxData[2]);
        data->zAxis = CONVERT_MSB_LSB_TO_SINT16(rxData[5], rxData[4]);
        /* convert to normalised units */
        data->xAxis = lis2dh12Normalise(data->xAxis);
        data->yAxis = lis2dh12Normalise(data->yAxis);     
        data->zAxis = lis2dh12Normalise(data->zAxis);     
        return 1; /* data available */
    }
    data->xAxis = 0;
    data->yAxis = 0;
    data->zAxis = 0;
    return 0; /* no data available */
}    
