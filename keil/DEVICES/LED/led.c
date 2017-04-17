#include "led.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc_ex.h"

/*****************************
*函数名称：led_init(void)
*函数功能：初始化LED灯
*返回值：无
*输入值：无
*作者：何必仕
*日期：2016-10-2
*修改日期：
********************************/

void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_Init;
	 __HAL_RCC_GPIOF_CLK_ENABLE();        //使能
	
	GPIO_Init.Pin  = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;       //推挽输出
	GPIO_Init.Pull = GPIO_NOPULL;               //无上下拉
	GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH ;     //高速
	
	HAL_GPIO_Init(GPIOF,&GPIO_Init);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9|GPIO_PIN_10,GPIO_PIN_SET);   //高电平，灯灭
}

//设置两个灯的灭
void LED_OFF(void)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9|GPIO_PIN_10,GPIO_PIN_SET);
}

//设置两个灯的开
void LED_ON(void)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9|GPIO_PIN_10,GPIO_PIN_RESET);
}



