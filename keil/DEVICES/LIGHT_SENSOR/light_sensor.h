#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc_ex.h"
#include "adc3.h"
#include "delay.h"



#define Light_Sensor_Read  10


void Light_Sensor_Init(void);
uint8_t Light_Sensor_Get(void);

#endif
