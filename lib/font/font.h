#ifndef _FONT_H_
#define _FONT_H_

#include "../lcd/LCD.h"
#define  TOTAL 	38


int print_string(pLcdInfo_t plcdinfo, int x, int y, unsigned char *str, int size);
int font_lib_init();




#endif
