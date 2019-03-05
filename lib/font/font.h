#ifndef _FONT_H_
#define _FONT_H_

#include "../lcd/LCD.h"

//初始化字库
int font_print_char(pLcdInfo_t pcldinfo, int x, int y, unsigned char ch);
int font_print_string();


#endif
