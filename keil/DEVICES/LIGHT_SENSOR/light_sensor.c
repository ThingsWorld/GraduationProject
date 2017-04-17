#include "light_sensor.h"

/*****************************
*函数名称：light_sensor_init(void)
*函数功能：初始化光敏传感器
*返回值：无
*输入值：无
*作者：何必仕
*日期：2016-2-27
*修改日期：
********************************/
void Light_Sensor_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Init;
	__HAL_RCC_GPIOF_CLK_ENABLE();         //使能
	
	GPIO_Init.Pin  = GPIO_PIN_7;
	GPIO_Init.Mode = GPIO_MODE_ANALOG;    //模拟
	GPIO_Init.Pull = GPIO_NOPULL;         //不带上下拉
	
	HAL_GPIO_Init(GPIOF,&GPIO_Init);
	ADC3_Init();
}


/*****************************
*函数名称：light_sensor_init(void)
*函数功能：读取光敏传感器的采样值，并转换为光线百分比
*返回值：光线百分比：0~100 ， 0（最暗）  100（最亮）
*输入值：无
*作者：何必仕
*日期：2016-10-3
*修改日期：
********************************/
uint8_t Light_Sensor_Get(void)
{
	uint32_t temp = 0;
	uint8_t i;
	
	for(i=0; i<Light_Sensor_Read; i++)        
	{
		temp += adc3_read();              //读取ADC3的值
		delay_ms(5);
	}
	temp = temp / Light_Sensor_Read;      //平均值
	
	if(temp>4000)                         //光线最大不超过4000
	{
		temp=4000;
	}
	
	return  (uint8_t)(100-(temp/40));     //计算百分比
	
}
