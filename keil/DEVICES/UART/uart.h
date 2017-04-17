#ifndef __UART_T
#define __UART_T
#include "stdint.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_rcc.h"

#define  EN_USART1_RX      1
#define  BAUD_RATE       9600

void rev_it_init(uint8_t *data,uint8_t size);
void uart_init(uint32_t  bound);
#endif

