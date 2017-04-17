#include "dht11.h"

/**************************
*��������DHT11_RESET
*�������ܣ�������λDHT11
*�����������
*�����������
*����ֵ����
*���ߣ��α���
*������ڣ�2016-10-2
*�޸����ڣ�
***************************/

void  DHT11_RESET(void)
{
	DHT11_IO_OUT();      //�������
	DHT11_PIN_OUT = 0;   //����
	delay_ms(20);        //��������18ms  
	DHT11_PIN_OUT = 1;     
	delay_us(30);        //��������20-40us
}                        

/*****************************
*�������ƣ�uint8_t DHT11_CHECK(void)
*�������ܣ����DHT11���ȴ�DHT11�Ļ�Ӧ
*����ֵ��1:δ��⵽DHT11�Ĵ���   0:����
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-2
*�޸����ڣ�
********************************/
uint8_t DHT11_CHECK(void)
{
	uint8_t reread = 0;
	DHT11_IO_IN();                      //��������             
	while(DHT11_PIN_IN&&reread<100)     //�������ͼ�����룬100��,
	{
		reread++;
		delay_us(1);
	};
	if(reread>=100)
		return 1;
	else 
		reread = 0;
	
	
	while(!DHT11_PIN_IN&&reread<100)     
	{
		reread++;
		delay_us(1);
	};
	if(reread>=100)
		return 1;
	else 
		return 0;
}

/*****************************
*�������ƣ�uint8_t DHT11_Read_bit(void) 	
*�������ܣ���DHT11��ȡһ�����أ�bit��λ
*����ֵ��1/0
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-2
*�޸����ڣ�
********************************/
uint8_t DHT11_Read_bit(void) 
{
	uint8_t reread = 0;                
	while(DHT11_PIN_IN&&reread<100)     //�ȴ��͵�ƽ
	{
		reread++;
		delay_us(1);
	}
	reread = 0;
	
	
	while(!DHT11_PIN_IN&&reread<100)     //�ȴ��ߵ�ƽ
	{
		reread++;
		delay_us(1);
	}
	delay_us(40);            //�ȴ�40us
	if(DHT11_PIN_IN)         //������������룬�򷵻�����1
		return 1; 
	else
		return 0;
}

/*****************************
*�������ƣ�uint8_t DHT11_Read_Byte(void) 	
*�������ܣ���DHT11��ȡһ��(Byte)�ֽ�
*����ֵ���������ֽ�����
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-2
*�޸����ڣ�
********************************/
uint8_t DHT11_Read_Byte(void) 
{
	uint8_t Byte = 0;
	uint8_t i;
	for(i=0; i<8; i++)
	{
		Byte <<= 1;
		Byte |= DHT11_Read_bit();            //�ѣ����أ�bitλ��ΪByte���ֽڣ�
	}
	return Byte;
}

/*****************************
*�������ƣ�uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)
*�������ܣ���DHT11��ȡһ������
*����ֵ��0 ����   1,��ȡʧ��
*����ֵ��temp:�¶�ֵ(��Χ:0~50��)    humi:ʪ��ֵ(��Χ:20%~90%)
*���ߣ��α���
*���ڣ�2016-2-27
*�޸����ڣ�
********************************/
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)
{
	uint8_t  data[5];
	uint8_t i;
	DHT11_RESET();

	if(DHT11_CHECK()==0)
	{
		for(i=0; i<5; i++)
		{
			data[i] = DHT11_Read_Byte();  //��ȡ5 Byte�������У���40λ����
		}
		
		if(data[0]+data[1]+data[2]+data[3]==data[4])  //����У���
		{
			*humi = data[0] ;
			*temp = data[2] ;
		}
	}
	else  return 1;
	return 0;
	
}


/*****************************
*�������ƣ�uint8_t DHT11_Init(void)	
*�������ܣ���ʼ��DHT11��IO�� DQ   ͬʱ���DHT11�Ĵ���
*����ֵ��1��������      0������
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-2
*�޸����ڣ�
********************************/
uint8_t DHT11_Init(void)
{
	GPIO_InitTypeDef  GPIO_Init;
	 __HAL_RCC_GPIOG_CLK_ENABLE();              //ʹ��
	
	GPIO_Init.Pin  = GPIO_PIN_9;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;       //�������
	GPIO_Init.Pull = GPIO_PULLUP;               //����
	GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH ;     //����
	
	HAL_GPIO_Init(GPIOG,&GPIO_Init);
	DHT11_RESET();                               //��λDHT11
	
	return DHT11_CHECK();
}

