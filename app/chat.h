#ifndef _CHAT_H_
#define _CHAT_H_

#include "../lib/lcd/LCD.h"
#include "album.h"

/*
*功能：调用 chat 功能
*返回值：
*	成功：0
*	失败：-1
*/
int chat(pLcdInfo_t plcdinfo, pPoint_t pts_point);


#endif