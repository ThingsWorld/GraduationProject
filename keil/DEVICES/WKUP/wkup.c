#include "wkup.h"
#include "stm32f4xx_exti.h"
/**************************
*��������uint8_t Check_KEY_UP(void)
*�������ܣ�����Ƿ���key_up��������
*�����������
*�����������
*����ֵ��1 / 0
*���ߣ��α���
*������ڣ�2016-10-5
*�޸����ڣ�
***************************/
uint8_t Check_KEY_UP(void)
{
	uint8_t t=0;
	uint8_t tx=0;//��¼�ɿ��Ĵ���
	LED_ON(); 	
	while(1)
	{
		if(KEY_UP)//�Ѿ�������
		{
			t++;
			tx=0;
		}else 
		{
			tx++; 
			if(tx>3)//����90ms��û��WKUP�ź�
			{
				LED_OFF();
				return 0;//����İ���,���´�������
			}
		}
		delay_ms(30);
		if(t>=100)//���³���3����
		{
			LED_ON(); 
			return 1; //����3s������
		}
	}
}

/**************************
*��������void EXTI0_IRQHandler(void)
*�������ܣ��ⲿ�ж�0����
*�����������
*�����������
*����ֵ����
*���ߣ��α���
*������ڣ�2016-10-6
*�޸����ڣ�
***************************/
void EXTI0_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI_IMR_MR0);                //�����־λ
	if(Check_KEY_UP())
	{
		Sys_Enter_Standby(); //�������ģʽ
	}
}




/*****************************
*�������ƣ�wkup_init(void)
*�������ܣ���ʼ����������ģʽ
*����ֵ����
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-4
*�޸����ڣ�
********************************/
void wkup_init(void)
{
	
	GPIO_InitTypeDef  GPIO_Init;
	EXTI_InitTypeDef  EXTI_InitStructure;
	
	__HAL_RCC_PWR_CLK_ENABLE();             //ʹ��ʱ��Դ
	__HAL_RCC_GPIOA_CLK_ENABLE();           //ʹ��GPIOA
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	
	
	GPIO_Init.Pin  = GPIO_PIN_0;
	GPIO_Init.Mode =  GPIO_MODE_INPUT;       //����ģʽ
	GPIO_Init.Pull = GPIO_PULLDOWN;          //��������
	GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH ;  //����
	
	HAL_GPIO_Init(GPIOA,&GPIO_Init);
	__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
	
	if(Check_KEY_UP()==0)
	{
		Sys_Enter_Standby();	//���ǿ���,�������ģʽ
	}
	
	
 
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE0
  EXTI_Init(&EXTI_InitStructure);//����

	
	
	//�жϿ���
	 HAL_NVIC_SetPriority(EXTI0_IRQn,NVIC_PRIORITYGROUP_2, 2);
	 HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/*****************************
*�������ƣ�Sys_Enter_Standby(void)
*�������ܣ�ϵͳ�������ģʽ
*����ֵ����
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-4
*�޸����ڣ�
********************************/
void Sys_Enter_Standby(void)
{
	while(KEY_UP);
	
	RCC->AHB1RSTR &= ~(0X04FF);                           //��λ���е�IO��
	__HAL_RCC_PWR_CLK_ENABLE();                           //ʹ��ʱ��Դ
		
	HAL_PWR_EnableBkUpAccess();                           //ʹ�ܷ���RTC��
	 	
	//�ر����RTC�ж�	
	__HAL_RTC_ALARM_EXTI_DISABLE_IT();
	__HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_IT();
	__HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_IT();
	
	//�������RTC����жϱ�־λ
	__HAL_RTC_ALARM_EXTI_CLEAR_FLAG();
	__HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
	__HAL_RTC_TAMPER_TIMESTAMP_EXTI_CLEAR_FLAG();
	
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                    //���WK_UP���ű�־λ
	
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);             //����ʹ��WK_UP���Ż���
	
	HAL_PWR_EnterSTANDBYMode();                           //�������ģʽ
}



