#ifndef _AS3933_SPI
#define _AS3933_SPI

#include "mxgpio.h"
#include "sys.h"
#include "as3933.h"
#define AS3933_SDI_Set NRF_GPIO->OUTSET=0x01UL<<AS3933_SDI_PIN_NUM  //SDI=1
#define AS3933_SDI_Clr NRF_GPIO->OUTCLR=0x01UL<<AS3933_SDI_PIN_NUM  //SDI=0
#define AS3933_SCLK_Set NRF_GPIO->OUTSET=0x01UL<<AS3933_SCLK_PIN_NUM  //SCLK =1;	
#define AS3933_SCLK_Clr NRF_GPIO->OUTCLR=0x01UL<<AS3933_SCLK_PIN_NUM  //SCLK =0;
#define AS3933_CS_Set NRF_GPIO->OUTSET=0x01UL<<AS3933_CS_PIN_NUM  //CS =1;	
#define AS3933_CS_Clr NRF_GPIO->OUTCLR=0x01UL<<AS3933_CS_PIN_NUM  //CS =0;
#define AS3933_SDO  ((NRF_GPIO->IN>>AS3933_SDO_PIN_NUM)&1UL)  //SDO
#define AS3933_DAT  ((NRF_GPIO->IN>>AS3933_DAT_PIN_NUM)&1UL)  //DAT
extern u8 SPI_RW(uint8_t byte);//SPI FLASH 写数据
uint8_t as3933ReadRegister(uint8_t address,u8 *value);
void as3933WriteRegister(u8 address, u8 value);
u8 as3933ModifyRegister(u8 address, u8 mask, u8 value);
u8 as3933SendCommand(as3933Commands_t command);
#endif

