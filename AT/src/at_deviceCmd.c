#include <stdlib.h>

#include "at.h"
#include "at_deviceCmd.h"
#include "at_version.h"
#include "types.h"
#include "config.h"

extern at_funcationType at_fun[];
extern u16 DebugLevel;

u8 send_Message(u8 task, u16 message, u16 target, u8 * pData, u16 len);


void at_testCmdPlu(uint8_t id)
{
	char temp[32];
	sprintf(temp, "%s:(0-9),(10-2500)\r\n", at_fun[id].at_cmdName);
	at_printf(temp);
	at_backOk;
}
void at_queryCmdPlu(uint8_t id)
{

}

void at_setupCmdPlu(uint8_t id, char *pPara)
{

}


void at_testCmdCoin(uint8_t id)
{
	char temp[32];
	sprintf(temp, "%s:(1-2),(1-200)\r\n", at_fun[id].at_cmdName);
	at_printf(temp);
	at_backOk;
}

void at_setupCmdCoin(uint8_t id, char *pPara)
{
	int off,data;
	sscanf(pPara,"=%u,%u",&off,&data);
	if(off==1 || off==2){
		at_backOk;
		send_Message(TASK_IO,MESSAGE_IN_COIN,off,(u8*)&data,2);
	}else{
		at_backError;
	}
}

void at_testCmdDebug(uint8_t id)
{
	char temp[32];
	sprintf(temp, "%s:(0-100)\r\n", at_fun[id].at_cmdName);
	at_printf(temp);
	at_backOk;
}
void at_queryCmdDebug(uint8_t id)
{
	char temp[32];
	sprintf(temp, "%s:%d\r\n", at_fun[id].at_cmdName,DebugLevel);
	at_printf(temp);
	at_backOk;
}

void at_setupCmdDebug(uint8_t id, char *pPara)
{
	int data;
	sscanf(pPara,"=%d",&data);
	if(data<=100){
		DebugLevel=data;
		at_backOk;
	}else{
		at_backError;
	}
}


