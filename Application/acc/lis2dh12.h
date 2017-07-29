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

/*! \file lis2dh12.h
 *
 *  \author M. Arpa
 *  \author M. Zimmerman
 *
 *  \brief application specific header file - defines the interface for 
 * the ST LIS2DH12 sensor.
 */

#ifndef LIS2DH12_H
#define LIS2DH12_H

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

/* if we have a lis2dh the who am I register must have this value */
#define LIS2DH12_WHO_AM_I_VALUE             0x33

/* the register map of the LIS2DH12  */
#define LIS2DH12_STATUS_REG_AUX             0x07
#define LIS2DH12_OUT_TEMP_L                 0x0C
#define LIS2DH12_OUT_TEMP_H                 0x0D
#define LIS2DH12_INT_COUNTER_REG            0x0E
#define LIS2DH12_WHO_AM_I                   0x0F
#define LIS2DH12_TEMP_CFG_REG               0x1F
#define LIS2DH12_CTRL_REG1                  0x20
#define LIS2DH12_CTRL_REG2                  0x21
#define LIS2DH12_CTRL_REG3                  0x22
#define LIS2DH12_CTRL_REG4                  0x23
#define LIS2DH12_CTRL_REG5                  0x24
#define LIS2DH12_CTRL_REG6                  0x25
#define LIS2DH12_REFERENCE_DATACAPTURE      0x26
#define LIS2DH12_STATUS_REG                 0x27
#define LIS2DH12_OUT_X_L                    0x28
#define LIS2DH12_OUT_X_H                    0x29
#define LIS2DH12_OUT_Y_L                    0x2A
#define LIS2DH12_OUT_Y_H                    0x2B
#define LIS2DH12_OUT_Z_L                    0x2C
#define LIS2DH12_OUT_Z_H                    0x2D
#define LIS2DH12_FIFO_CTRL_REG              0x2E
#define LIS2DH12_FIFO_SRC_REG               0x2F
#define LIS2DH12_INT1_CFG                   0x30
#define LIS2DH12_INT1_SRC                   0x31
#define LIS2DH12_INT1_THS                   0x32
#define LIS2DH12_INT1_DURATION              0x33
#define LIS2DH12_INT2_CFG                   0x34
#define LIS2DH12_INT2_SRC                   0x35 
#define LIS2DH12_INT2_THS                   0x36
#define LIS2DH12_INT2_DURATION              0x37
#define LIS2DH12_CLICK_CFG                  0x38
#define LIS2DH12_CLICK_SRC                  0x39
#define LIS2DH12_CLICK_THS                  0x3A
#define LIS2DH12_TIME_LIMIT                 0x3B
#define LIS2DH12_TIME_LATENCY               0x3C
#define LIS2DH12_TIME_WINDOW                0x3D
#define LIS2DH12_ACT_THS                    0x3E
#define LIS2DH12_ACT_DUR                    0x3F

/* to use the register adddress autoincrment for i2c and spi interface 
   each register address must be binary ored with this value */
#define LIS2DH12_REG_ADDR_AUTO_INCREMENT	  0x80

/* data generation rates (CTRL_REG1) */
#define LIS2DH12_DATA_RATE_400_HZ    0x70
#define LIS2DH12_DATA_RATE_200_HZ    0x60
#define LIS2DH12_DATA_RATE_100_HZ    0x50
#define LIS2DH12_DATA_RATE_50_HZ     0x40
#define LIS2DH12_DATA_RATE_25_HZ     0x30
#define LIS2DH12_DATA_RATE_10_HZ     0x20

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
uint8_t lis2dh12Open(uint8_t sampleReadMode, uint16_t *sampleFrequencyMilliHz);

/*!
 *****************************************************************************
 * \brief Disable accelerometer measurement.
 *****************************************************************************
 */
void lis2dh12Close(void);

uint8_t lis2dh12ReadNumSamplesInFifo(uint8_t *numSamplesInFifo);
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
uint8_t lis2dh12ReadData(accelerometerData_t *data);


#endif /* LIS2DH12_H */
