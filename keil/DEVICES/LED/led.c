#include "led.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc_ex.h"

/*****************************
*�������ƣ�led_init(void)
*�������ܣ���ʼ��LED��
*����ֵ����
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-2
*�޸����ڣ�
********************************/

void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_Init;
	 __HAL_RCC_GPIOF_CLK_ENABLE();        //ʹ��
	
	GPIO_Init.Pin  = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;       //�������
	GPIO_Init.Pull = GPIO_NOPULL;               //��������
	GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH ;     //����
	
	HAL_GPIO_Init(GPIOF,&GPIO_Init);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9|GPIO_PIN_10,GPIO_PIN_SET);   //�ߵ�ƽ������
}

//���������Ƶ���
void LED_OFF(void)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9|GPIO_PIN_10,GPIO_PIN_SET);
}

//���������ƵĿ�
void LED_ON(void)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9|GPIO_PIN_10,GPIO_PIN_RESET);
}



