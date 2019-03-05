#include "font.h"
#include "../jpg/JPG.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int font_print_char(pLcdInfo_t plcdinfo, int x, int y, unsigned char ch)
{
	
	unsigned char reference_ch;
	unsigned char jpgpath[30] = {0};

	//数字
	if(ch >= '0' && ch <= ':')
	{
		reference_ch = '0';
		strcpy(jpgpath, "./image/desktop/num.jpg");
	}
	else if(ch >= 'a' && ch <= 'z')
	{
		reference_ch = 'a';
		strcpy(jpgpath, "./image/desktop/a_z.jpg");
	
	}
	else {
		printf("ch can't be found in font lib\n");
		return -1;
	}

	int ch_x = 50 * (ch - reference_ch);
	int ch_y = 0;

	JpgInfo_t char_jpginfo;

	select_decompress_jpg2buffer(&char_jpginfo, jpgpath, ch_x, ch_y, 50, 50);
	draw_pic(plcdinfo, x, y, &char_jpginfo);
	
	free(char_jpginfo.buff);

	return 0;

}

int font_print_string(pLcdInfo_t plcdinfo, int x, int y, unsigned char *str)
{
	if(plcdinfo == NULL)
	{
		printf("plcdinfo couldn't be NULL\n");
		return -1;
	}

	for(int i = 0; i < strlen(str); i++)
	{
		font_print_char(plcdinfo, x+50*i, y, str[i]);

	}

	return 0;





}



