#include "accelerometer.h"
#include "adxl345.h"
#include "lis2dh12.h"
#include "lis3dh.h"
#include "nrf.h"

//#include "pah_driver_types.h"

#define MAX_MEMS_SAMPLE_NUM 	100
#define MEMS_SAMPLE_PER_CH  25

#define ACC_TYPE_NONE      0 // use fake-data
#define ACC_TYPE_ADXL345   1
#define ACC_TYPE_LIS2DH12  2
#define ACC_TYPE_LIS3DH    3
uint8_t accelerometerType = ACC_TYPE_LIS3DH;
uint8_t accelerometerSampleReadMode = 1;

/* for generating pseudo - accelerometer data - the rules are here */
/* x-Axis is ACC_FAKE_DATA_RESET_VALUE .. ACC_FAKE_DATA_MAX_VALUE */
/* y-Axis is -(x-Axis)  */
/* z-Axis is 2*(x-Axis) */
#define ACC_FAKE_DATA_RESET_VALUE 1
#define ACC_FAKE_DATA_MAX_VALUE   1000
int16_t accelerometerFakeValue = ACC_FAKE_DATA_RESET_VALUE;

#if 0
/******************************************************************************
* accelerometerStart
******************************************************************************/
uint8_t accelerometerStart(uint8_t sampleReadMode, uint16_t *sampleFrequencyMilliHz)
{
    uint8_t retval;
    accelerometerSampleReadMode = sampleReadMode;
		if(accelerometerType == ACC_TYPE_ADXL345)
		{
			// Determine which accelerometer is present (if any)
			retval = adxl345Open(sampleReadMode, sampleFrequencyMilliHz);
			if (retval == 1)
			{
					accelerometerType = ACC_TYPE_ADXL345;
					if (sampleReadMode == 1)
							*sampleFrequencyMilliHz = 20000; // 20.000Hz
					return 1;
			}
		}
		else if(accelerometerType == ACC_TYPE_LIS2DH12)
		{
			retval = lis2dh12Open(sampleReadMode, sampleFrequencyMilliHz);
			if (retval == 1)
			{
					accelerometerType = ACC_TYPE_LIS2DH12;
					if (sampleReadMode == 1)
							*sampleFrequencyMilliHz = 20000; // 20.000Hz
					return 1;
			}			
		}
		else if(accelerometerType == ACC_TYPE_LIS3DH)
		{
			retval = lis3dhOpen(sampleReadMode, sampleFrequencyMilliHz);
			if (retval == 1)
			{
					accelerometerType = ACC_TYPE_LIS3DH;
					if (sampleReadMode == 1)
							*sampleFrequencyMilliHz = 20000; // 20.000Hz
					return 1;
			}
		}
    // No real accelerometer found -- just setup to generate fake data
    accelerometerType = ACC_TYPE_NONE;
    *sampleFrequencyMilliHz = 20000; // 20.000Hz
    // Reset the fake data
    accelerometerFakeValue = ACC_FAKE_DATA_RESET_VALUE;
    return retval;
}

/******************************************************************************
* accelerometerStop
******************************************************************************/
void accelerometerStop(void)
{
    // Stop the accelerometer (to save power)
    if (accelerometerType == ACC_TYPE_ADXL345)
        adxl345Close();
    else if (accelerometerType == ACC_TYPE_LIS2DH12)
        lis2dh12Close();
		else if(accelerometerType == ACC_TYPE_LIS3DH)
				lis3dhClose();
    accelerometerType = ACC_TYPE_NONE;
}

/******************************************************************************
* accelerometerReadSample
******************************************************************************/
uint8_t accelerometerReadSample(accelerometerData_t *accData)
{
    if (accData == NULL)
        return 0;
    // Read a real sample
    if (accelerometerType == ACC_TYPE_ADXL345)
        return adxl345ReadData(accData);
    else if (accelerometerType == ACC_TYPE_LIS2DH12)
        return lis2dh12ReadData(accData);
		else if(accelerometerType == ACC_TYPE_LIS3DH)
				return lis3dhReadData(accData);
    // Return fake data
    accData->xAxis = accelerometerFakeValue;
    accData->yAxis = -accelerometerFakeValue;
    accData->zAxis = accelerometerFakeValue * 2;
    accelerometerFakeValue++;
    if (accelerometerFakeValue > ACC_FAKE_DATA_MAX_VALUE)
        accelerometerFakeValue = ACC_FAKE_DATA_RESET_VALUE;
    return 1;
}

/******************************************************************************
* accelerometerReadAllSamples
******************************************************************************/

uint8_t accelerometerReadAllSamples(as7000AccData_t *accData, uint8_t maxSamples)
{
    accelerometerData_t accSample;
    uint8_t numAccSamplesToRead;
    uint8_t i;
    uint8_t status;
    
    if (accData == NULL)
        return 0;
    numAccSamplesToRead = maxSamples;
    if (maxSamples > 1)
    {
        // Multiple samples requested (not just one) -- determine how many are available
        if (accelerometerType == ACC_TYPE_ADXL345)
        {
            status = adxl345ReadNumSamplesInFifo(&numAccSamplesToRead);
            if (status == 0)
                return 0;
        }
        else if (accelerometerType == ACC_TYPE_LIS2DH12)
        {
            status = lis2dh12ReadNumSamplesInFifo(&numAccSamplesToRead);
            if (status == 0)
                return 0;
        }
        else // no accelerometer
        {
            // setup the number of fake samples
            numAccSamplesToRead = 20;
        }
    }
    if (maxSamples < numAccSamplesToRead)
        numAccSamplesToRead = maxSamples;
    for (i=0; i<numAccSamplesToRead; i++)
    {
        status = accelerometerReadSample(&accSample);
        if (status == 0) // no data
            break;
        accData[i].x = accSample.xAxis;
        accData[i].y = accSample.yAxis;
        accData[i].z = accSample.zAxis;
        accData[i].index = i + 1;
    }
    return i;
}

#endif



uint8_t accelerometer_init(void)
{
	// TODO
	if(!lis3dhInit())
		return FALSE;
	return TRUE;
}
#define COM_DATA_MAX_ACC_SAMPLES    21
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} AccData_t;


/******************************************************************************
* accelerometerReadSample
******************************************************************************/
uint8_t accelerometerReadSample(accelerometerData_t *accData)
{
    if (accData == NULL)
        return 0;
		if(accelerometerType == ACC_TYPE_LIS3DH)
				return lis3dhReadData(accData);
    // Return fake data
    accData->xAxis = accelerometerFakeValue;
    accData->yAxis = -accelerometerFakeValue;
    accData->zAxis = accelerometerFakeValue * 2;
    accelerometerFakeValue++;
    if (accelerometerFakeValue > ACC_FAKE_DATA_MAX_VALUE)
        accelerometerFakeValue = ACC_FAKE_DATA_RESET_VALUE;
    return 1;
}

//int16_t mems_data[MAX_MEMS_SAMPLE_NUM * 3];

int16_t                mems_data[MAX_MEMS_SAMPLE_NUM * 3];
uint8_t accelerometer_get_fifo(int16_t **fifo, uint32_t *fifo_size)
{
	// TODO
	accelerometerData_t accSample;
	uint8_t numAccSamplesToRead;
	uint8_t i;
	uint8_t status;
//	AccData_t mems_data[MEMS_SAMPLE_PER_CH];
	

	if (fifo == NULL)
			return 0;
	// Multiple samples requested (not just one) -- determine how many are available
	if (accelerometerType == ACC_TYPE_LIS3DH)
	{
			status = lis3dhReadNumSamplesInFifo(&numAccSamplesToRead);
			if (status == 0)
					return 0;
	}
	if(numAccSamplesToRead > MEMS_SAMPLE_PER_CH)
		numAccSamplesToRead = MEMS_SAMPLE_PER_CH;
	for (i=0; i<numAccSamplesToRead; i++)
	{
			status = accelerometerReadSample(&accSample);
			if (status == 0) // no data
					break;
			mems_data[i*3] = accSample.xAxis;
			mems_data[i*3+1] = accSample.yAxis;
			mems_data[i*3+2] = accSample.zAxis;
//			mems_data[i].x = accSample.xAxis;
//			mems_data[i].y = accSample.yAxis;
//			mems_data[i].z = accSample.zAxis;
	}
	*fifo = mems_data;
	*fifo_size = numAccSamplesToRead;
	return i;
}

void accelerometer_start(void)
{
	
	if(accelerometer_init())
		lis3dhStart();
	// TODO
}
uint8_t accelerometer_stop(void)
{
	return lis3dhClose();
	// TODO
}
