#ifndef __SD_h
#define __SD_h
#include "sys.h"
//------------------------------------------------------------
// Port define
//-------------------------------------------------------------
#define SD_Disable()	PAout(4)=1		//GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define SD_Enable()	PAout(4)=0		//GPIO_ResetBits(GPIOA, GPIO_Pin_4)
//------------------------------------------------------------
// Error define
//-------------------------------------------------------------
#define INIT_CMD0_ERROR     0x01
#define INIT_CMD1_ERROR		0x02
#define WRITE_BLOCK_ERROR	0x03
#define READ_BLOCK_ERROR   	0x04


//供给外部调用的程序
u8 SD_Init(void);
u8 SD_Read_Block(u32 addr);
u8 SD_Write_Block(u32 addr,u8 *buffer);
u8 Write_Command_SD(u8 cmd,u32 arg,u8 crc7);
u8 SD_Read_CID(u8 *dest);
extern u8 SD_Buffer[];
#endif

