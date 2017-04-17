#include "light_sensor.h"

/*****************************
*�������ƣ�light_sensor_init(void)
*�������ܣ���ʼ������������
*����ֵ����
*����ֵ����
*���ߣ��α���
*���ڣ�2016-2-27
*�޸����ڣ�
********************************/
void Light_Sensor_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Init;
	__HAL_RCC_GPIOF_CLK_ENABLE();         //ʹ��
	
	GPIO_Init.Pin  = GPIO_PIN_7;
	GPIO_Init.Mode = GPIO_MODE_ANALOG;    //ģ��
	GPIO_Init.Pull = GPIO_NOPULL;         //����������
	
	HAL_GPIO_Init(GPIOF,&GPIO_Init);
	ADC3_Init();
}


/*****************************
*�������ƣ�light_sensor_init(void)
*�������ܣ���ȡ�����������Ĳ���ֵ����ת��Ϊ���߰ٷֱ�
*����ֵ�����߰ٷֱȣ�0~100 �� 0�����  100��������
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-3
*�޸����ڣ�
********************************/
uint8_t Light_Sensor_Get(void)
{
	uint32_t temp = 0;
	uint8_t i;
	
	for(i=0; i<Light_Sensor_Read; i++)        
	{
		temp += adc3_read();              //��ȡADC3��ֵ
		delay_ms(5);
	}
	temp = temp / Light_Sensor_Read;      //ƽ��ֵ
	
	if(temp>4000)                         //������󲻳���4000
	{
		temp=4000;
	}
	
	return  (uint8_t)(100-(temp/40));     //����ٷֱ�
	
}
