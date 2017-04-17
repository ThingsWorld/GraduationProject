#include "wkup.h"
#include "stm32f4xx_exti.h"
/**************************
*函数名：uint8_t Check_KEY_UP(void)
*函数功能：检查是否有key_up按键按下
*输入参数：无
*输出参数：无
*返回值：1 / 0
*作者：何必仕
*完成日期：2016-10-5
*修改日期：
***************************/
uint8_t Check_KEY_UP(void)
{
	uint8_t t=0;
	uint8_t tx=0;//记录松开的次数
	LED_ON(); 	
	while(1)
	{
		if(KEY_UP)//已经按下了
		{
			t++;
			tx=0;
		}else 
		{
			tx++; 
			if(tx>3)//超过90ms内没有WKUP信号
			{
				LED_OFF();
				return 0;//错误的按键,按下次数不够
			}
		}
		delay_ms(30);
		if(t>=100)//按下超过3秒钟
		{
			LED_ON(); 
			return 1; //按下3s以上了
		}
	}
}

/**************************
*函数名：void EXTI0_IRQHandler(void)
*函数功能：外部中断0服务
*输入参数：无
*输出参数：无
*返回值：无
*作者：何必仕
*完成日期：2016-10-6
*修改日期：
***************************/
void EXTI0_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI_IMR_MR0);                //清除标志位
	if(Check_KEY_UP())
	{
		Sys_Enter_Standby(); //进入待机模式
	}
}




/*****************************
*函数名称：wkup_init(void)
*函数功能：初始化待机唤醒模式
*返回值：无
*输入值：无
*作者：何必仕
*日期：2016-10-4
*修改日期：
********************************/
void wkup_init(void)
{
	
	GPIO_InitTypeDef  GPIO_Init;
	EXTI_InitTypeDef  EXTI_InitStructure;
	
	__HAL_RCC_PWR_CLK_ENABLE();             //使能时钟源
	__HAL_RCC_GPIOA_CLK_ENABLE();           //使能GPIOA
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	
	
	GPIO_Init.Pin  = GPIO_PIN_0;
	GPIO_Init.Mode =  GPIO_MODE_INPUT;       //输入模式
	GPIO_Init.Pull = GPIO_PULLDOWN;          //无上下拉
	GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH ;  //高速
	
	HAL_GPIO_Init(GPIOA,&GPIO_Init);
	__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
	
	if(Check_KEY_UP()==0)
	{
		Sys_Enter_Standby();	//不是开机,进入待机模式
	}
	
	
 
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
  EXTI_Init(&EXTI_InitStructure);//配置

	
	
	//中断控制
	 HAL_NVIC_SetPriority(EXTI0_IRQn,NVIC_PRIORITYGROUP_2, 2);
	 HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/*****************************
*函数名称：Sys_Enter_Standby(void)
*函数功能：系统进入待机模式
*返回值：无
*输入值：无
*作者：何必仕
*日期：2016-10-4
*修改日期：
********************************/
void Sys_Enter_Standby(void)
{
	while(KEY_UP);
	
	RCC->AHB1RSTR &= ~(0X04FF);                           //复位所有的IO口
	__HAL_RCC_PWR_CLK_ENABLE();                           //使能时钟源
		
	HAL_PWR_EnableBkUpAccess();                           //使能访问RTC域
	 	
	//关闭相关RTC中断	
	__HAL_RTC_ALARM_EXTI_DISABLE_IT();
	__HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_IT();
	__HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_IT();
	
	//清除所有RTC相关中断标志位
	__HAL_RTC_ALARM_EXTI_CLEAR_FLAG();
	__HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
	__HAL_RTC_TAMPER_TIMESTAMP_EXTI_CLEAR_FLAG();
	
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                    //清除WK_UP引脚标志位
	
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);             //重新使能WK_UP引脚唤醒
	
	HAL_PWR_EnterSTANDBYMode();                           //进入待机模式
}



