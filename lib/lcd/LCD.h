#ifndef _LCD_H_
#define _LCD_H_
#include <stdbool.h>
#include "../jpg/JPG.h"

/*
@author:czw;


*/


typedef struct{
	int fd;		//lcd 文件描述符
	unsigned int *base;	//lcd mmap映射空间首地址
	int width;	//lcd 宽
	int height;	//lcd 高
	int size;	//lcd 大小

}LcdInfo_t, *pLcdInfo_t;


typedef struct BtnInfo{
	int X;
	int Y;
	int width;
	int height;
	struct BtnInfo *next;

}BtnInfo_t, *pBtnInfo_t; 

typedef struct{
	int 		width;
	int 		height;
	unsigned int 	color;
	int 		linewidth;//线宽，单位为px,定义为-1时填充整个矩形

}Rect_t, *pRect_t;

pLcdInfo_t 	lcd_create(const char *path, pLcdInfo_t plcdinfo);
bool     	lcd_destroy(pLcdInfo_t plcdinfo);
bool		draw_rect(pLcdInfo_t plcdinfo, int x, int y, pRect_t rect);
bool		draw_pic(pLcdInfo_t plcdinfo, int x, int y, pJpgInfo_t pjpginfo);
pBtnInfo_t	draw_btn(pLcdInfo_t plcdinfo, int x, int y, pJpgInfo_t pjpginfo);
bool		if_btnclick(pBtnInfo_t pbtninfo, int x, int y);

// btn链表 
typedef BtnInfo_t	Btn_SqList_t; 
typedef pBtnInfo_t	pBtn_SqList_t;

pBtn_SqList_t	create_btn_sqlist();
bool		clear_btn_sqlist(pBtn_SqList_t *head);
bool		destroy_btn_sqlist(pBtn_SqList_t *head);
bool		AddFromTail_btn_sqlist(pBtn_SqList_t head, pBtn_SqList_t node);
bool		DelFromTail_btn_sqlist();
void		ergodic_btn_sqlist(pBtn_SqList_t head);
int		find_which_btn_click(pBtn_SqList_t head, int x, int y);


#endif
