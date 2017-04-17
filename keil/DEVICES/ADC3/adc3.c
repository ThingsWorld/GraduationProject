#include "adc3.h"


/**************************
*��������void ADC3_Init(void)
*�������ܣ���ʼ��ADC3
*�����������
*�����������
*����ֵ����
*���ߣ��α���
*������ڣ�2016-10-3
*�޸����ڣ�
***************************/
ADC_HandleTypeDef  adc3;

void ADC3_Init(void)
{
	
	__HAL_RCC_ADC3_CLK_ENABLE();
	
	adc3.Instance = ADC3;
	adc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;  //ѡ��Ԥ��Ƶ2��Ƶ
	adc3.Init.ContinuousConvMode = DISABLE;               //�ر�����ת��
	adc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;            //�Ҷ���
	adc3.Init.ScanConvMode = DISABLE;                     //��ɨ��ģʽ
	adc3.Init.Resolution = ADC_RESOLUTION_12B;            //12λģʽ�ֱ���
	adc3.Init.NbrOfConversion = (uint32_t) 1;             //1��ת���ڹ���������
	adc3.Init.EOCSelection = DISABLE;
		/**��ֹ������⣬ʹ��������*/
	adc3.Init.ExternalTrigConvEdge= ADC_EXTERNALTRIGCONVEDGE_NONE;
	
	HAL_ADC_Init(&adc3);
	
}

/*****************************
*�������ƣ�uint32_t adc3_read(void)
*�������ܣ���ȡADC3�Ĳ�����ѹֵ
*����ֵ��������ѹֵ
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-3
*�޸����ڣ�
********************************/
uint32_t adc3_read(void)
{
	uint32_t read_value = 0;
	
	ADC_ChannelConfTypeDef  sConfig; 
	//ͨ������
	sConfig.Channel = ADC_CHANNEL_5;                 //ָ��ͨ��5chΪ����ͨ��        
	sConfig.Rank = 1  ;                             //1������
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES ; //��������
	HAL_ADC_ConfigChannel(&adc3,&sConfig);          //��ʼ������ͨ��
	                                                
                                                                                             
	HAL_ADC_Start(&adc3);                           //����ADת����
	read_value = HAL_ADC_GetValue(&adc3);           //��ȡADC3�Ĳ�����ѹֵ
	HAL_ADC_Stop(&adc3);                            //ֹͣADת����
	
	return read_value;
}


