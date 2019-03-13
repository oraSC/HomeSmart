#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "../lib/lcd/LCD.h"
#include "./album.h"

/*
*功能：打开相机
*返回值：
*   成功：0
*   失败：-1
*/
int camera(pLcdInfo_t plcdinfo, struct point *pts_point, struct Command *pcommand);


#endif