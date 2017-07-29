/*
 *****************************************************************************
 */
/*
 *      PROJECT:   AS7000
 *      LANGUAGE:  ANSI C
 */

/*! \file adxl345.c
 *
 *  \brief adxl345 accelerometer-control
 *
 */

/*
 *****************************************************************************
 * INCLUDES
 *****************************************************************************
 */
#include "adxl345.h"
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

//#define ADXL345_I2C_SLAVE_ADDRESS       0x53    /* alt_address pin of ADXL345 must be low */
#define ADXL345_I2C_SLAVE_ADDRESS       0xA6   //0x53<<1
/*
 *****************************************************************************
 * VARIABLES
 *****************************************************************************
 */


/*
 *****************************************************************************
 * FUNCTIONS
 *****************************************************************************
 */

static uint8_t adxl345SendCommand(uint8_t reg, uint8_t value)
{
    uint8_t txData[2];
    txData[0] = reg;
    txData[1] = value;
		return (I2C_SendDates(ADXL345_I2C_SLAVE_ADDRESS,txData,2)==ERR_NONE);
}

static uint8_t adxl345ReadRegisters(uint8_t reg, uint8_t count, uint8_t *data)
{	
		return (ERR_NONE==I2C_ReadDatas(ADXL345_I2C_SLAVE_ADDRESS,reg,data,count));   
}

uint8_t adxl345Open(uint8_t sampleReadMode, uint16_t *sampleFrequencyMilliHz)
{
    /* configure the adxl345 */
    *sampleFrequencyMilliHz = 0;
    if (adxl345SendCommand(ADXL345_POWER_CTL, 0x00) != 1) /* no measurement -> to allow configuration of adxl345 */
        return 0;
    if (adxl345SendCommand(ADXL345_DATA_FORMAT, 0x0f) != 1) /* full resolution, left justified, +/-16g */
        return 0;
    if (adxl345SendCommand(ADXL345_FIFO_CTL, 0x00) != 1) /* FIFO-bypass (clears the FIFO) */
        return 0;
    if (sampleReadMode)
    {
        if (adxl345SendCommand(ADXL345_BW_RATE, ADXL345_DATA_RATE_200_HZ) != 1) /* select the data-rate */
            return 0;
    }
    else
    {
        if (adxl345SendCommand(ADXL345_BW_RATE, ADXL345_DATA_RATE_12_5_HZ) != 1) /* select the data-rate */
            return 0;
        if (adxl345SendCommand(ADXL345_FIFO_CTL, 0x40) != 1) /* FIFO-enabled */
            return 0;
        *sampleFrequencyMilliHz = 12500; // 12.500Hz
    }
    if (adxl345SendCommand(ADXL345_POWER_CTL, 0x08) != 1) /* start measurement */
        return 0;
    return 1; 
}

void adxl345Close(void)
{
    adxl345SendCommand(ADXL345_POWER_CTL, 0x00); /* no measurements */
}

uint8_t adxl345ReadNumSamplesInFifo(uint8_t *numSamplesInFifo)
{
    uint8_t retval;
    uint8_t rxData;
    retval = adxl345ReadRegisters(ADXL345_FIFO_STATUS, 1, &rxData);
    if (retval == 0)
        return 0;
    *numSamplesInFifo = rxData & 0x3F;
    return 1;
}

uint8_t adxl345ReadData(accelerometerData_t *data)
{
    uint8_t retval;
    uint8_t rxData[6];
        
    if (data == NULL)
        return 0;
    retval = adxl345ReadRegisters(ADXL345_DATAX0, 6, rxData);
    if (retval == 1)
    {
        data->xAxis = CONVERT_MSB_LSB_TO_SINT16(rxData[1], rxData[0]);
        data->yAxis = CONVERT_MSB_LSB_TO_SINT16(rxData[3], rxData[2]);
        data->zAxis = CONVERT_MSB_LSB_TO_SINT16(rxData[5], rxData[4]);
        return 1; /* data available */
    }
    data->xAxis = 0;
    data->yAxis = 0;
    data->zAxis = 0;
    return 0; /* no data available */
}    
