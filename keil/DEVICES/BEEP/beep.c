#include "beep.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc_ex.h"

/*****************************
*函数名称：beep_init(void)
*函数功能：初始化蜂鸣器
*返回值：无
*输入值：无
*作者：何必仕
*日期：2016-10-4
*修改日期：
********************************/

void beep_init(void)
{
	GPIO_InitTypeDef  GPIO_Init;
	 __HAL_RCC_GPIOF_CLK_ENABLE();              //使能
	
	GPIO_Init.Pin  = GPIO_PIN_8;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;       //推挽输出
	GPIO_Init.Pull = GPIO_PULLDOWN;             //下拉
	GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH ;     //高速
	
	HAL_GPIO_Init(GPIOF,&GPIO_Init);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8,GPIO_PIN_RESET);   //高电平，灯灭
}

void BEEP_OFF(void)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8,GPIO_PIN_RESET);
}


void BEEP_ON(void)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8,GPIO_PIN_SET);
}
