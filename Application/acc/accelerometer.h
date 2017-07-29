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

/*!
 *
 *  \author M. Zimmerman
 *
 *  \brief accelerometer-control
 *
 */
#ifndef ACCELEROMETER_H
#define	ACCELEROMETER_H

#include <stdint.h>
#include <stddef.h>
#include "errno.h"
#include "sys.h"

typedef struct
{
    int16_t xAxis;
    int16_t yAxis;
    int16_t zAxis;
} accelerometerData_t;

uint8_t accelerometerStart(uint8_t sampleReadMode, uint16_t *sampleFrequencyMilliHz);
void accelerometerStop(void);
uint8_t accelerometerReadSample(accelerometerData_t *accData);

void accelerometer_start(void);
uint8_t accelerometer_init(void);
uint8_t accelerometer_get_fifo(int16_t **fifo, uint32_t *fifo_size);
uint8_t accelerometer_stop(void);
#endif	/* ACCELEROMETER_H */

