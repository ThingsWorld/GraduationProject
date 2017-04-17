#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc_ex.h"
#include "stdint.h"
#include "stm32f407xx.h"
#include "delay.h"
#include "lcd.h"



#include "stdint.h"

#define DHT11_IO_IN()  {GPIOG->MODER&=~(3<<(9*2));GPIOG->MODER|=0<<9*2;}	//PG9输入模式
#define DHT11_IO_OUT() {GPIOG->MODER&=~(3<<(9*2));GPIOG->MODER|=1<<9*2;} 	//PG9输出模式 

#define	DHT11_PIN_OUT   BIT_ADDR((GPIOG_BASE+20),9)//数据端口	PG9 
#define	DHT11_PIN_IN    BIT_ADDR((GPIOG_BASE+16),9)//数据端口	PG9 



void  DHT11_RESET(void);
uint8_t DHT11_CHECK(void);
uint8_t DHT11_Read_bit(void); 
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);
uint8_t DHT11_Init(void);
#endif 


