#include "adc3.h"


/**************************
*函数名：void ADC3_Init(void)
*函数功能：初始化ADC3
*输入参数：无
*输出参数：无
*返回值：无
*作者：何必仕
*完成日期：2016-10-3
*修改日期：
***************************/
ADC_HandleTypeDef  adc3;

void ADC3_Init(void)
{
	
	__HAL_RCC_ADC3_CLK_ENABLE();
	
	adc3.Instance = ADC3;
	adc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;  //选择预分频2分频
	adc3.Init.ContinuousConvMode = DISABLE;               //关闭连续转换
	adc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;            //右对齐
	adc3.Init.ScanConvMode = DISABLE;                     //非扫描模式
	adc3.Init.Resolution = ADC_RESOLUTION_12B;            //12位模式分辨率
	adc3.Init.NbrOfConversion = (uint32_t) 1;             //1个转换在规则序列中
	adc3.Init.EOCSelection = DISABLE;
		/**禁止触发检测，使用软件检测*/
	adc3.Init.ExternalTrigConvEdge= ADC_EXTERNALTRIGCONVEDGE_NONE;
	
	HAL_ADC_Init(&adc3);
	
}

/*****************************
*函数名称：uint32_t adc3_read(void)
*函数功能：读取ADC3的采样电压值
*返回值：采样电压值
*输入值：无
*作者：何必仕
*日期：2016-10-3
*修改日期：
********************************/
uint32_t adc3_read(void)
{
	uint32_t read_value = 0;
	
	ADC_ChannelConfTypeDef  sConfig; 
	//通道配置
	sConfig.Channel = ADC_CHANNEL_5;                 //指定通道5ch为规则通道        
	sConfig.Rank = 1  ;                             //1个序列
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES ; //采样周期
	HAL_ADC_ConfigChannel(&adc3,&sConfig);          //初始化配置通道
	                                                
                                                                                             
	HAL_ADC_Start(&adc3);                           //开启AD转换器
	read_value = HAL_ADC_GetValue(&adc3);           //读取ADC3的采样电压值
	HAL_ADC_Stop(&adc3);                            //停止AD转换器
	
	return read_value;
}


