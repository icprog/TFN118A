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

/*! \file adxl345.h
 *
 *  \author M. Arpa
 *  \author M. Zimmerman
 *
 *  \brief application specific header file - defines the interface for 
 * the ADXL345 sensor.
 */

#ifndef ADXL345_H
#define ADXL345_H

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

/* the register map of the ADXL345  */
#define ADXL345_DEVID           0x00
#define ADXL345_TRESH_TAB       0x1D
#define ADXL345_OFSX            0x1E
#define ADXL345_OFSY            0x1F
#define ADXL345_OFSZ            0x20
#define ADXL345_DUR             0x21
#define ADXL345_LATENT          0x22
#define ADXL345_WINDOW          0x23
#define ADXL345_TRESH_ACT       0x24
#define ADXL345_TRESH_INACT     0x25
#define ADXL345_TIME_INACT      0x26
#define ADXL345_ACT_INACT_CTL   0x27
#define ADXL345_TRESH_FF        0x28
#define ADXL345_TIME_FF         0x29
#define ADXL345_TAP_AXES        0x2A
#define ADXL345_ACT_TAB_STATUS  0x2B
#define ADXL345_BW_RATE         0x2C
#define ADXL345_POWER_CTL       0x2D
#define ADXL345_INT_ENABLE      0x2E
#define ADXL345_INT_MAP         0x2F
#define ADXL345_INT_SOURCE      0x30
#define ADXL345_DATA_FORMAT     0x31
#define ADXL345_DATAX0          0x32    /* LSB */
#define ADXL345_DATAX1          0x33    /* MSB */
#define ADXL345_DATAY0          0x34    /* LSB */
#define ADXL345_DATAY1          0x35    /* MSB */
#define ADXL345_DATAZ0          0x36    /* LSB */
#define ADXL345_DATAZ1          0x37    /* MSB */
#define ADXL345_FIFO_CTL        0x38
#define ADXL345_FIFO_STATUS     0x39


/* data generation rates */
#define ADXL345_DATA_RATE_800_HZ    0xD
#define ADXL345_DATA_RATE_400_HZ    0xC
#define ADXL345_DATA_RATE_200_HZ    0xB
#define ADXL345_DATA_RATE_100_HZ    0xA
#define ADXL345_DATA_RATE_50_HZ     0x9
#define ADXL345_DATA_RATE_25_HZ     0x8
#define ADXL345_DATA_RATE_12_5_HZ   0x7 // 12.5Hz


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
uint8_t adxl345Open(uint8_t sampleReadMode, uint16_t *sampleFrequencyMilliHz);

/*!
 *****************************************************************************
 * \brief Disable accelerometer measurement.
 *****************************************************************************
 */
void adxl345Close(void);

uint8_t adxl345ReadNumSamplesInFifo(uint8_t *numSamplesInFifo);

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
uint8_t adxl345ReadData(accelerometerData_t *data);


#endif /* ADXL345_H */
