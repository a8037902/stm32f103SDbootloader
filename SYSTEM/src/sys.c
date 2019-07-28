

/* Includes ------------------------------------------------------------------*/
#include "sys.h"


/*******************************************************************************
* Function Name  : NVIC_SETPRIMASK.
* Descriptioan   : 屏蔽所以中断.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_SETPRIMASK(void)//提升优先级至0
{
	__set_PRIMASK(1);
}


/*******************************************************************************
* Function Name  : NVIC_RESETPRIMASK.
* Descriptioan   : 中断屏蔽移除.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_RESETPRIMASK(void)//放开优先级
{
  __set_PRIMASK(0);
}


/*******************************************************************************
* Function Name  : NVIC_SETBASEMASK.
* Descriptioan   : 设置屏蔽中断值.
* Input          : 屏蔽值.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_SETBASEMASK(uint32_t basePri)//
{
	__set_BASEPRI(basePri);
}


/*******************************************************************************
* Function Name  : NVIC_GETBASEMASK.
* Descriptioan   : 获取屏蔽中断值.
* Input          : None.
* Output         : None.
* Return         : 屏蔽值.
*******************************************************************************/
uint32_t NVIC_GETBASEMASK(void)//
{
  return __get_BASEPRI();
}


/*******************************************************************************
* Function Name  : NVIC_Configuration.
* Descriptioan   : NVIC中断配置.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_Configuration(void)
{
	//NVIC_InitTypeDef NVIC_InitStructure;

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
}

u16 DebugLevel=MY_DEBUG;
/*******************************************************************************
* Function Name  : Debug_Printf.
* Descriptioan   : 调试打印.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Debug_Printf(u16 level, const char *s,...)
{
	OS_CPU_SR cpu_sr=0;
	va_list argp;  
	u8 buf[100];
	
	u16 l;
	
	OS_ENTER_CRITICAL();
	l = DebugLevel;
	OS_EXIT_CRITICAL();
	if(level<l){
		va_start( argp, s); 
		vsprintf((char*)buf,s,argp);
		va_end( argp );
		Usart_SetData(0,buf,strlen((const char*)buf));
	}
	
}

/************************ (C) COPYRIGHT 黑帮老大 *****END OF FILE***************/
