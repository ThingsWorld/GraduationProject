#include "delay.h"
#include "stm32f4xx_hal_cortex.h"



#if SYSTEM_SUPPORT_OS
#include "uCOS_II.h"				//支持OS时，使用	  
#endif


static u8  fac_us=0;							//us延时倍乘数			   
static u16 fac_ms=0;							//ms延时倍乘数,在os下,代表每个节拍的ms数
	
#if SYSTEM_SUPPORT_OS							//如果SYSTEM_SUPPORT_OS定义了,说明要支持OS了(不限于UCOS).

#ifdef 	OS_CRITICAL_METHOD						//OS_CRITICAL_METHOD定义了,说明要支持UCOSII				
#define delay_osrunning		OSRunning			//OS是否运行标记,0,不运行;1,在运行
#define delay_ostickspersec	OS_TICKS_PER_SEC	//OS时钟节拍,即每秒调度次数
#define delay_osintnesting 	OSIntNesting		//中断嵌套级别,即中断嵌套次数
#endif

/*********************************
*函数名称：SysTick_Handler
*函数功能：systick中断服务函数,使用OS时用到
*返回值：无
*输入值：无
*作者：何必仕
*日期：2016-10-11
*修改日期：
********************************/
void SysTick_Handler(void)
{	
	if(delay_osrunning==1)					//OS开始跑了,才执行正常的调度处理
	{
		OSIntEnter();						//进入中断
		OSTimeTick();       				//调用ucos的时钟服务程序               
		OSIntExit();       	 				//触发任务切换软中断
	}
}
#endif
			   
/*****************************
*函数名称：void delay_init(uint8_t SYSCLK)
*函数功能：初始化延时函数
*返回值：无
*输入值：SYSCLK 系统时钟频率
*作者：何必仕
*日期：2016-10-11
*修改日期：
*描述：当使用ucos的时候,此函数会初始化ucos的时钟节拍
       SYSTICK的时钟固定为AHB时钟的1/8
       SYSCLK:系统时钟频率
********************************/
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
	u32 reload;
#endif
//	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);	
	SysTick->CTRL&=~(1<<2);	
	fac_us=SYSCLK/8;						//不论是否使用OS,fac_us都需要使用
#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
	reload=SYSCLK/8;						//每秒钟的计数次数 单位为M	   
	reload*=1000000/delay_ostickspersec;	//根据delay_ostickspersec设定溢出时间
											//reload为24位寄存器,最大值:16777216,在168M下,约合0.7989s左右	
	fac_ms=1000/delay_ostickspersec;		//代表OS可以延时的最少单位	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 					//每1/delay_ostickspersec秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 	//开启SYSTICK    
#else
	fac_ms=(u16)fac_us*1000;				//非OS下,代表每个ms需要的systick时钟数   
#endif
}								    

#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.

/*********************************
*函数名称：delay_us
*函数功能：延时nus
*返回值：
*输入值：nus:要延时的us数.nus:0~204522252(最大值即2^32/fac_us@fac_us=21)
*作者：何必仕
*日期：2016-10-11
*修改日期：
********************************/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	OSSchedLock();					        //阻止OS调度，防止打断us延时
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
	OSSchedUnlock();					//恢复OS调度											    
}  


/*********************************
*函数名称：delay_ms
*函数功能：延时nms
*返回值：无
*输入值：nms:要延时的ms数，nms:0~65535
*作者：何必仕
*日期：2016-10-11
*修改日期：
********************************/
void delay_ms(u16 nms)
{	
	if(delay_osrunning&&delay_osintnesting==0)//如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度)	    
	{		 
		if(nms>=fac_ms)						//延时的时间大于OS的最少时间周期 
		{ 
   			OSTimeDly(nms/fac_ms);	//OS延时
		}
		nms%=fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((u32)(nms*1000));				//普通方式延时
}
#else 

/*****************************
*函数名称：delay_us(uint32_t nus)
*函数功能：不用ucosii 时，延时us
*返回值：无
*输入值：us 
*作者：何必仕
*日期：2016-10-11
*修改日期：
********************************/
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//时间加载	  		 
	SysTick->VAL=0x00;        				//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //关闭计数器
	SysTick->VAL =0X00;       				//清空计数器 
}


/*****************************
*函数名称：delay_xms(u16 nms)
*函数功能：考虑到某些客户可能超频使用,延时ms
*返回值：无
*输入值：nms,ms 
*作者：何必仕
*日期：2016-10-11
*修改日期：
描述：SysTick->LOAD为24位寄存器,所以,最大延时为:
      nms<=0xffffff*8*1000/SYSCLK
      SYSCLK单位为Hz,nms单位为ms
      对168M条件下,nms<=798ms 
********************************/
void delay_xms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;			//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           			//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;     		  		//清空计数器	  	    
} 
/*********************************
*函数名称：delay_ms
*函数功能：延时nms
*返回值：无
*输入值：nms:0~65535
*作者：何必仕
*日期：2016-10-11
*修改日期：
********************************/
void delay_ms(u16 nms)
{	 	 
	u8 repeat=nms/540;						//这里用540,是考虑到某些客户可能超频使用,
											//比如超频到248M的时候,delay_xms最大只能延时541ms左右了
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

//static uint8_t  fac_us=0;							//us延时倍乘数			   
//static uint16_t fac_ms=0;							//ms延时倍乘数


///*****************************
//*函数名称：void delay_init(uint8_t SYSCLK)
//*函数功能：初始化延时函数
//*返回值：无
//*输入值：SYSCLK 系统时钟频率
//*作者：何必仕
//*日期：2016-2-26
//*修改日期：
//********************************/
//void delay_init(uint8_t SYSCLK)
//{
//	SysTick->CTRL  &= SysTick_CLKSource_HCLK_Div;  //
//	fac_us = SYSCLK/8;
//	fac_ms =(uint16_t)fac_us*1000; 
//}


///*****************************
//*函数名称：delay_us(uint32_t nus)
//*函数功能：延时us
//*返回值：无
//*输入值：us 
//*作者：何必仕
//*日期：2016-2-26
//*修改日期：
//********************************/
//void delay_us(uint32_t nus)
//{		
//	uint32_t temp;	    	 
//	SysTick->LOAD=nus*fac_us; 				//时间加载	  		 
//	SysTick->VAL=0x00;        				//清空计数器
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //开始倒数 	 
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //关闭计数器
//	SysTick->VAL =0X00;       				//清空计数器 
//}

///*****************************
//*函数名称：delay_xms(uint16_t nms), delay_ms(uint16_t ms)
//*函数功能：考虑到某些客户可能超频使用,延时ms
//*返回值：无
//*输入值：nms,ms 
//*作者：何必仕
//*日期：2016-10-2
//*修改日期：
//********************************/
//void delay_xms(uint16_t nms)
//{	 		  	  
//	uint32_t temp;		   
//	SysTick->LOAD=(uint32_t)nms*fac_ms;			//时间加载(SysTick->LOAD为24bit)
//	SysTick->VAL =0x00;           			//清空计数器
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数 
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
//	SysTick->VAL =0X00;     		  		//清空计数器	  	    
//} 



//void delay_ms(uint16_t nms)
//{	 	 
//	uint8_t repeat=nms/540;						//这里用540,是考虑到某些客户可能超频使用,
//											    //比如超频到248M的时候,delay_xms最大只能延时541ms左右了
//	uint16_t remain=nms%540;
//	while(repeat)
//	{
//		delay_xms(540);
//		repeat--;
//	}
//	if(remain)delay_xms(remain);
//} 

