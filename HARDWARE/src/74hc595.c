#include "74hc595.h"

static void delay(){
	int n=10;
	while(--n);
}

void HC595_Init(){
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA端口时钟
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
}

// data -(q0-q7)
void HC595_Write(u8 data)
{
	int i=0;
	u8 tmp=data;
	//GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	for(i=0;i<8;i++){
		if(tmp&0x01){
			GPIO_SetBits(GPIOB,GPIO_Pin_4);
		}else{
			GPIO_ResetBits(GPIOB,GPIO_Pin_4);
		}
		delay();
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		delay();
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		tmp>>=1;
	}
	//delay();
	delay();
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	delay();
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
}