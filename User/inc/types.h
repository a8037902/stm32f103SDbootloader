#ifndef __TYPES_H
#define __TYPES_H

#include "config.h"


typedef enum {OPEN = 0, CLOSE = !OPEN} PLUSE_MODE;


typedef enum{
	TASK_IO,
	TASK_COMM,
	TASK_EEPROM_WRITE
} Task_TypeDef;

typedef enum{
	INPUT_IDLE=0,
	INPUT_DOWN,
	INPUT_LONG_DOWN,
	INPUT_VIBRATE,
	INPUT_VIBRATE2,
	INPUT_VIBRATE3,
} Input_TypeDef;


typedef enum{
	MESSAGE_NULL,
	MESSAGE_EEPROM_WRITE,
	MESSAGE_WT588D,
} Message_TypeDef;

typedef struct _Message {
	unsigned short int message;
	unsigned short int target;
	union {
        unsigned char data8[8];
		unsigned short int data16[4];
		unsigned int data32[2];
    } data;
} Message_sut;


typedef struct _Input {
	unsigned short int inputOffTime;
	unsigned short int inputLongTime;
	unsigned short int inputTime;
	PLUSE_MODE mode;
	Input_TypeDef state;
	char keyCode;
	s8 event;
	u8 effective;
} Input_sut;

typedef struct _Output {
	unsigned short int OutputTime;
	char out;
} Output_sut;
		 				    
#endif
