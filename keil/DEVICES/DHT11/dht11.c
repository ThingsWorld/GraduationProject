#include "dht11.h"

/**************************
*函数名：DHT11_RESET
*函数功能：主机复位DHT11
*输入参数：无
*输出参数：无
*返回值：无
*作者：何必仕
*完成日期：2016-10-2
*修改日期：
***************************/

void  DHT11_RESET(void)
{
	DHT11_IO_OUT();      //设置输出
	DHT11_PIN_OUT = 0;   //拉低
	delay_ms(20);        //拉低至少18ms  
	DHT11_PIN_OUT = 1;     
	delay_us(30);        //主机拉高20-40us
}                        

/*****************************
*函数名称：uint8_t DHT11_CHECK(void)
*函数功能：检测DHT11，等待DHT11的回应
*返回值：1:未检测到DHT11的存在   0:存在
*输入值：无
*作者：何必仕
*日期：2016-10-2
*修改日期：
********************************/
uint8_t DHT11_CHECK(void)
{
	uint8_t reread = 0;
	DHT11_IO_IN();                      //设置输入             
	while(DHT11_PIN_IN&&reread<100)     //引脚拉低检测输入，100次,
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
*函数名称：uint8_t DHT11_Read_bit(void) 	
*函数功能：从DHT11读取一个比特（bit）位
*返回值：1/0
*输入值：无
*作者：何必仕
*日期：2016-10-2
*修改日期：
********************************/
uint8_t DHT11_Read_bit(void) 
{
	uint8_t reread = 0;                
	while(DHT11_PIN_IN&&reread<100)     //等待低电平
	{
		reread++;
		delay_us(1);
	}
	reread = 0;
	
	
	while(!DHT11_PIN_IN&&reread<100)     //等待高电平
	{
		reread++;
		delay_us(1);
	}
	delay_us(40);            //等待40us
	if(DHT11_PIN_IN)         //如果是引脚输入，则返回数据1
		return 1; 
	else
		return 0;
}

/*****************************
*函数名称：uint8_t DHT11_Read_Byte(void) 	
*函数功能：从DHT11读取一个(Byte)字节
*返回值：读到的字节数据
*输入值：无
*作者：何必仕
*日期：2016-10-2
*修改日期：
********************************/
uint8_t DHT11_Read_Byte(void) 
{
	uint8_t Byte = 0;
	uint8_t i;
	for(i=0; i<8; i++)
	{
		Byte <<= 1;
		Byte |= DHT11_Read_bit();            //把（比特）bit位换为Byte（字节）
	}
	return Byte;
}

/*****************************
*函数名称：uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)
*函数功能：从DHT11读取一次数据
*返回值：0 正常   1,读取失败
*输入值：temp:温度值(范围:0~50°)    humi:湿度值(范围:20%~90%)
*作者：何必仕
*日期：2016-2-27
*修改日期：
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
			data[i] = DHT11_Read_Byte();  //读取5 Byte到数组中，即40位数据
		}
		
		if(data[0]+data[1]+data[2]+data[3]==data[4])  //数据校验和
		{
			*humi = data[0] ;
			*temp = data[2] ;
		}
	}
	else  return 1;
	return 0;
	
}


/*****************************
*函数名称：uint8_t DHT11_Init(void)	
*函数功能：初始化DHT11的IO口 DQ   同时检测DHT11的存在
*返回值：1：不存在      0：存在
*输入值：无
*作者：何必仕
*日期：2016-10-2
*修改日期：
********************************/
uint8_t DHT11_Init(void)
{
	GPIO_InitTypeDef  GPIO_Init;
	 __HAL_RCC_GPIOG_CLK_ENABLE();              //使能
	
	GPIO_Init.Pin  = GPIO_PIN_9;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;       //推挽输出
	GPIO_Init.Pull = GPIO_PULLUP;               //上拉
	GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH ;     //高速
	
	HAL_GPIO_Init(GPIOG,&GPIO_Init);
	DHT11_RESET();                               //复位DHT11
	
	return DHT11_CHECK();
}

