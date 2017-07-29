/*! \file lis3dh.h
 *
 *  \author M. Arpa
 *  \author M. Zimmerman
 *
 *  \brief application specific header file - defines the interface for 
 * the ST LIS3DH sensor.
 */

#ifndef LIS3DH_H
#define LIS3DH_H

/*
 *****************************************************************************
 * INCLUDES
 *****************************************************************************
 */
#include <stdint.h>
#include "accelerometer.h"

/*
 *****************************************************************************
 * DEFINES
 *****************************************************************************
 */

/* if we have a lis3dh the who am I register must have this value */
#define LIS3DH_WHO_AM_I_VALUE             0x33
/* the register map of the LIS3DH  */
#define LIS3DH_STATUS_REG_AUX     				0x07
#define LIS3DH_WHO_AM_I										0x0F//device identification register
#define LIS3DH_TEMP_CFG_REG      					0x1F
#define LIS3DH_CTRL_REG1									0x20//CONTROL REGISTER 1
#define LIS3DH_CTRL_REG2									0x21//CONTROL REGISTER 2
#define LIS3DH_CTRL_REG3									0x22//CONTROL REGISTER 3
#define LIS3DH_CTRL_REG4									0x23//CONTROL REGISTER 4
#define LIS3DH_CTRL_REG5									0x24//CONTROL REGISTER 5
#define LIS3DH_CTRL_REG6									0x25//CONTROL REGISTER 6
#define LIS3DH_REFERENCE_DATACAPTURE      0x26
#define LIS3DH_STATUS_REG                 0x27
#define LIS3DH_OUT_X_L                    0x28
#define LIS3DH_OUT_X_H                    0x29
#define LIS3DH_OUT_Y_L                    0x2A
#define LIS3DH_OUT_Y_H                    0x2B
#define LIS3DH_OUT_Z_L                    0x2C
#define LIS3DH_OUT_Z_H                    0x2D
#define LIS3DH_FIFO_CTRL_REG              0x2E
#define LIS3DH_FIFO_SRC_REG               0x2F
#define LIS3DH_INT1_CFG                   0x30
#define LIS3DH_INT1_SRC                   0x31
#define LIS3DH_INT1_THS                   0x32
#define LIS3DH_INT1_DURATION              0x33
#define LIS3DH_INT2_CFG                   0x34
#define LIS3DH_INT2_SRC                   0x35 
#define LIS3DH_INT2_THS                   0x36
#define LIS3DH_INT2_DURATION              0x37
#define LIS3DH_CLICK_CFG                  0x38
#define LIS3DH_CLICK_SRC                  0x39
#define LIS3DH_CLICK_THS                  0x3A
#define LIS3DH_TIME_LIMIT                 0x3B
#define LIS3DH_TIME_LATENCY               0x3C
#define LIS3DH_TIME_WINDOW                0x3D
#define LIS3DH_ACT_THS                    0x3E
#define LIS3DH_ACT_DUR                    0x3F

/* to use the register adddress autoincrment for i2c and spi interface 
   each register address must be binary ored with this value */
#define LIS3DH_REG_ADDR_AUTO_INCREMENT	  0x80

/* data generation rates (CTRL_REG1) */
#define LIS3DH_DATA_RATE_400_HZ    0x70
#define LIS3DH_DATA_RATE_200_HZ    0x60
#define LIS3DH_DATA_RATE_100_HZ    0x50
#define LIS3DH_DATA_RATE_50_HZ     0x40
#define LIS3DH_DATA_RATE_25_HZ     0x30
#define LIS3DH_DATA_RATE_10_HZ     0x20

/*
 *****************************************************************************
 * FUNCTIONS
 *****************************************************************************
 */

/*!
 *****************************************************************************
 * \brief Configures and switches the accelerometer on.
 *
 * \return <>0 - when accelerometer could be configured and switched on
 *         ==0 - could not configure the accelerometer (either there is none
 *               or accelerometer is not responding)
 *****************************************************************************
 */
 #if 1
uint8_t lis3dhOpen(uint8_t sampleReadMode, uint16_t *sampleFrequencyMilliHz);
#endif
uint8_t lis3dhStart(void);
uint8_t lis3dhInit(void);
/*!
 *****************************************************************************
 * \brief Disable accelerometer measurement.
 *****************************************************************************
 */
uint8_t lis3dhClose(void);

uint8_t lis3dhReadNumSamplesInFifo(uint8_t *numSamplesInFifo);
/*!
 *****************************************************************************
 * \brief Reads through i2c the actual accelerometer data.
 *
 * \param data - pointer to an accelerometerData structure, this 
 * structure is filled with the read accelerometer data (X,Y and Z axis ).
 * If you hand in a real pointer, but the data cannot be read from the
 * accelerometer, the data will be set to zero.
 *
 * \return <>0 - data structure is filled with new data
 *         ==0 - either parameter data was the 0-pointer, or no 
 *         new data read from accelerometer
 *****************************************************************************
 */
uint8_t lis3dhReadData(accelerometerData_t *data);


#endif /* LIS3DH_H */
