#include "uart.h"


UART_HandleTypeDef  usart1; 
uint8_t rev_num_error    = RESET;
uint8_t rev_num_complete = RESET;

/*****************************
*函数名称：void HAL_UART_MspInit(UART_HandleTypeDef *huart)
*函数功能：初始化串口1（USART1）
*返回值：无
*输入值： usart1
*作者：何必仕
*日期：2016-10-2
*修改日期：
********************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	 GPIO_InitTypeDef  GPIO_Init;
	 __HAL_RCC_GPIOA_CLK_ENABLE();              //使能GPIOA
     __HAL_RCC_USART1_CLK_ENABLE();             //使能USART1
	
	GPIO_Init.Alternate = GPIO_AF7_USART1;
	GPIO_Init.Pin  = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;           //复用推挽
	GPIO_Init.Pull = GPIO_PULLUP;               //无上下拉
	GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH ;     //高速
	
	HAL_GPIO_Init(GPIOA,&GPIO_Init);
}

/*****************************
*函数名称：void uart_init(uint32_t  bound)
*函数功能：配置串口1（USART1）的波特率，停止位，模式，校验位
*返回值：无
*输入值： bound
*作者：何必仕
*日期：2016-10-2
*修改日期：
********************************/
void uart_init(uint32_t  bound)
{
	usart1.Instance = USART1;                      //选择串口1
	usart1.Init.BaudRate =  bound;                 //设置波特率
	usart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;   //无硬件控制流
	usart1.Init.Mode =  UART_MODE_TX_RX;           //收发模式
	usart1.Init.Parity = UART_PARITY_NONE;         //无奇偶校验位
	usart1.Init.StopBits = UART_STOPBITS_1;        //停止位为1
	usart1.Init.WordLength = UART_WORDLENGTH_8B;   //字长为8位数据格式
	
	HAL_UART_Init(&usart1);                        //初始化USART1
	
	
//	设置串口1中断优先级
#ifdef  EN_USART1_RX
    HAL_NVIC_SetPriority(USART1_IRQn,NVIC_PRIORITYGROUP_1, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
#endif
	
}


/*****************************
*函数名称：void rev_it_init(uint8_t &data,uint8_t size)
*函数功能：串口1中断接收数据
*返回值：无
*输入值：data,size
*作者：何必仕
*日期：2016-10-2
*修改日期：
********************************/
#ifdef  EN_USART1_RX

void rev_it_init(uint8_t *data,uint8_t size)
{
	HAL_UART_Receive_IT(&usart1,data,size);
}


/*****************************
*函数名称：HAL_UART_RxCpltCallback
*函数功能：接收数据后，回调函数
*返回值：无
*输入值：huart
*作者：何必仕
*日期：2016-2-26
*修改日期：
********************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	rev_num_complete = SET;
}


/*****************************
*函数名称：HAL_UART_ErrorCallback
*函数功能：串口1发生出错时的回调函数
*返回值：无
*输入值：huart
*作者：何必仕
*日期：2016-10-2
*修改日期：
********************************/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	rev_num_error    = SET;
}
#endif

/*****************************
*函数名称：fputc
*函数功能：向指定文件中写入一个值，使用printf输出字符串
*返回值：无
*输入值：ch(要写入的值)   f(文件指针)
*作者：何必仕
*日期：2016-10-2
*修改日期：
********************************/
int fputc(int ch,FILE *f)
{
	uint8_t data =(uint8_t) ch;
	HAL_UART_Transmit(&usart1,&data,1,10);
	return ch;
}

