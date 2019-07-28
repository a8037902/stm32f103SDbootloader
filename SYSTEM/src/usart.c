

/* Includes ------------------------------------------------------------------*/
#include "sys.h"	


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 0
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif

#define DATA_BUF_SIZE	USART_BUF_SIZE

typedef struct usart_struct {
    
	u8 *pRecBufH;
	u8 *pRecBufE;
	u8 *pSendBufH;
	u8 *pSendBufE;
	u8 receiveBuf[DATA_BUF_SIZE];
	u8 sendBuf[DATA_BUF_SIZE];
	
} sutUSART;

//static sutUSART g_Usart1;

//static sutUSART g_Usart2;

static sutUSART g_Usarts[3];


void usart_Init(USART_TypeDef *usart, GPIO_TypeDef * gpio, uint16_t tx, uint16_t rx, IRQn_Type irq, u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
     //USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = tx; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(gpio, &GPIO_InitStructure);
   
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = rx;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(gpio, &GPIO_InitStructure);  

	//Usart1 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(usart, &USART_InitStructure); //��ʼ������
	USART_ITConfig(usart, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(usart, ENABLE);                    //ʹ�ܴ��� 
}

u16 usart_SetData(u8 *pData, u16 size, sutUSART *p)
{
	OS_CPU_SR cpu_sr=0;
	u8 *pH, *pE;
	u16 i=0;
	
	if(size == 0)return 0;
	OS_ENTER_CRITICAL();
	pH = p->pSendBufH;
	pE = p->pSendBufE;
	OS_EXIT_CRITICAL();
	
	for(i=0;i<size;i++)
	{
		*pH = *pData;
		pH++;
		pData++;
		if(pH - p->sendBuf >= DATA_BUF_SIZE)pH = p->sendBuf;
		if(pH == pE)break;
	}
	
	if(i)
	{
		p->pSendBufH = pH;
		//USART_ITConfig(USART, USART_IT_TXE, ENABLE);//?a???D??
	}

	return i;
}

u16 usart_GetData(u8 *pData, u16 size, sutUSART *p)
{
	//u32 data;
	OS_CPU_SR cpu_sr=0;
	u8 *pH, *pE;
	u16 i=0;
	u16 len=0;
	OS_ENTER_CRITICAL();
	pH = p->pRecBufH;
	pE = p->pRecBufE;
	OS_EXIT_CRITICAL();
	if(pH > pE)
	{
		len = pH - pE;
		if(len>size && size != 0)len = size;
			
		i = len;
			
		while(i--)
		{
			*pData = *pE;
			++pData;
			++pE;
		}
		p->pRecBufE = pE;		
	}
	else if(pH < pE)
	{
		len = DATA_BUF_SIZE-(p->pRecBufE - p->pRecBufH);
		if(len>size && size != 0)len = size;
		
		i = len;
		while(i--)
		{
			*pData = *pE;
			++pData;
			++pE;
			if((pE-p->receiveBuf) >= DATA_BUF_SIZE)				
			{
				pE = p->receiveBuf;
			}
		}
		p->pRecBufE = pE;
	}
	return len;
}
  

/*******************************************************************************
* Function Name  : uart_init.
* Descriptioan   : ���ڳ�ʼ��.
* Input          : ������.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Usart_Init(int usart, u32 bound){
	g_Usarts[usart].pRecBufH = g_Usarts[usart].receiveBuf;
	g_Usarts[usart].pRecBufE = g_Usarts[usart].receiveBuf;
	g_Usarts[usart].pSendBufH = g_Usarts[usart].sendBuf;
	g_Usarts[usart].pSendBufE = g_Usarts[usart].sendBuf;
	if(usart==0){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ��
		usart_Init(USART1,GPIOA,GPIO_Pin_9,GPIO_Pin_10,USART1_IRQn,bound); 
	}else if(usart==1){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
		usart_Init(USART2,GPIOA,GPIO_Pin_2,GPIO_Pin_3,USART2_IRQn,bound); 
	}else if(usart==2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
		usart_Init(USART3,GPIOB,GPIO_Pin_10,GPIO_Pin_11,USART3_IRQn,bound); 
	}
	
}

void Usart_BufInit(int usart){
	g_Usarts[usart].pRecBufH = g_Usarts[usart].receiveBuf;
	g_Usarts[usart].pRecBufE = g_Usarts[usart].receiveBuf;
	g_Usarts[usart].pSendBufH = g_Usarts[usart].sendBuf;
	g_Usarts[usart].pSendBufE = g_Usarts[usart].sendBuf;
	
}


u16 Usart_SetData(int usart, u8 *pData, u16 size)
{
#if USART_SUPPORT_MUX_TASK
	OS_CPU_SR cpu_sr=0;
#endif
	u16 r;
	USART_TypeDef *u;
	sutUSART *s;
	if(usart == 0){
		u=USART1;
		//s=&g_Usart1;
	}else if(usart ==1 ){
		u=USART2;
		//s=&g_Usart2;
	}else if(usart ==2 ){
		u=USART3;
		//s=&g_Usart2;
	}
	s=&g_Usarts[usart];
	
#if USART_SUPPORT_MUX_TASK
	OS_ENTER_CRITICAL();
#endif
	r= usart_SetData(pData,size,s);
	if(r && !(u->CR1&(uint32_t)(1<<7))){
		u->CR1 |= 1<<7;
	}
#if USART_SUPPORT_MUX_TASK
	OS_EXIT_CRITICAL();
#endif
	return r;
}

u16 Usart_GetData(int usart, u8 *pData, u16 size)
{
#if USART_SUPPORT_MUX_TASK
	OS_CPU_SR cpu_sr=0;
#endif
	u16 r;
	sutUSART *s;
//	if(usart == 0){
//		s=&g_Usart1;
//	}else if(usart ==1 ){
//		s=&g_Usart2;
//	}
	s=&g_Usarts[usart];
#if USART_SUPPORT_MUX_TASK
	OS_ENTER_CRITICAL();
#endif
	r = usart_GetData(pData,size,s);
#if USART_SUPPORT_MUX_TASK
	OS_EXIT_CRITICAL();
#endif
	return r;
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
//	u8 Res;
	OSIntEnter();

	if((USART1->SR & (uint32_t)(1<<5)) && (USART1->CR1 & (uint32_t)(1<<5)))
	{	
		*g_Usarts[0].pRecBufH = USART1->DR & (uint16_t)0x01FF;
		++g_Usarts[0].pRecBufH;
		if((g_Usarts[0].pRecBufH-g_Usarts[0].receiveBuf) >= DATA_BUF_SIZE)
		{
			g_Usarts[0].pRecBufH = g_Usarts[0].receiveBuf;
		}
	}	
	
	if((USART1->SR & (uint32_t)(1<<7)) && (USART1->CR1 & (uint32_t)(1<<7)))
	{
		//USART_ClearITPendingBit(USART, USART_IT_TXE);
		USART1->DR = *g_Usarts[0].pSendBufE;
		g_Usarts[0].pSendBufE++;
		if(g_Usarts[0].pSendBufE - g_Usarts[0].sendBuf >= DATA_BUF_SIZE)g_Usarts[0].pSendBufE = g_Usarts[0].sendBuf;
		if(g_Usarts[0].pSendBufE == g_Usarts[0].pSendBufH)		//����?������3��
		{
			USART1->CR1 &= ~((uint32_t)(1<<7));
		}
	}	
	
	OSIntExit();			
}

void USART2_IRQHandler(void)                	//����1�жϷ������
{
//	u8 Res;
	OSIntEnter();

	if((USART2->SR & (uint32_t)(1<<5)) && (USART2->CR1 & (uint32_t)(1<<5)))
	{	
		*g_Usarts[1].pRecBufH = USART2->DR & (uint16_t)0x01FF;
		++g_Usarts[1].pRecBufH;
		if((g_Usarts[1].pRecBufH-g_Usarts[1].receiveBuf) >= DATA_BUF_SIZE)
		{
			g_Usarts[1].pRecBufH = g_Usarts[1].receiveBuf;
		}
	}	
	
	if((USART2->SR & (uint32_t)(1<<7)) && (USART2->CR1 & (uint32_t)(1<<7)))
	{
		//USART_ClearITPendingBit(USART, USART_IT_TXE);
		USART2->DR = *g_Usarts[1].pSendBufE;
		g_Usarts[1].pSendBufE++;
		if(g_Usarts[1].pSendBufE - g_Usarts[1].sendBuf >= DATA_BUF_SIZE)g_Usarts[1].pSendBufE = g_Usarts[1].sendBuf;
		if(g_Usarts[1].pSendBufE == g_Usarts[1].pSendBufH)		//
		{
			USART2->CR1 &= ~((uint32_t)(1<<7));
		}
	}	
	
	OSIntExit();			
}

void USART3_IRQHandler(void)                	//����1�жϷ������
{
//	u8 Res;
	OSIntEnter();

	if((USART3->SR & (uint32_t)(1<<5)) && (USART3->CR1 & (uint32_t)(1<<5)))
	{	
		*g_Usarts[2].pRecBufH = USART3->DR & (uint16_t)0x01FF;
		++g_Usarts[2].pRecBufH;
		if((g_Usarts[2].pRecBufH-g_Usarts[2].receiveBuf) >= DATA_BUF_SIZE)
		{
			g_Usarts[2].pRecBufH = g_Usarts[2].receiveBuf;
		}
	}	
	
	if((USART3->SR & (uint32_t)(1<<7)) && (USART3->CR1 & (uint32_t)(1<<7)))
	{
		//USART_ClearITPendingBit(USART, USART_IT_TXE);
		USART3->DR = *g_Usarts[2].pSendBufE;
		g_Usarts[2].pSendBufE++;
		if(g_Usarts[2].pSendBufE - g_Usarts[2].sendBuf >= DATA_BUF_SIZE)g_Usarts[2].pSendBufE = g_Usarts[2].sendBuf;
		if(g_Usarts[2].pSendBufE == g_Usarts[2].pSendBufH)		//
		{
			USART3->CR1 &= ~((uint32_t)(1<<7));
		}
	}	
	
	OSIntExit();			
}

/************************ (C) COPYRIGHT �ڰ��ϴ� *****END OF FILE***************/

