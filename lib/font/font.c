#include "font.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>

#define CHINESE		1


typedef struct FonT{
	
	unsigned char str[4];
	unsigned char *array_16;
	unsigned char *array_32; 

}Font_t , *pFont_t;



Font_t 	FONTLIB[TOTAL];
int	LIB_SIZE;



unsigned char *search_font_array();
int Print();
int add_FONTLIB();

int main()
{
	
	//初始化
	bzero(&FONTLIB, sizeof(FONTLIB));
	LIB_SIZE = 0;

	add_FONTLIB("车", che_16x16, che_32x32);
	add_FONTLIB("位", wei_16x16, wei_32x32);
	
	add_FONTLIB("0", _0_8x16, _0_16x32);	
	add_FONTLIB("1", _1_8x16, _1_16x32);	
	add_FONTLIB("2", _2_8x16, _2_16x32);	
	add_FONTLIB("3", _3_8x16, _3_16x32);	
	add_FONTLIB("4", _4_8x16, _4_16x32);	
	add_FONTLIB("5", _5_8x16, _5_16x32);	
	add_FONTLIB("6", _6_8x16, _6_16x32);	
	add_FONTLIB("7", _7_8x16, _7_16x32);	
	add_FONTLIB("8", _8_8x16, _8_16x32);	
	add_FONTLIB("9", _9_8x16, _9_16x32);	
	
	add_FONTLIB("a", _a_8x16, _a_16x32);	
	add_FONTLIB("b", _b_8x16, _b_16x32);	
	add_FONTLIB("c", _c_8x16, _c_16x32);	
	add_FONTLIB("d", _d_8x16, _d_16x32);	
	add_FONTLIB("e", _e_8x16, _e_16x32);	
	add_FONTLIB("f", _f_8x16, _f_16x32);	
	add_FONTLIB("g", _g_8x16, _g_16x32);	
	add_FONTLIB("h", _h_8x16, _h_16x32);	
	add_FONTLIB("i", _i_8x16, _i_16x32);	
	add_FONTLIB("j", _j_8x16, _j_16x32);
	add_FONTLIB("k", _k_8x16, _k_16x32);	
	add_FONTLIB("l", _l_8x16, _l_16x32);	
	add_FONTLIB("m", _m_8x16, _m_16x32);	
	add_FONTLIB("n", _n_8x16, _n_16x32);	
	add_FONTLIB("o", _o_8x16, _o_16x32);
	add_FONTLIB("p", _p_8x16, _p_16x32);	
	add_FONTLIB("q", _q_8x16, _q_16x32);	
	add_FONTLIB("r", _r_8x16, _r_16x32);	
	add_FONTLIB("s", _s_8x16, _s_16x32);	
	add_FONTLIB("t", _t_8x16, _t_16x32);	
	add_FONTLIB("u", _u_8x16, _u_16x32);	
	add_FONTLIB("v", _v_8x16, _v_16x32);	
	add_FONTLIB("w", _w_8x16, _w_16x32);	
	add_FONTLIB("x", _x_8x16, _x_16x32);	
	add_FONTLIB("y", _y_8x16, _y_16x32);	
	add_FONTLIB("z", _z_8x16, _z_16x32);	
	
	pLcdInfo_t plcdinfo = (LcdInfo_t *)malloc(sizeof(LcdInfo_t));
	lcd_create("/dev/fb0", plcdinfo);

	print_string(plcdinfo, 0, 30, "车位01234567890abcdefghijklmnopqrstuvwxyz", 32);
	print_string(plcdinfo, 0, 100, "车位01234567890abcdefghijklmnopqrstuvwxyz", 16);
	


}


//
int print_string(pLcdInfo_t plcdinfo, int x, int y, unsigned char *str, int size)
{
	
	//遍历字符串，匹配字符 UTF-8中文字符占用三个字节
	unsigned char *font_array = NULL;
	for(int i = 0; i < strlen(str); )
	{
		if(str[i] > 127)
		{
			unsigned char chinese[4] = {0};
			strncpy(chinese, str + i, 3);
			printf("%s\n", chinese);
			i = i + 3;
			
			//寻找字库数据
			font_array = search_font_array(chinese, size);
			Print(plcdinfo, x, y, font_array, size, size, 0x00FF0000);
			
			//位置右移
			x = x + size;
		}
		else 
		{
			unsigned char english[2] = {0};
			english[0] = str[i];
			printf("%s\n", english);
			i++;
		
			//寻找字库数据
			font_array = search_font_array(english, size);
			Print(plcdinfo, x, y, font_array, size / 2, size, 0x00FF0000);
	
			//位置右移
			x = x + size / 2;
		}

	}
	





}

int add_FONTLIB(unsigned char *str, unsigned char *array_16, unsigned char *array_32)
{
	strcpy(FONTLIB[LIB_SIZE].str, str);
	FONTLIB[LIB_SIZE].array_16 = array_16;
	FONTLIB[LIB_SIZE].array_32 = array_32;
	LIB_SIZE++;
}




unsigned char *search_font_array(unsigned char *str, int size)
{
	
	for(int i = 0; i < LIB_SIZE; i++)
	{
		
		if(strcmp(FONTLIB[i].str, str) == 0)
		{
			if(size == 16)
			{
				return FONTLIB[i].array_16;
			
			}
			else if(size == 32)
			{
				return FONTLIB[i].array_32;
			}
		}
	}

	printf("find no this %s in fontlib\n", str);
}







//
int Print(pLcdInfo_t plcdinfo, int x, int y, unsigned char *font_array, int width, int height, 
	 unsigned int color)
{
	
	//保存起始地址	
	unsigned int *base = plcdinfo->base + x + plcdinfo->width*y;

	//计算一行用多少位数据表示,仅支持 1位数据 代表 8个像素点格式
	int rowsize = width / 8;

	//每一次读取一行
	for(int rows = 0; rows < height*rowsize; rows = rows + rowsize)
	{

		int judge = 0x80;
		
		for(int cols = 0; cols < rowsize; cols++)
		{
			for(int i = 0; i < 8; i++)
			{
				if((font_array[rows + cols] & (judge >> i)))
				{
					//printf("1");
					*(base + i) = color; 
				}
				//else printf("0");
			}

			//前进八个像素点
			base = base + 8;
		}
		//printf("\n");

		//换行	
		base = base + plcdinfo->width - 8 * rowsize;
	}

}

