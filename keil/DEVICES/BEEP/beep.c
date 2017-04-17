#include "beep.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc_ex.h"

/*****************************
*�������ƣ�beep_init(void)
*�������ܣ���ʼ��������
*����ֵ����
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-4
*�޸����ڣ�
********************************/

void beep_init(void)
{
	GPIO_InitTypeDef  GPIO_Init;
	 __HAL_RCC_GPIOF_CLK_ENABLE();              //ʹ��
	
	GPIO_Init.Pin  = GPIO_PIN_8;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;       //�������
	GPIO_Init.Pull = GPIO_PULLDOWN;             //����
	GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH ;     //����
	
	HAL_GPIO_Init(GPIOF,&GPIO_Init);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8,GPIO_PIN_RESET);   //�ߵ�ƽ������
}

void BEEP_OFF(void)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8,GPIO_PIN_RESET);
}


void BEEP_ON(void)
{
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8,GPIO_PIN_SET);
}
