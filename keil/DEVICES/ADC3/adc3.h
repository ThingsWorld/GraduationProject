#ifndef __ADC3_H
#define __ADC3_H
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_rcc_ex.h"


void ADC3_Init(void);
uint32_t adc3_read(void);
#endif
