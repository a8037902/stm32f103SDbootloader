

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYS_H
#define __SYS_H	

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include "stm32sys_cfg.h"

#define MY_DEBUG		10

#if SYSTEM_SUPPORT_UCOS
	#include "includes.h"
#else
	#define OSIntEnter	__nop
	#define OSIntExit	__nop
	#define OS_ENTER_CRITICAL	__nop
	#define OS_EXIT_CRITICAL	__nop
	#define OS_CPU_SR	unsigned int
		
	typedef unsigned char  BOOLEAN;
	typedef unsigned char  INT8U;			/* Unsigned  8 bit quantity       */
	typedef signed   char  INT8S;			/* Signed    8 bit quantity       */
	typedef unsigned short INT16U;			/* Unsigned 16 bit quantity       */
	typedef signed   short INT16S;			/* Signed   16 bit quantity       */
	typedef unsigned int   INT32U;			/* Unsigned 32 bit quantity       */
	typedef signed   int   INT32S;			/* Signed   32 bit quantity       */
	typedef float          FP32;			/* Single precision floating point*/
	typedef double         FP64;			/* Double precision floating point*/
#endif

#if SYSTEM_SUPPORT_UCGUI
	#include "GUI.H"
#endif



/* Exported types ------------------------------------------------------------*/
typedef enum {FALSE = 0, TRUE = !FALSE} BOOL;


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define countof(Array) (sizeof(Array)/sizeof(Array[0]))																	    
	 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum)    ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)           *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入



/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void     NVIC_SETPRIMASK   (void);
void     NVIC_RESETPRIMASK (void);
void     NVIC_SETBASEMASK  (uint32_t basePri);
uint32_t NVIC_GETBASEMASK  (void);
void     NVIC_Configuration(void);

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

void Usart_Init(int usart, u32 bound);
void Usart_BufInit(int usart);
u16 Usart_SetData(int usart, u8 *pData, u16 size);
u16 Usart_GetData(int usart, u8 *pData, u16 size);

void Debug_Printf(u16 level, const char *s,...);

/* External variables --------------------------------------------------------*/



#endif
