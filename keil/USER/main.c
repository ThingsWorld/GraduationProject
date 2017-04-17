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

//�����������ȼ�
#define   START_TASK_PRIO      			10   //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define   START_STK_SIZE  				64
//���������ջ�ռ�	
OS_STK    START_TASK_STK[START_STK_SIZE];
//�������ӿ�
void   start_task(void *pdata);	
 			   
//lcd��ʾ����
//�����������ȼ�
#define LCD_TASK_PRIO       			9 
//���������ջ��С
#define LCD_STK_SIZE  		    		64
//���������ջ�ռ�	
OS_STK LCD_TASK_STK[LCD_STK_SIZE];
//�������ӿ�
void lcd_display_task(void *pdata);


//�¶ȱ�������������
//�����������ȼ�
#define TEMP_BEEP_TASK_PRIO       			5
//���������ջ��С
#define TEMP_BEEP_STK_SIZE  					64
//���������ջ�ռ�	
OS_STK TEMP_BEEP_TASK_STK[TEMP_BEEP_STK_SIZE];
//�������ӿ�
void temp_police_task(void *pdata);



//ʪ�ȱ�������������
//�����������ȼ�
#define HUMI_BEEP_TASK_PRIO       			6
//���������ջ��С
#define HUMI_BEEP_STK_SIZE  					64
//���������ջ�ռ�	
OS_STK HUMI_BEEP_TASK_STK[HUMI_BEEP_STK_SIZE];
//�������ӿ�
void humi_police_task(void *pdata);


//���ձ�������������
//�����������ȼ�
#define LIGHT_BEEP_TASK_PRIO       			7
//���������ջ��С
#define LIGHT_BEEP_STK_SIZE  					64
//���������ջ�ռ�	
OS_STK LIGHT_BEEP_TASK_STK[LIGHT_BEEP_STK_SIZE];
//�������ӿ�
void light_police_task(void *pdata);

int main(void)
{ 
 
	delay_init(SYS);		   //��ʼ����ʱ����
	LED_Init();                //��ʼ��LED��
	uart_init(BAUD_RATE);      //��ʼ��������
	beep_init();               //��ʼ��������
	DHT11_Init();              //��ʼ����ʪ��
	ADC3_Init();               //��ʼ��ADC3
    Light_Sensor_Init();       //��ʼ������ǿ��
	wkup_init();               //��ʼ������ģʽ
	LCD_Init();           	   //��ʼ��LCD FSMC�ӿ�
	
	
	
	POINT_COLOR = GREEN;           //������ɫ����ɫ
	LCD_ShowString(30,40,210,24,24,"he bi shi");
	LCD_ShowString(30,60,210,24,24,"2013111287");
	
	POINT_COLOR = RED;
	LCD_ShowString(30,80,210,24,24,"bai se xue yuan");
	LCD_ShowString(30,100,210,24,24,"bi ye she ji");
	OSInit();  	 			   //��ʼ��UCOSII		 			  
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	
}

//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		
	
	temp_sem = OSSemCreate(0);
	humi_sem = OSSemCreate(0);
	LightSensor_sem = OSSemCreate(0);
	
	
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(lcd_display_task,(void *)0,(OS_STK*)&LCD_TASK_STK[LCD_STK_SIZE-1],LCD_TASK_PRIO);						    				   
 	OSTaskCreate(temp_police_task,(void *)0,(OS_STK*)&TEMP_BEEP_TASK_STK[TEMP_BEEP_STK_SIZE-1],TEMP_BEEP_TASK_PRIO);
	OSTaskCreate(humi_police_task,(void *)0,(OS_STK*)&HUMI_BEEP_TASK_STK[HUMI_BEEP_STK_SIZE-1],HUMI_BEEP_TASK_PRIO);
	OSTaskCreate(light_police_task,(void *)0,(OS_STK*)&LIGHT_BEEP_TASK_STK[LIGHT_BEEP_STK_SIZE-1],LIGHT_BEEP_TASK_PRIO);
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}	  
   

//LCD��ʾ����
void lcd_display_task(void *pdata)
{
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
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
			LCD_ShowNum(30+40,150,temp,2,16);		//��ʾ�¶�	   		   
			LCD_ShowNum(30+40,170,humi,2,16);	    //��ʾʪ��
			LCD_ShowNum(30+48,190,LightSensor,3,16);
			
			//���¶ȸ���25�棬���ߵ���10��ʱ�����;�������
			if((temp<10)||(temp>25))
			{
				OSSemPost(temp_sem);
			}
		
			//��ʪ�ȸ���65%�����ߵ���30%ʱ�����;�������
			if((humi<30)||(humi>65))
			{
				OSSemPost(humi_sem);
			}
		
			//������ǿ�ȴ���80%ʱ�����;�������
			if(LightSensor>80)
			{
				OSSemPost(LightSensor_sem);
			}
		
			delay_ms(5000);
		}
		

	}
 	
}



//�¶ȱ�������
void temp_police_task(void *pdata)
{
	u8 err;
	
	/**ͨ�������������������б���****
	��3S��Ƶ�ʣ���3�룬ͣ3�룩*****/
	while(1)
	{
		OSSemPend(temp_sem,0,&err);
		BEEP_ON();
		delay_ms(3000);
		BEEP_OFF();
		delay_ms(3000);
	};
}

//ʪ�ȱ�������
void humi_police_task(void *pdata)
{
	u8 err;
	
	/**ͨ��ͨ�������������������б���
	��5S��Ƶ�ʣ���5�룬ͣ5�룩*****/
	while(1)
	{
		OSSemPend(humi_sem,0,&err);
		BEEP_ON();
		delay_ms(5000);
		BEEP_OFF();
		delay_ms(5000);		
	};		
}


//���߱�������
void light_police_task(void *pdata)
{

	u8 err;
	
	/**ͨ��ͨ�������������������б���
	��7S��Ƶ�ʣ���7�룬ͣ7�룩��***/
	while(1)
	{
		OSSemPend(LightSensor_sem,0,&err);
		BEEP_ON();
		delay_ms(7000);
		BEEP_OFF();
		delay_ms(7000);
	};
}

