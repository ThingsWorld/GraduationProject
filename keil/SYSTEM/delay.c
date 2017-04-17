#include "delay.h"
#include "stm32f4xx_hal_cortex.h"



#if SYSTEM_SUPPORT_OS
#include "uCOS_II.h"				//֧��OSʱ��ʹ��	  
#endif


static u8  fac_us=0;							//us��ʱ������			   
static u16 fac_ms=0;							//ms��ʱ������,��os��,����ÿ�����ĵ�ms��
	
#if SYSTEM_SUPPORT_OS							//���SYSTEM_SUPPORT_OS������,˵��Ҫ֧��OS��(������UCOS).

#ifdef 	OS_CRITICAL_METHOD						//OS_CRITICAL_METHOD������,˵��Ҫ֧��UCOSII				
#define delay_osrunning		OSRunning			//OS�Ƿ����б��,0,������;1,������
#define delay_ostickspersec	OS_TICKS_PER_SEC	//OSʱ�ӽ���,��ÿ����ȴ���
#define delay_osintnesting 	OSIntNesting		//�ж�Ƕ�׼���,���ж�Ƕ�״���
#endif

/*********************************
*�������ƣ�SysTick_Handler
*�������ܣ�systick�жϷ�����,ʹ��OSʱ�õ�
*����ֵ����
*����ֵ����
*���ߣ��α���
*���ڣ�2016-10-11
*�޸����ڣ�
********************************/
void SysTick_Handler(void)
{	
	if(delay_osrunning==1)					//OS��ʼ����,��ִ�������ĵ��ȴ���
	{
		OSIntEnter();						//�����ж�
		OSTimeTick();       				//����ucos��ʱ�ӷ������               
		OSIntExit();       	 				//���������л����ж�
	}
}
#endif
			   
/*****************************
*�������ƣ�void delay_init(uint8_t SYSCLK)
*�������ܣ���ʼ����ʱ����
*����ֵ����
*����ֵ��SYSCLK ϵͳʱ��Ƶ��
*���ߣ��α���
*���ڣ�2016-10-11
*�޸����ڣ�
*��������ʹ��ucos��ʱ��,�˺������ʼ��ucos��ʱ�ӽ���
       SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
       SYSCLK:ϵͳʱ��Ƶ��
********************************/
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
	u32 reload;
#endif
//	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);	
	SysTick->CTRL&=~(1<<2);	
	fac_us=SYSCLK/8;						//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
	reload=SYSCLK/8;						//ÿ���ӵļ������� ��λΪM	   
	reload*=1000000/delay_ostickspersec;	//����delay_ostickspersec�趨���ʱ��
											//reloadΪ24λ�Ĵ���,���ֵ:16777216,��168M��,Լ��0.7989s����	
	fac_ms=1000/delay_ostickspersec;		//����OS������ʱ�����ٵ�λ	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 					//ÿ1/delay_ostickspersec���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 	//����SYSTICK    
#else
	fac_ms=(u16)fac_us*1000;				//��OS��,����ÿ��ms��Ҫ��systickʱ����   
#endif
}								    

#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.

/*********************************
*�������ƣ�delay_us
*�������ܣ���ʱnus
*����ֵ��
*����ֵ��nus:Ҫ��ʱ��us��.nus:0~204522252(���ֵ��2^32/fac_us@fac_us=21)
*���ߣ��α���
*���ڣ�2016-10-11
*�޸����ڣ�
********************************/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD��ֵ	    	 
	ticks=nus*fac_us; 						//��Ҫ�Ľ����� 
	OSSchedLock();					        //��ֹOS���ȣ���ֹ���us��ʱ
	told=SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
	OSSchedUnlock();					//�ָ�OS����											    
}  


/*********************************
*�������ƣ�delay_ms
*�������ܣ���ʱnms
*����ֵ����
*����ֵ��nms:Ҫ��ʱ��ms����nms:0~65535
*���ߣ��α���
*���ڣ�2016-10-11
*�޸����ڣ�
********************************/
void delay_ms(u16 nms)
{	
	if(delay_osrunning&&delay_osintnesting==0)//���OS�Ѿ�������,���Ҳ������ж�����(�ж����治���������)	    
	{		 
		if(nms>=fac_ms)						//��ʱ��ʱ�����OS������ʱ������ 
		{ 
   			OSTimeDly(nms/fac_ms);	//OS��ʱ
		}
		nms%=fac_ms;						//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	delay_us((u32)(nms*1000));				//��ͨ��ʽ��ʱ
}
#else 

/*****************************
*�������ƣ�delay_us(uint32_t nus)
*�������ܣ�����ucosii ʱ����ʱus
*����ֵ����
*����ֵ��us 
*���ߣ��α���
*���ڣ�2016-10-11
*�޸����ڣ�
********************************/
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//ʱ�����	  		 
	SysTick->VAL=0x00;        				//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //�رռ�����
	SysTick->VAL =0X00;       				//��ռ����� 
}


/*****************************
*�������ƣ�delay_xms(u16 nms)
*�������ܣ����ǵ�ĳЩ�ͻ����ܳ�Ƶʹ��,��ʱms
*����ֵ����
*����ֵ��nms,ms 
*���ߣ��α���
*���ڣ�2016-10-11
*�޸����ڣ�
������SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
      nms<=0xffffff*8*1000/SYSCLK
      SYSCLK��λΪHz,nms��λΪms
      ��168M������,nms<=798ms 
********************************/
void delay_xms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;			//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           			//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ���� 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;     		  		//��ռ�����	  	    
} 
/*********************************
*�������ƣ�delay_ms
*�������ܣ���ʱnms
*����ֵ����
*����ֵ��nms:0~65535
*���ߣ��α���
*���ڣ�2016-10-11
*�޸����ڣ�
********************************/
void delay_ms(u16 nms)
{	 	 
	u8 repeat=nms/540;						//������540,�ǿ��ǵ�ĳЩ�ͻ����ܳ�Ƶʹ��,
											//���糬Ƶ��248M��ʱ��,delay_xms���ֻ����ʱ541ms������
	u16 remain=nms%540;
	while(repeat)
	{
		delay_xms(540);
		repeat--;
	}
	if(remain)delay_xms(remain);
} 
#endif
			 

//#include "delay.h"
//#include "stm32f4xx_hal_cortex.h"

//static uint8_t  fac_us=0;							//us��ʱ������			   
//static uint16_t fac_ms=0;							//ms��ʱ������


///*****************************
//*�������ƣ�void delay_init(uint8_t SYSCLK)
//*�������ܣ���ʼ����ʱ����
//*����ֵ����
//*����ֵ��SYSCLK ϵͳʱ��Ƶ��
//*���ߣ��α���
//*���ڣ�2016-2-26
//*�޸����ڣ�
//********************************/
//void delay_init(uint8_t SYSCLK)
//{
//	SysTick->CTRL  &= SysTick_CLKSource_HCLK_Div;  //
//	fac_us = SYSCLK/8;
//	fac_ms =(uint16_t)fac_us*1000; 
//}


///*****************************
//*�������ƣ�delay_us(uint32_t nus)
//*�������ܣ���ʱus
//*����ֵ����
//*����ֵ��us 
//*���ߣ��α���
//*���ڣ�2016-2-26
//*�޸����ڣ�
//********************************/
//void delay_us(uint32_t nus)
//{		
//	uint32_t temp;	    	 
//	SysTick->LOAD=nus*fac_us; 				//ʱ�����	  		 
//	SysTick->VAL=0x00;        				//��ռ�����
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //��ʼ���� 	 
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //�رռ�����
//	SysTick->VAL =0X00;       				//��ռ����� 
//}

///*****************************
//*�������ƣ�delay_xms(uint16_t nms), delay_ms(uint16_t ms)
//*�������ܣ����ǵ�ĳЩ�ͻ����ܳ�Ƶʹ��,��ʱms
//*����ֵ����
//*����ֵ��nms,ms 
//*���ߣ��α���
//*���ڣ�2016-10-2
//*�޸����ڣ�
//********************************/
//void delay_xms(uint16_t nms)
//{	 		  	  
//	uint32_t temp;		   
//	SysTick->LOAD=(uint32_t)nms*fac_ms;			//ʱ�����(SysTick->LOADΪ24bit)
//	SysTick->VAL =0x00;           			//��ռ�����
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ���� 
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
//	SysTick->VAL =0X00;     		  		//��ռ�����	  	    
//} 



//void delay_ms(uint16_t nms)
//{	 	 
//	uint8_t repeat=nms/540;						//������540,�ǿ��ǵ�ĳЩ�ͻ����ܳ�Ƶʹ��,
//											    //���糬Ƶ��248M��ʱ��,delay_xms���ֻ����ʱ541ms������
//	uint16_t remain=nms%540;
//	while(repeat)
//	{
//		delay_xms(540);
//		repeat--;
//	}
//	if(remain)delay_xms(remain);
//} 

