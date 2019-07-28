/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "config.h"
#include "mmc_sd.h"
#include "ff.h"
#include "74hc595.h"
#include "DES.h"
//#include "FlashEncode.h"

#if AT_EN
#include "at.h"
#endif



//#include "userIO.h"


#include <stdlib.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define START_ADDRESS	0x8009000

#if SYSTEM_SUPPORT_UCOS
//设置任务堆栈大小
#define STK_SIZE      64

//设置任务优先级
#define TASK_Prio     10

//供分配的内存大小
#define MEM_SIZE_BIG		64
#define MEM_SIZE_LIT		32
#define MEM_NUM_BIG			20
#define MEM_NUM_LIT			10
#define MEM_NUM				MEM_NUM_LIT
#define MEM_SIZE			MEM_SIZE_BIG
#endif


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FIL fnew;
FATFS fs;
FRESULT res;
UINT br,bw;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#if SYSTEM_SUPPORT_UCOS
//任务申明
void TaskStart(void *pdata);
void TaskOne(void *pdata);
void TaskTwo(void *pdata);	
void TaskThree(void *pdata);

//_TASK任务 _NUM堆栈大小 _PRIO优先级
#define CREATE_TASK(_TASK,_STKSIZE,_PRIO,_PARA)	do{\
		static OS_STK _TASK##_Stk[_STKSIZE];			\
		OSTaskCreate(_TASK, (void *)_PARA, &_TASK##_Stk[_STKSIZE-1],_PRIO);\
	}while(0)
#define	CREATE_SMSG(_OSQ,_NUM)	do{\
		static void* _OSQ##_MSG[_NUM];	\
		_OSQ =	OSQCreate(_OSQ##_MSG, _NUM); \
	}while(0)
#define CREATE_MEM(_OSMEM,_NUM,_SIZE)	do{\
		INT8U err;\
		static INT8U _OSMEM##_MEM[_NUM][_SIZE];	\
		_OSMEM =	OSMemCreate((void*)_OSMEM##_MEM, _NUM, _SIZE, &err); \
	}while(0)

#endif


/* Private functions ---------------------------------------------------------*/
void BootLoader_Jump(u32 sect, u32 msp, u32 reset){
	u32 base;
	u32 offset;
	base = (sect > NVIC_VectTab_FLASH)?NVIC_VectTab_FLASH : NVIC_VectTab_RAM;
	offset = sect-base;
	
	NVIC_SetVectorTable(base, offset);
	
	__set_MSP(msp);
	
	((void(*)())(reset))();
	
}

void FLASH_EraseAll(unsigned long dataSize){
	u8 i;
	u8 page = dataSize/PAGE_SIZE+1;
	for(i=0;i<page;i++){
		FLASH_ErasePage(START_ADDRESS+(i*PAGE_SIZE));
	}
}

void FLASH_ReadBuffer(u32 addr, u16 *pData, u16 num){
	u16 i;
	for(i=0;i<num;i++){
		*(pData+i) = *(vu16 *)(addr+i*2);
	}
}


void Flash_Encode (void);

const char KEYSTR[]={"sdjlfj108&)$#lgksr;kf5094s388$*"};

/*******************************************************************************
* Function Name  : main.
* Descriptioan   : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{	
	u32 i=0;
	u8 data[2];
	u16 buf[4];
	u32 msp;
	u32 reset;
	u8 sdRes;
	u16 index=0;
	u16 len = sizeof(KEYSTR);
	NVIC_Configuration();
	
#if ENCRYPTION==1
	if(*(u32*)Flash_Encode!=0xFFFFFFFF){
		FLASH_Unlock();
		Flash_Encode();
		FLASH_ErasePage(ENCRYPT_FLASH_ADDRESS);
		FLASH_Lock();
	}
#endif
	HC595_Init();
	delay_init();
	Usart_Init(0,115200);

	sdRes = SD_Initialize();
	
	Debug_Printf(5,"SD Init %d...\r\n",sdRes);
	
	delay_ms(100);
	//Debug_Printf(5,"SD Init Complete\r\n");
	
	if(sdRes==0){
		f_mount(0,&fs);
		fs.drv=0;
		res = f_open(&fnew,"0:demo.bin", FA_OPEN_EXISTING | FA_READ);
		Debug_Printf(5,"Open demo.bin %d...\r\n",res);
		delay_ms(100);
		if(res == FR_OK){
			FLASH_Unlock();
			FLASH_EraseAll(fnew.fsize);
			
			f_lseek(&fnew, 0);
			br=0;
			for(i=0;i<(fnew.fsize+1)/2;i++){
				f_read(&fnew, data, 2, &br);
				data[0]^=KEYSTR[index++];
				if(index>=len){
					index=0;
				}
				data[1]^=KEYSTR[index++];
				if(index>=len){
					index=0;
				}
				FLASH_ProgramHalfWord(START_ADDRESS+(i*2),data[0]+(data[1]<<8));
			}
			index=0;
			f_lseek(&fnew, 0);
			br=0;
			for(i=0;i<(fnew.fsize+1)/2;i++){
				f_read(&fnew, data, 2, &br);
				data[0]^=KEYSTR[index++];
				if(index>=len){
					index=0;
				}
				data[1]^=KEYSTR[index++];
				if(index>=len){
					index=0;
				}
				if(*(u16*)(START_ADDRESS+(i*2))!=data[0]+(data[1]<<8)){
					Debug_Printf(5,"Update demo.bin Fail %d...\r\n",START_ADDRESS+(i*2));
				}
			}
			f_close(&fnew);
			f_mount(0,0);
			FLASH_Lock();
			Debug_Printf(5,"Load demo.bin OK\r\n");
		}
		
		delay_ms(100);
		return 0;
	}
	
	SD_Close();
	
	FLASH_ReadBuffer(START_ADDRESS, buf, 4);
	msp = buf[0] +(buf[1]<<16);
	reset = buf[2] + (buf[3]<<16);
	
	BootLoader_Jump(START_ADDRESS, msp, reset);

	return 0;	
}



