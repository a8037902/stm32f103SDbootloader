#include "mmc_sd.h"
#include "diskio.h"
//#include "flash.h"
//#include "malloc.h"
//#include "ucos_ii.h"
#include  <stdio.h>	 

#define EX_FLASH 1	//外部flash,卷标为1
#define MMC_SD	 0

#define FLASH_SECTOR_SIZE 	4096

#define MMC_SD_SECTOR_COUNT	4096
#define MMC_SD_BLOCK_SIZE	1
//对于W25Q16
//全部2M字节都用作fatfs管理.
//FLASH_SECTOR_COUNT=4096
//对于W25Q32
//前2M字节给fatfs用,后2M字节~2M+500K给用户用,2M+500K以后,用于存放字库,字库占用1.5M.
//FLASH_SECTOR_COUNT=4096
//对于W25Q64 
//前6M字节给fatfs用,6M字节后~6M+500K给用户用,6M+500K以后,用于存放字库,字库占用1.5M.
//	
//FLASH_SECTOR_COUNT=2048*6 
u16	    FLASH_SECTOR_COUNT=512;//6M字节,默认为W25Q64
#define FLASH_BLOCK_SIZE  	1     //每个BLOCK有8个扇区



//初始化磁盘
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{	
	u8 res=0;	    
	switch(drv)
	{
		case EX_FLASH://外部flash
//			SPI_Flash_Init();
 			break;
		case MMC_SD:
			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //初始化成功
}   
//获得磁盘状态
DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{		   
    return 0;
}
 //读扇区
 //drv:磁盘编号0~9
 //*buff:数据接收缓冲首地址
 //sector:扇区地址
 //count:需要读取的扇区数
DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	u8 res=0; 
	//Debug_Printf(5,"disk_read=%d,%d\r\n",sector,count);
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(drv)
	{
		case EX_FLASH://外部flash
			for(;count>0;count--)
			{
				//SPI_Flash_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		case MMC_SD:
			for(;count>0;count--)
			{
				//SPI_Flash_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				SD_ReadDisk(buff,sector,1);
				sector++;
				buff+=MMC_SD_SECTOR_SIZE;
			}
			res=0;
			break;
		default:
			res=1; 
	}
  //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
  if(res==0x00)return RES_OK;	 
  else return RES_ERROR;	   
}  
 //写扇区
 //drv:磁盘编号0~9
 //*buff:发送数据首地址
 //sector:扇区地址
 //count:需要写入的扇区数	    
#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	        /* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	u8 res=0;  
	//Debug_Printf(5,"disk_write\r\n");
  if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(drv)
	{
		case EX_FLASH://外部flash
			for(;count>0;count--)
			{
				//SPI_Flash_Erase_Sector(sector);
				//SPI_Flash_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		case MMC_SD:
			for(;count>0;count--)
			{
				//SPI_Flash_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				SD_WriteDisk((u8*)buff,sector,1);
				sector++;
				buff+=MMC_SD_SECTOR_SIZE;
			}
			res=0;
			break;
		default:
			res=1; 
	}
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;		 
}
#endif /* _READONLY */

//其他表参数的获得
 //drv:磁盘编号0~9
 //ctrl:控制代码
 //*buff:发送/接收缓冲区指针
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{	
	DRESULT res;		
	Debug_Printf(5,"disk_ioctl=%d,%d\r\n",drv,ctrl);
	
	if(drv==MMC_SD)	//外部FLASH  
	{
	    switch(ctrl)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = MMC_SD_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = MMC_SD_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(WORD*)buff = SD_GetSectorCount();//MMC_SD_SECTOR_COUNT;
		        Debug_Printf(5,"SD_GetSectorCount=%d\r\n",*(WORD*)buff);
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else res=RES_ERROR;//其他的不支持
  return res;
}   
//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
//    u32 ttime;
//	u32 date=0;
//    
//    RTC_Get();		//得到当前时间
//    ttime=calendar.w_year-1980;	//得到偏移后的年份
// 	date|=ttime<<25;
//    ttime=calendar.w_month;		//得到月份
// 	date|=ttime<<21;
//	ttime=calendar.w_date;		//得到日期
// 	date|=ttime<<16;
//	ttime=calendar.hour;		//得到时钟
// 	date|=ttime<<11;
//	ttime=calendar.min;			//得到分钟
// 	date|=ttime<<5;
// 	date|=calendar.min>>1;  	//得到秒钟	   			    
//    return date;   
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////
//OS_CPU_SR cpu_sr=0;
void ff_enter(void)
{
 	//OS_ENTER_CRITICAL();//进入临界区(无法被中断打断)    
}
void ff_leave(void)
{	 
 	//OS_EXIT_CRITICAL();	//退出临界区(可以被中断打断)
}
/////////////////////////////////////////////////////////////////////////////////////

//动态分配内存
/*void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
//释放内存
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}*/



