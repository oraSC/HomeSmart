#include "LCD.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>




pLcdInfo_t lcd_create(const char *path, pLcdInfo_t plcdinfo)
{

	//打开设备文件
	plcdinfo->fd = open(path, O_RDWR);
	if(plcdinfo->fd < 0)
	{
		perror("fail to open dev");
		goto error;
	}
	//printf("%d\n", plcdinfo->fd);	
	//获取lcd信息
	
	struct fb_var_screeninfo info;
	struct fb_var_screeninfo *pinfo = &info;
		
	int ret = ioctl(plcdinfo->fd, FBIOGET_VSCREENINFO, pinfo);
	if(ret < 0)
	{
		perror("ioctl fail");
		goto error;
	}
	
	plcdinfo->width  = pinfo->xres;
	plcdinfo->height = pinfo->yres;
	plcdinfo->size   = plcdinfo->width * plcdinfo->height * pinfo->bits_per_pixel/8;
	printf("size:%d\n", plcdinfo->size);	
	//将lcd内核空间进行 mmap 映射
	plcdinfo->base = mmap(NULL, plcdinfo->size, PROT_READ | PROT_WRITE, MAP_SHARED, plcdinfo->fd, 0);
	if(plcdinfo->base == (void *)-1)
	{
		perror("fail to mmap ");
		goto error;
	}

	//返回结构体
	return plcdinfo;
error:	
	
	//lcd_destroy(plcdinfo);
	return NULL;

}

bool draw_rect(pLcdInfo_t plcdinfo, int x, int y,pRect_t prect) 
{
	//错误处理
	if(plcdinfo == NULL)
	{
		perror("plcdinfo is NULL");
		return false;
	}
	unsigned int *base = plcdinfo->base + y * plcdinfo->width + x;
	int LW = prect->linewidth;

	for(int rows = 0; rows < prect->height; rows++)
	{
		for(int cols = 0; cols < prect->width; cols++)
		{
			if(LW == -1 || rows < LW || cols < LW || prect->width - rows <= LW || prect->height - cols <= LW)
			{
				*(base + cols) = prect->color;
			}
		}
		base += plcdinfo->width;
	
	}
	return true;



}



bool draw_pic(pLcdInfo_t plcdinfo, int x, int y, pJpgInfo_t pjpginfo)
{
	unsigned int *base = plcdinfo->base + y * plcdinfo->width + x;
	
	//画图片
	
	
	int min_W = (plcdinfo->width  - x) < pjpginfo->width  ? plcdinfo->width -  x : pjpginfo->width;
	int min_H = (plcdinfo->height - y) < pjpginfo->height ? plcdinfo->height - y : pjpginfo->height;
	//printf("minW:%d, minH:%d",min_W,min_H);
	for(int rows = 0; rows < min_H - 1; rows++)
	{
		
		for(int cols = 0; cols < min_W - 1; cols++)
		{
		
			memcpy(base + cols, pjpginfo->buff + rows * pjpginfo->rowsize  + cols * 3, 3 );
			//printf("%d\n",cols);	
		}
		//printf("base add%d\n", plcdinfo->width);	
		base += plcdinfo->width ;
	
	
	
	}

	/*
	 *backlog:返回值未完善
	 */

}

pBtnInfo_t draw_btn(pLcdInfo_t plcdinfo, int x, int y, pJpgInfo_t pjpginfo)
{
	pBtnInfo_t pbtninfo = (BtnInfo_t *)malloc(sizeof(BtnInfo_t));
	if(pbtninfo == NULL)
	{
		perror("fail to malloc memory for pbtninfo");
		return NULL;
	
	}
	//初始化 pbtninfo
	pbtninfo->X      = x;
	pbtninfo->Y      = y;
	pbtninfo->width  = pjpginfo->width;
	pbtninfo->height = pjpginfo->height;
	pbtninfo->next	 = NULL;

	draw_pic(plcdinfo, x, y, pjpginfo);
	
	return pbtninfo;
}

bool if_btnclick(pBtnInfo_t pbtninfo, int x, int y)
{
	if(x >= pbtninfo->X && x <= pbtninfo->X + pbtninfo->width && y >= pbtninfo->Y && y <= pbtninfo->Y + pbtninfo->height)
	{
		return true;
	
	}

	else
	{
		return false;
	}


}

pBtn_SqList_t create_btn_sqlist()
{
	pBtn_SqList_t head = (Btn_SqList_t *)malloc(sizeof(Btn_SqList_t));

	if(head == NULL)
	{
		perror("fail to create btn sqlist");
		return NULL;
	}

	//初始化头
	head->X		= 0;	
	head->Y		= 0;
	head->width	= 0;
	head->height	= 0;
	head->next	= NULL;

}

bool clear_btn_sqlist(pBtn_SqList_t *head)
{
	if(!head || !(*head))
	{
		perror("head or *head is NULL");
		return false;
	}
	
	//跳过头
	pBtn_SqList_t node1, node2;
	node1 = (*head)->next;
	
	while(node1 != NULL)
	{
		node2 = node1->next;
		free(node1);
		node1 = node2;
		
	}

	(*head)->next = NULL;
	return true;


}





bool destroy_btn_sqlist(pBtn_SqList_t *head)
{
	if(!head || !(*head))
	{
		perror("fail to destroy btn sqlist");
		return false;
	}

	//先清空链表
	clear_btn_sqlist(head);

	free(*head);

	*head = NULL;

	return true;

}
bool AddFromTail_btn_sqlist(pBtn_SqList_t head, pBtn_SqList_t node)
{
	if(!head && !node)
	{
		perror("fail to addfromtail btn sqlist");
		return false;
	}

	//找到尾节点
	while(head->next != NULL)
	{
		head = head->next;
	}
	//尾插入
	head->next = node;
	head = node;
	head->next = NULL;
	return true; 
}


void ergodic_btn_sqlist(pBtn_SqList_t head)
{
	if(!head || !head->next)
	{
		perror("head or head->next is NULL");
		return;
	}


	//跳过头
	head = head->next;
	while(head)
	{
		printf("%d\n", head->X);
		head = head->next;
	}


}

int find_which_btn_click(pBtn_SqList_t head, int x, int y)
{
	//跳过头
	head = head->next;

	int i = 1;
	while(head != NULL)
	{
		if(if_btnclick(head, x, y))
		{
			break;
		}
		head = head->next;
		i++;
	}
	//未找到
	if(head == NULL)
	{
		return 0;
	
	}
	
	return i;
}














