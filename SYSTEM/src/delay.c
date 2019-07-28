

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
	  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define  fac_us        (9)		   
//#define  fac_ms        (9000)
static int fac_us;
static int fac_ms;


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if SYSTEM_SUPPORT_UCOS
void SysTick_Handler(void)
{				   
	OSIntEnter();		    //�����ж�
	OSTimeTick();       //����ucos��ʱ�ӷ������               
	OSIntExit();        //���������л����ж�
}
#endif

			   

/*******************************************************************************
* Function Name  : delay_init.
* Descriptioan   : ��ʼ���ӳٺ���
*                  ��ʹ��ucos��ʱ��,�˺������ʼ��ucos��ʱ�ӽ���
*                  SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
*                  SYSCLK:ϵͳʱ��
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void delay_init()
{
	//SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8  
	
	
#if SYSTEM_SUPPORT_UCOS 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	u32 reload;
#endif
	SystemCoreClock=64000000;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	fac_us=SystemCoreClock/8000000;	//Ϊϵͳʱ�ӵ�1/8  
	 
#if SYSTEM_SUPPORT_UCOS 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
	reload=SystemCoreClock/8000000;		//ÿ���ӵļ������� ��λΪK	   
	reload*=1000000/OS_TICKS_PER_SEC;//����OS_TICKS_PER_SEC�趨���ʱ��
							//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����	
	fac_ms=1000/OS_TICKS_PER_SEC;//����ucos������ʱ�����ٵ�λ	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 	//ÿ1/OS_TICKS_PER_SEC���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK    
#else
	fac_ms=(u16)fac_us*1000;//��ucos��,����ÿ��ms��Ҫ��systickʱ����   
#endif
}								    



/*******************************************************************************
* Function Name  : delay_us.
* Descriptioan   : ΢����ʱ.
* Input          : ΢��.
* Output         : None.
* Return         : None.
*******************************************************************************/	
void delay_us(u32 nus)
{		
#if SYSTEM_SUPPORT_UCOS==0
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����
#else
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;	          //LOAD��ֵ	    	 
	ticks=nus*fac_us; 			            //��Ҫ�Ľ�����	  		 
	tcnt=0;
	OSSchedLock();				              //��ֹucos���ȣ���ֹ���us��ʱ
	told=SysTick->VAL;        	        //�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;   //����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;           //ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
	OSSchedUnlock();			              //����ucos���� 
#endif		
}



/*******************************************************************************
* Function Name  : delay_ms.
* Descriptioan   : ������ʱ.
* Input          : ����.
* Output         : None.
* Return         : None.
*******************************************************************************/
void delay_ms(u16 nms)
{	
#if SYSTEM_SUPPORT_UCOS==0
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ����� 
#else
	if(OSRunning==TRUE)               //���os�Ѿ�������	    
	{		  
		if(nms>=fac_ms)                 //��ʱ��ʱ�����ucos������ʱ������ 
		{
   			OSTimeDly(nms/fac_ms);      //ucos��ʱ
		}
		nms%=fac_ms;			              //ucos�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	delay_us((u32)(nms*1000));	      //��ͨ��ʽ��ʱ 
#endif
}

/************************ (C) COPYRIGHT �ڰ��ϴ� *****END OF FILE***************/

			 



































