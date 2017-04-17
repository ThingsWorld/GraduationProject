#include "uart.h"


UART_HandleTypeDef  usart1; 
uint8_t rev_num_error    = RESET;
uint8_t rev_num_complete = RESET;

/*****************************
*�������ƣ�void HAL_UART_MspInit(UART_HandleTypeDef *huart)
*�������ܣ���ʼ������1��USART1��
*����ֵ����
*����ֵ�� usart1
*���ߣ��α���
*���ڣ�2016-10-2
*�޸����ڣ�
********************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	 GPIO_InitTypeDef  GPIO_Init;
	 __HAL_RCC_GPIOA_CLK_ENABLE();              //ʹ��GPIOA
     __HAL_RCC_USART1_CLK_ENABLE();             //ʹ��USART1
	
	GPIO_Init.Alternate = GPIO_AF7_USART1;
	GPIO_Init.Pin  = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;           //��������
	GPIO_Init.Pull = GPIO_PULLUP;               //��������
	GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH ;     //����
	
	HAL_GPIO_Init(GPIOA,&GPIO_Init);
}

/*****************************
*�������ƣ�void uart_init(uint32_t  bound)
*�������ܣ����ô���1��USART1���Ĳ����ʣ�ֹͣλ��ģʽ��У��λ
*����ֵ����
*����ֵ�� bound
*���ߣ��α���
*���ڣ�2016-10-2
*�޸����ڣ�
********************************/
void uart_init(uint32_t  bound)
{
	usart1.Instance = USART1;                      //ѡ�񴮿�1
	usart1.Init.BaudRate =  bound;                 //���ò�����
	usart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;   //��Ӳ��������
	usart1.Init.Mode =  UART_MODE_TX_RX;           //�շ�ģʽ
	usart1.Init.Parity = UART_PARITY_NONE;         //����żУ��λ
	usart1.Init.StopBits = UART_STOPBITS_1;        //ֹͣλΪ1
	usart1.Init.WordLength = UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	
	HAL_UART_Init(&usart1);                        //��ʼ��USART1
	
	
//	���ô���1�ж����ȼ�
#ifdef  EN_USART1_RX
    HAL_NVIC_SetPriority(USART1_IRQn,NVIC_PRIORITYGROUP_1, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
#endif
	
}


/*****************************
*�������ƣ�void rev_it_init(uint8_t &data,uint8_t size)
*�������ܣ�����1�жϽ�������
*����ֵ����
*����ֵ��data,size
*���ߣ��α���
*���ڣ�2016-10-2
*�޸����ڣ�
********************************/
#ifdef  EN_USART1_RX

void rev_it_init(uint8_t *data,uint8_t size)
{
	HAL_UART_Receive_IT(&usart1,data,size);
}


/*****************************
*�������ƣ�HAL_UART_RxCpltCallback
*�������ܣ��������ݺ󣬻ص�����
*����ֵ����
*����ֵ��huart
*���ߣ��α���
*���ڣ�2016-2-26
*�޸����ڣ�
********************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	rev_num_complete = SET;
}


/*****************************
*�������ƣ�HAL_UART_ErrorCallback
*�������ܣ�����1��������ʱ�Ļص�����
*����ֵ����
*����ֵ��huart
*���ߣ��α���
*���ڣ�2016-10-2
*�޸����ڣ�
********************************/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	rev_num_error    = SET;
}
#endif

/*****************************
*�������ƣ�fputc
*�������ܣ���ָ���ļ���д��һ��ֵ��ʹ��printf����ַ���
*����ֵ����
*����ֵ��ch(Ҫд���ֵ)   f(�ļ�ָ��)
*���ߣ��α���
*���ڣ�2016-10-2
*�޸����ڣ�
********************************/
int fputc(int ch,FILE *f)
{
	uint8_t data =(uint8_t) ch;
	HAL_UART_Transmit(&usart1,&data,1,10);
	return ch;
}

