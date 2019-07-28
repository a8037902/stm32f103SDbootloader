#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SPI 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/13 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
 

//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25X16/24L01/JF24C							  
//SPI口初始化
//这里针是对SPI1的初始化
void SPIx_Init(void)
{	 
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;  //SPI CS  
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  //通用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOB,GPIO_Pin_12);  ///拉高片选，使SPI工作。
	///SPI 使能
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE ); 
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_CRCPolynomial = 7; //CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器



	/* Enable SPI1 */
	SPI_Cmd(SPI2, ENABLE);

	SPIx_ReadWriteByte(0xff);//启动传输   
}   
//SPI 速度设置函数
//SpeedSet:
//SPI_SPEED_2   2分频   (SPI 36M@sys 72M)
//SPI_SPEED_8   8分频   (SPI 9M@sys 72M)
//SPI_SPEED_16  16分频  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256分频 (SPI 281.25K@sys 72M)
void SPIx_SetSpeed(u8 SpeedSet)
{
	SPI2->CR1&=0XFFC7;//Fsck=Fcpu/256
	switch(SpeedSet)
	{
		case SPI_SPEED_2://二分频
			SPI2->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz
			break;
		case SPI_SPEED_4://四分频
			SPI2->CR1|=1<<3;//Fsck=Fpclk/4=18Mhz
			break;
		case SPI_SPEED_8://八分频
			SPI2->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz
			break;
		case SPI_SPEED_16://十六分频
			SPI2->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
			break;
		case SPI_SPEED_256://256分频
			SPI2->CR1|=7<<3;//Fsck=Fpclk/16=281.25Khz
			break;
	}		 
	SPI2->CR1|=1<<6; //SPI设备使能	  
} 
//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPIx_ReadWriteByte(u8 TxData)
{		
 	while((SPI2->SR&1<<1)==0);
	SPI2->DR=TxData;	 	  //发送一个byte   
	while((SPI2->SR&1<<0)==0);						    
	return SPI2->DR;          //返回收到的数据				    
}

void SPIx_Close(){
	SPI_Cmd(SPI2, DISABLE);
}































