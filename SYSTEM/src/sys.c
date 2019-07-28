

/* Includes ------------------------------------------------------------------*/
#include "sys.h"


/*******************************************************************************
* Function Name  : NVIC_SETPRIMASK.
* Descriptioan   : ���������ж�.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_SETPRIMASK(void)//�������ȼ���0
{
	__set_PRIMASK(1);
}


/*******************************************************************************
* Function Name  : NVIC_RESETPRIMASK.
* Descriptioan   : �ж������Ƴ�.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_RESETPRIMASK(void)//�ſ����ȼ�
{
  __set_PRIMASK(0);
}


/*******************************************************************************
* Function Name  : NVIC_SETBASEMASK.
* Descriptioan   : ���������ж�ֵ.
* Input          : ����ֵ.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_SETBASEMASK(uint32_t basePri)//
{
	__set_BASEPRI(basePri);
}


/*******************************************************************************
* Function Name  : NVIC_GETBASEMASK.
* Descriptioan   : ��ȡ�����ж�ֵ.
* Input          : None.
* Output         : None.
* Return         : ����ֵ.
*******************************************************************************/
uint32_t NVIC_GETBASEMASK(void)//
{
  return __get_BASEPRI();
}


/*******************************************************************************
* Function Name  : NVIC_Configuration.
* Descriptioan   : NVIC�ж�����.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NVIC_Configuration(void)
{
	//NVIC_InitTypeDef NVIC_InitStructure;

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
}

u16 DebugLevel=MY_DEBUG;
/*******************************************************************************
* Function Name  : Debug_Printf.
* Descriptioan   : ���Դ�ӡ.
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

/************************ (C) COPYRIGHT �ڰ��ϴ� *****END OF FILE***************/
