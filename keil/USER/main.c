#include "dht11.h"
#include "delay.h"
#include "led.h"
#include "uart.h"
#include "adc3.h"
#include "light_sensor.h"
#include "beep.h"
#include "wkup.h"
#include "lcd.h"
#include "stdio.h"
#include "uCOS_II.h"



OS_EVENT * temp_sem;
OS_EVENT * humi_sem;
OS_EVENT * LightSensor_sem;

uint8_t   temp ;
uint8_t   humi ;
uint8_t   LightSensor;

//设置任务优先级
#define   START_TASK_PRIO      			10   //开始任务的优先级设置为最低
//设置任务堆栈大小
#define   START_STK_SIZE  				64
//创建任务堆栈空间	
OS_STK    START_TASK_STK[START_STK_SIZE];
//任务函数接口
void   start_task(void *pdata);	
 			   
//lcd显示任务
//设置任务优先级
#define LCD_TASK_PRIO       			9 
//设置任务堆栈大小
#define LCD_STK_SIZE  		    		64
//创建任务堆栈空间	
OS_STK LCD_TASK_STK[LCD_STK_SIZE];
//任务函数接口
void lcd_display_task(void *pdata);


//温度报警蜂鸣器任务
//设置任务优先级
#define TEMP_BEEP_TASK_PRIO       			5
//设置任务堆栈大小
#define TEMP_BEEP_STK_SIZE  					64
//创建任务堆栈空间	
OS_STK TEMP_BEEP_TASK_STK[TEMP_BEEP_STK_SIZE];
//任务函数接口
void temp_police_task(void *pdata);



//湿度报警蜂鸣器任务
//设置任务优先级
#define HUMI_BEEP_TASK_PRIO       			6
//设置任务堆栈大小
#define HUMI_BEEP_STK_SIZE  					64
//创建任务堆栈空间	
OS_STK HUMI_BEEP_TASK_STK[HUMI_BEEP_STK_SIZE];
//任务函数接口
void humi_police_task(void *pdata);


//光照报警蜂鸣器任务
//设置任务优先级
#define LIGHT_BEEP_TASK_PRIO       			7
//设置任务堆栈大小
#define LIGHT_BEEP_STK_SIZE  					64
//创建任务堆栈空间	
OS_STK LIGHT_BEEP_TASK_STK[LIGHT_BEEP_STK_SIZE];
//任务函数接口
void light_police_task(void *pdata);

int main(void)
{ 
 
	delay_init(SYS);		   //初始化延时函数
	LED_Init();                //初始化LED灯
	uart_init(BAUD_RATE);      //初始化波特率
	beep_init();               //初始化蜂鸣器
	DHT11_Init();              //初始化温湿度
	ADC3_Init();               //初始化ADC3
    Light_Sensor_Init();       //初始化光照强度
	wkup_init();               //初始化待机模式
	LCD_Init();           	   //初始化LCD FSMC接口
	
	
	
	POINT_COLOR = GREEN;           //画笔颜色：绿色
	LCD_ShowString(30,40,210,24,24,"he bi shi");
	LCD_ShowString(30,60,210,24,24,"2013111287");
	
	POINT_COLOR = RED;
	LCD_ShowString(30,80,210,24,24,"bai se xue yuan");
	LCD_ShowString(30,100,210,24,24,"bi ye she ji");
	OSInit();  	 			   //初始化UCOSII		 			  
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	
}

//开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		
	
	temp_sem = OSSemCreate(0);
	humi_sem = OSSemCreate(0);
	LightSensor_sem = OSSemCreate(0);
	
	
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(lcd_display_task,(void *)0,(OS_STK*)&LCD_TASK_STK[LCD_STK_SIZE-1],LCD_TASK_PRIO);						    				   
 	OSTaskCreate(temp_police_task,(void *)0,(OS_STK*)&TEMP_BEEP_TASK_STK[TEMP_BEEP_STK_SIZE-1],TEMP_BEEP_TASK_PRIO);
	OSTaskCreate(humi_police_task,(void *)0,(OS_STK*)&HUMI_BEEP_TASK_STK[HUMI_BEEP_STK_SIZE-1],HUMI_BEEP_TASK_PRIO);
	OSTaskCreate(light_police_task,(void *)0,(OS_STK*)&LIGHT_BEEP_TASK_STK[LIGHT_BEEP_STK_SIZE-1],LIGHT_BEEP_TASK_PRIO);
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}	  
   

//LCD显示任务
void lcd_display_task(void *pdata)
{
	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
	LCD_ShowString(30,170,200,16,16,"Humi:  %");
	LCD_ShowString(30,190,200,16,16,"Light: ");
	
	while(1)
	{	
		DHT11_Read_Data(&temp,&humi);
		
		if(temp==0||humi==0)
		{
			DHT11_Read_Data(&temp,&humi);
		}
		else
		{
			DHT11_Read_Data(&temp,&humi);
			LightSensor = Light_Sensor_Get();
			printf(" %d %d %d ",temp,humi,LightSensor);
			LCD_ShowNum(30+40,150,temp,2,16);		//显示温度	   		   
			LCD_ShowNum(30+40,170,humi,2,16);	    //显示湿度
			LCD_ShowNum(30+48,190,LightSensor,3,16);
			
			//当温度高于25℃，或者低于10℃时，发送警报任务
			if((temp<10)||(temp>25))
			{
				OSSemPost(temp_sem);
			}
		
			//当湿度高于65%，或者低于30%时，发送警报任务
			if((humi<30)||(humi>65))
			{
				OSSemPost(humi_sem);
			}
		
			//当光线强度大于80%时，发送警报任务
			if(LightSensor>80)
			{
				OSSemPost(LightSensor_sem);
			}
		
			delay_ms(5000);
		}
		

	}
 	
}



//温度报警任务
void temp_police_task(void *pdata)
{
	u8 err;
	
	/**通过蜂鸣器发出声音进行报警****
	（3S的频率，响3秒，停3秒）*****/
	while(1)
	{
		OSSemPend(temp_sem,0,&err);
		BEEP_ON();
		delay_ms(3000);
		BEEP_OFF();
		delay_ms(3000);
	};
}

//湿度报警任务
void humi_police_task(void *pdata)
{
	u8 err;
	
	/**通过通过蜂鸣器发出声音进行报警
	（5S的频率，响5秒，停5秒）*****/
	while(1)
	{
		OSSemPend(humi_sem,0,&err);
		BEEP_ON();
		delay_ms(5000);
		BEEP_OFF();
		delay_ms(5000);		
	};		
}


//光线报警任务
void light_police_task(void *pdata)
{

	u8 err;
	
	/**通过通过蜂鸣器发出声音进行报警
	（7S的频率，响7秒，停7秒）。***/
	while(1)
	{
		OSSemPend(LightSensor_sem,0,&err);
		BEEP_ON();
		delay_ms(7000);
		BEEP_OFF();
		delay_ms(7000);
	};
}

