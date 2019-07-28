//#include "sys.h"
#include "SD.h"

//--------------------延时time us----------------------------
//时间偏长4%
//void delay_us(u32 time)
//{
//	int i;
//	do{
//		for(i=0;i<7;i++){}
//	}while(--time);

//}

//-------------------延时time ms-----------------------------
//时间偏长4%
//void delay_ms(u32 time)
//{
//	do{
//		delay_us(1000);
//	}while(--time);
//}

/********************************************************************************************
stm32的spi硬件初始化
********************************************************************************************/
void SpiIoInit(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1,ENABLE);	//GPIOA 的时钟 SPI的时钟	
	/* Configure SPI1 pins: CS, SCK, MISO and MOSI ---------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//配置片选线
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Spi1_Init(void)
{
	SPI_InitTypeDef   SPI_InitStructure;
	SpiIoInit();
	/* SPI1 Config -------------------------------------------------------------*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	/* Enable SPI1 */
	SPI_Cmd(SPI1, ENABLE);
}

void Write_Byte_SPI(u8 byte)
{
//	SPDR = byte;
//	while (!SPSR_Bit7);
    /* Send SPI1 data */
    SPI_I2S_SendData(SPI1, byte);
	/* Wait for SPI1 Tx buffer empty */
    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == SET);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == SET);
}
//----------------------------------------
u8 Read_Byte_SPI(void)
{
//	SPDR = 0xFF;
//	while (!SPSR_Bit7);
//	return SPDR;

//调试
//  return  1;
//
	Write_Byte_SPI(0xff);
	/* Wait for SPI1 data reception */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    /* Read SPI1 received data */
    return SPI_I2S_ReceiveData(SPI1);
}

u8 SD_Buffer[512];
u32 LastReadSector=0xFFFFFFFF;
//****************************************************************************
//Send a Command to SD-Card
//Return: the second byte of response register of SD-Card
//****************************************************************************
u8 Write_Command_SD(u8 cmd,u32 arg,u8 crc7)
{
	u8 tmp;
	u8 retry=0;

	//set MMC_Chip_Select to high (SD-Card disable)
	SD_Disable();

	//send 8 Clock Impulse
	Write_Byte_SPI(0xFF);			//for ZZZZZZZZ	state
	//set MMC_Chip_Select to low (SD-Card active)

	SD_Enable();
	delay_us(5);
	//send 6 Byte Command to SD-Card
	Write_Byte_SPI(cmd|0x40);   				//送头命令
	Write_Byte_SPI( (arg>>24) & 0xff );
	Write_Byte_SPI( (arg>>16) & 0xff );
	Write_Byte_SPI( (arg>> 8) & 0xff );
	Write_Byte_SPI(  arg&0xff);
	Write_Byte_SPI(crc7);       //仅仅对RESET有效的CRC效验码

	delay_us(2);
	//get 16 bit response
	Read_Byte_SPI(); //read the first byte,ignore it.
	do
	{  //Only last 8 bit is used here.Read it out.
		tmp = Read_Byte_SPI();
		retry++;
	}
	while((tmp==0xff)&&(retry<100));
	return(tmp);
}

//****************************************************************************
//Routine for Init SD card(SPI-MODE)
//****************************************************************************
u8 SD_Init(void)
{
	u8 retry,temp;
	u8 i;

	Spi1_Init();			//spi_init();
	//MMC_Port_Init(); //Init SPI port
	//for(i=0;i<200;i++) //Wait SD ready...
	{
	//	NOP();
	}

	delay_us(200);

	for (i=0;i<0xFF;i++)
	{
		Write_Byte_SPI(0xFF); //send 74 clock at least!!!
	}
	SD_Enable();								   
	//Send Command CMD0 to SD Card
	delay_us(50);
	retry=0;
	do
	{ //retry 200 times to send CMD0 command
		temp=Write_Command_SD(00,00,0x95);
		//retry++;
		if(++retry==200)
		{
			SD_Disable();  //set SD_Chip_Select to high
			return(INIT_CMD0_ERROR);//CMD0 Error!
		}
	}
	while(temp!=1);
	delay_ms(30);

	//Send Command CMD1 to SD-Card
	retry=0;
	do
	{
		delay_ms(1);
		temp=Write_Command_SD(0x01,0x00,0xFF);
		retry++;
		if(retry==100)
		{
			SD_Disable();  //set SD_Chip_Select to high
			return(INIT_CMD1_ERROR);//CMD1 Error!
		}
	}
	while(temp!=0);

	//r1 = MMC_SD_SendCommand(59, 0);//关crc		//disable CRC
	Write_Command_SD(59, 0,0xFF);
	Write_Command_SD(16,512,0xFF);			//sent com16 to sd to set a block len

	SD_Disable();  //set SD_Chip_Select to high

	return(0); //All commands have been taken.
}

u8 SD_Read_Block(u32 addr)
{
	u8 tmp;
	unsigned int i;
	//INT32U wait_time1=0;
	if (addr==LastReadSector)	return 1;		//减少重复读取的可能性
	LastReadSector=addr;
	addr<<=9;
	//tmp=Write_Command_SD(17,addr,0xff);
	tmp=Write_Command_SD(17,addr,0x95);
	while (Read_Byte_SPI()!= 0xfe){}//{WDR();++wait_time1;	}
	//if(wait_time1>=wait_time)	wait_time=wait_time1;
	
	for(i=0;i<512;i++)
	{
		SD_Buffer[i]=Read_Byte_SPI();
	}

	Write_Byte_SPI(0xff);			//伪crc
	Write_Byte_SPI(0xff);
	
	SD_Disable();
	return(tmp);
}

/*---- S   D _   W R I T E _   B L O C K ----
【功能】：写块
【参数】：****
【返回】：****
【说明】：****
--------------作者:镇关西   2010年5月27日星期四15:06:46--------------------------------*/
u8 SD_Write_Block(u32 addr,u8 *buffer)
{
	u8 tmp;
	u16 i;
	addr<<=9;
	//printf(F2R("开始写入%ld\n"),addr);
	i=0;
	do{	
		tmp=Write_Command_SD(24,addr,0xff);
		if(++i>10) return 1;
	}while(tmp!=0);
	SD_Disable();
	for(i=0;i<0x1000;i++);		//	asm("nop");
	SD_Enable();
	
	Write_Byte_SPI(0xff);
	Write_Byte_SPI(0xff);
	Write_Byte_SPI(0xff);
	Write_Byte_SPI(0xff);
	Write_Byte_SPI(0xff);
	Write_Byte_SPI(0xff);

	Write_Byte_SPI(0xfe);//发开始符			//send start byte

	//printf(F2R("写入数据\n"));
	for(i=0; i<512; i++)//送512字节数据		//send 512 bytes data
		Write_Byte_SPI(*buffer++);
	
	//printf(F2R("写入CRC数据\n"));
	Write_Byte_SPI(0xff);
	Write_Byte_SPI(0xff);
	
	tmp = Read_Byte_SPI();
	//printf(F2R("回复数据%d\n"),tmp);
	if( (tmp&0x1f) != 0x05)//等待是否成功	//judge if it successful
	{
		SD_Disable();
		return tmp;
	}
	while(!Read_Byte_SPI());//if(retry++ > 50){SPI_CS_Deassert();return 1;}
	SD_Disable();
	//printf(F2R("写入数据结束\n"));
	return 0;
}

u8 SD_Read_CID(u8 *dest)
{
	u8 ret;
	u16 i;
	do{	
		ret=Write_Command_SD(10,0x00,0xFF);//,0xff);
		i=0xFFFF;
		while (Read_Byte_SPI()!= 0xfe && --i>0);			//{	WDR();}
	}while(i==0);
	for(i=0;i<16;i++)
	{
		*dest++=Read_Byte_SPI();
	}
	SD_Disable();
	return ret;
}



