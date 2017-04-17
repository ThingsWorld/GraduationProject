#ifndef __WKUP_H
#define __WKUP_H

#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_pwr.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_rtc.h"
#include "led.h"
#include "delay.h"
#include "lcd.h"

#define  KEY_UP   BIT_ADDR((GPIOA_BASE+16),0)

uint8_t Check_KEY_UP(void);
void wkup_init(void);
void Sys_Enter_Standby(void);
#endif
