
#include "lis3dh.h"
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

#define LIS3DH_I2C_SLAVE_ADDRESS      0x19    /* SA0 pin of LIS3DH must be high */
#define LIS3DH_I2C_SLAVE_ADDRESS_ALT  0x18    /* alt address: SA0 pin of LIS3DH must be low */
//#define LIS3DH_I2C_SLAVE_ADDRESS 0x30
//#define LIS3DH_I2C_SLAVE_ADDRESS_ALT  0x32

/*
 *****************************************************************************
 * VARIABLES
 *****************************************************************************
 */

/* default is the lis3dhI2cAddress - try alternate only when this one fails */
static uint8_t lis3dhI2cAddress = LIS3DH_I2C_SLAVE_ADDRESS<<1;

/*
 *****************************************************************************
 * FUNCTIONS
 *****************************************************************************
 */

static uint8_t lis3dhSendCommand(uint8_t reg, uint8_t value)
{
	
	uint8_t txData[2];
    txData[0] = reg;
    txData[1] = value;
	return (I2C_SendDates(lis3dhI2cAddress,txData,2) == TRUE);
}

static uint8_t lis3dhReadRegisters(uint8_t reg, uint8_t count, uint8_t *data)
{
		return (I2C_ReadDatas(lis3dhI2cAddress,reg,data,count)==TRUE);   
}

/* LIS IC returns accelerometer data with one unit of ~ 0.75mg (we want to
   standardize the units of the accelerometer data to be ~0.5mg 
   And indicate this change of original data by setting the lsb bit to 1 and
   clearing bits 1,2,3 to 0 */
static int16_t lis3dhNormalise ( int16_t sample )
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
#if 0
static uint8_t lis3dhOpenInternal(uint8_t sampleReadMode, uint16_t *sampleFrequencyMilliHz)
{
    uint8_t rxData; 

    /* configure the lis3dh */
    *sampleFrequencyMilliHz = 0;
    if (lis3dhReadRegisters(LIS3DH_WHO_AM_I, 1, &rxData) != 1)
        return 0;
    if (rxData != LIS3DH_WHO_AM_I_VALUE) /* validate the correct device is present */
        return 0;
    if (lis3dhSendCommand(LIS3DH_CTRL_REG1, 0x00) != 1) /* no measurement -> to allow configuration of lis3dh */
        return 0;
    if (lis3dhSendCommand(LIS3DH_CTRL_REG4, 0x80 | 0x10 | 0x08) != 1)  /* BDU, 4g, high resolution mode */
        return 0;
    if (lis3dhSendCommand(LIS3DH_CTRL_REG5, 0x00) != 1) /* FIFO-disabled */
        return 0;
    if (lis3dhSendCommand(LIS3DH_FIFO_CTRL_REG, 0x00) != 1) /* FIFO-bypass (clears the FIFO) */
        return 0;
    if (sampleReadMode)
    {
        if (lis3dhSendCommand(LIS3DH_CTRL_REG1, (LIS3DH_DATA_RATE_200_HZ) | 0x7) != 1) /* select bandwith, all 3 axis */
            return 0;
    }
    else
    {
        if (lis3dhSendCommand(LIS3DH_CTRL_REG5, 0x40) != 1) /* FIFO-enabled */
            return 0;
        if (lis3dhSendCommand(LIS3DH_FIFO_CTRL_REG, 0x40) != 1) /* FIFO-mode enabled */
            return 0;
        if (lis3dhSendCommand(LIS3DH_CTRL_REG1, (LIS3DH_DATA_RATE_25_HZ) | 0x7) != 1) /* select bandwith, all 3 axis */
				{
						*sampleFrequencyMilliHz = 25000; // 10.000Hz
            return 0;
				}
        
    }
    return 1;
}

uint8_t lis3dhOpen(uint8_t sampleReadMode, uint16_t *sampleFrequencyMilliHz)
{
    lis3dhI2cAddress = LIS3DH_I2C_SLAVE_ADDRESS; /* initial slave-address */
    if (lis3dhOpenInternal(sampleReadMode, sampleFrequencyMilliHz) == 1)
        return 1;
    lis3dhI2cAddress = LIS3DH_I2C_SLAVE_ADDRESS_ALT; /* alternate slave-address */
    return lis3dhOpenInternal(sampleReadMode, sampleFrequencyMilliHz);
}  
#endif

uint8_t lis3dhInit(void)
{
    uint8_t rxData; 

    /* configure the lis3dh */
    if (lis3dhReadRegisters(LIS3DH_WHO_AM_I, 1, &rxData) != 1)
        return 0;
    if (rxData != LIS3DH_WHO_AM_I_VALUE) /* validate the correct device is present */
        return 0;
    if (lis3dhSendCommand(LIS3DH_CTRL_REG1, 0x00) != 1) /* no measurement -> to allow configuration of lis3dh */
        return 0;
    if (lis3dhSendCommand(LIS3DH_CTRL_REG4, 0x80 | 0x10 | 0x08) != 1)  /* BDU, 4g, high resolution mode */
        return 0;
    if (lis3dhSendCommand(LIS3DH_CTRL_REG5, 0x00) != 1) /* FIFO-disabled */
        return 0;
    if (lis3dhSendCommand(LIS3DH_FIFO_CTRL_REG, 0x00) != 1) /* FIFO-bypass (clears the FIFO) */
        return 0;
    return 1;
}

uint8_t lis3dhStart(void)
{
		if (lis3dhSendCommand(LIS3DH_CTRL_REG5, 0x40) != 1) /* FIFO-enabled */
				return 0;
		if (lis3dhSendCommand(LIS3DH_FIFO_CTRL_REG, 0x40) != 1) /* FIFO-mode enabled */
				return 0;
		if (lis3dhSendCommand(LIS3DH_CTRL_REG1, (LIS3DH_DATA_RATE_25_HZ) | 0x7) != 1) /* select bandwith, all 3 axis */
		{
				return 0;
		}
		return 1;
}  


uint8_t lis3dhClose(void)
{
    return lis3dhSendCommand(LIS3DH_CTRL_REG1, 0x00); /* no measurements */
}

uint8_t lis3dhReadNumSamplesInFifo(uint8_t *numSamplesInFifo)
{
    uint8_t retval;
    uint8_t rxData;
    retval = lis3dhReadRegisters(LIS3DH_FIFO_SRC_REG, 1, &rxData);
    if (retval == 0)
        return 0;
    if (rxData & 0x40)
        *numSamplesInFifo = 32;
    else
        *numSamplesInFifo = rxData & 0x1F;
    return 1;
}

uint8_t lis3dhReadData(accelerometerData_t *data)
{
    uint8_t retval;
    uint8_t rxData[6];
        
    if (data == NULL)
        return 0;
    retval = lis3dhReadRegisters((LIS3DH_OUT_X_L | LIS3DH_REG_ADDR_AUTO_INCREMENT), 6, rxData);
    if (retval == 1)
    {
        data->xAxis = CONVERT_MSB_LSB_TO_SINT16(rxData[1], rxData[0]);
        data->yAxis = CONVERT_MSB_LSB_TO_SINT16(rxData[3], rxData[2]);
        data->zAxis = CONVERT_MSB_LSB_TO_SINT16(rxData[5], rxData[4]);
        /* convert to normalised units */
//        data->xAxis = lis3dhNormalise(data->xAxis);
//        data->yAxis = lis3dhNormalise(data->yAxis);     
//        data->zAxis = lis3dhNormalise(data->zAxis);     
        return 1; /* data available */
    }
    data->xAxis = 0;
    data->yAxis = 0;
    data->zAxis = 0;
    return 0; /* no data available */
}    
