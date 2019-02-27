#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "../lib/lcd/LCD.h"
#include "../lib/jpg/JPG.h"
#include "./album.h"
#include <linux/input.h>

#define TOTAL_PIC 7


int album(pLcdInfo_t plcdinfo, struct point *pts_point, struct Command *pcommand)
{
	int ret;

	char btn_name[3][30] = {"./image/album/exit.jpg", "./image/album/last.jpg", "./image/album/next.jpg"};

	//2.打开背景图片
	pJpgInfo_t bg_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
	if(bg_pjpginfo == NULL)
	{
		perror("fail to malloc bg_pjpginfo");
		return -1;
	}
	decompress_jpg2buffer(bg_pjpginfo, "./image/album/bg.jpg");

	//3.加载背景
	draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
	
	
	//overview bg		
	JpgInfo_t overviewBg_jpginfo;

	decompress_jpg2buffer(&overviewBg_jpginfo, "./image/album/overview_bg.jpg");
	draw_pic(plcdinfo, 600, 50, &overviewBg_jpginfo);


	//*4.加载按键
	//**创建链表头
	pBtn_SqList_t head = create_btn_sqlist();
	
	//**添加按键
	JpgInfo_t exit_jpginfo;
	JpgInfo_t last_jpginfo;
	JpgInfo_t next_jpginfo;
		
	//***exit按键
	decompress_jpg2buffer(&exit_jpginfo, btn_name[0]);
	pBtn_SqList_t exit_node = draw_btn(plcdinfo, 600, 0, &exit_jpginfo);
	AddFromTail_btn_sqlist(head, exit_node);


	//***last按键
	decompress_jpg2buffer(&last_jpginfo, btn_name[1]);
	pBtn_SqList_t last_node = draw_btn_onlyAcolor(plcdinfo, 600, 50, &last_jpginfo, 0x00000000);
	AddFromTail_btn_sqlist(head, last_node);
	

	//***next按键
	decompress_jpg2buffer(&next_jpginfo, btn_name[2]);
	pBtn_SqList_t next_node = draw_btn_onlyAcolor(plcdinfo, 600, 380, &next_jpginfo, 0x00000000);
	AddFromTail_btn_sqlist(head, next_node);
	
	
	//定义相册双向链表
	pDlinkList_t pic_phead = create_head();

	//*读取原图
	char src_jpg_name[TOTAL_PIC][30] = {"./image/album/pic1.jpg", "./image/album/pic2.jpg","./image/album/pic3.jpg","./image/album/pic4.jpg","./image/album/pic5.jpg", "./image/album/pic6.jpg", "./image/album/pic7.jpg"};

	for(int i = 0; i < TOTAL_PIC; i++)
	{
		//***分配空间
		pDlinkList_t new_node 		= (DlinkList_t *)malloc(sizeof(DlinkList_t));
		pJpgInfo_t src_pjpginfo 		= (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
		pJpgInfo_t album_pjpginfo		= (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
		pJpgInfo_t overview_pjpginfo		= (JpgInfo_t *)malloc(sizeof(JpgInfo_t));

		//***解压
		decompress_jpg2buffer(src_pjpginfo, src_jpg_name[i]);
		
		//***resize
		int src_width 	= src_pjpginfo->width;
		int src_height 	= src_pjpginfo->height;
		int dst_width = 0, dst_height = 0;
		
		//****for album
		calculate_resize(src_width, src_height, &dst_width, &dst_height, 600, 480);
		jpg_resize(src_pjpginfo, album_pjpginfo, dst_width, dst_height);
		
		//****for overview
		calculate_resize(src_width, src_height, &dst_width, &dst_height, 200, 100);
		jpg_resize(src_pjpginfo, overview_pjpginfo, dst_width, dst_height);

		//将数据添加进节点中
		new_node->prev 			= NULL;
		new_node->src_pjpginfo 		= src_pjpginfo;	 
		new_node->album_pjpginfo 	= album_pjpginfo;
		new_node->overview_pjpginfo	= overview_pjpginfo;
		new_node->index = i; 
		
		/*sleep(1);
		draw_pic(plcdinfo, 0, 0, new_node->src_pjpginfo);
		
		sleep(1);
		draw_pic(plcdinfo, 0, 0, new_node->album_pjpginfo);

		sleep(1);
		draw_pic(plcdinfo, 0, 0, new_node->overview_pjpginfo);
		*/

		//添加节点
		add_nodeintohead(pic_phead, new_node);
	}	

	//相册加载(默认展示第三张)
	pDlinkList_t now_pic = pic_phead->next->next;

	//更新相册
	update_album(plcdinfo, pic_phead, now_pic);
	draw_pic_onlyAcolor(plcdinfo, 600, 50, &last_jpginfo, 0x00000000);
	draw_pic_onlyAcolor(plcdinfo, 600, 380, &next_jpginfo, 0x00000000);
	
	int opt;
	while(1)
	{
		if(pts_point->update == true || pcommand->update == true)
		{
			if(pts_point->update == true)
			{
				opt = find_which_btn_click(head, pts_point->X, pts_point->Y);
			}
			else if(pcommand->update == true)	
			{
				opt = pcommand->ascii[0] - '0';
			
			}
			if(opt > 0)
			{	
				if(opt == 1)
				{
					return 0;
				
				}
				else if(opt == 2)
				{
					now_pic = now_pic->prev;
					if(now_pic == pic_phead)
					{
						now_pic = pic_phead->prev;
					}
				
				}	
				else if(opt == 3)
				{
					now_pic = now_pic->next;
					if(now_pic == pic_phead)
					{
						now_pic = pic_phead->next;
					}
					
				}
				
				//bg
				draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
				draw_pic(plcdinfo, 600, 50, &overviewBg_jpginfo);
				//刷新相册
				update_album(plcdinfo, pic_phead, now_pic);
				
				draw_pic_onlyAcolor(plcdinfo, 600, 50, &last_jpginfo, 0x00000000);
				draw_pic_onlyAcolor(plcdinfo, 600, 380, &next_jpginfo, 0x00000000);
		
			}
		
			pts_point->update = false;
			pcommand->update  = false;	
		}
	}
		
	




	//释放图片资源
	free(bg_pjpginfo->buff);
	free(bg_pjpginfo);
	free(overviewBg_jpginfo.buff);
	free(last_jpginfo.buff);
	free(next_jpginfo.buff);

	//销毁链表
	destroy_btn_sqlist(&head);
	destroyList(&pic_phead);

	printf("album exits\n");
	return 0;

}




int update_album(pLcdInfo_t plcdinfo, pDlinkList_t head, pDlinkList_t now_pic)
{
	
	middle_show(plcdinfo, 0, 0, 600, 480, now_pic->album_pjpginfo);	
	pDlinkList_t plast = now_pic->prev;
	pDlinkList_t pnext = now_pic->next;
	
	//overview
	if(plast == head)
	{
		plast = head->prev;
	
	}
	if(pnext == head)
	{
		pnext = head->next;
	}
	middle_show(plcdinfo, 600, 50, 200, 100, plast->overview_pjpginfo);
	middle_show(plcdinfo, 600, 215, 200, 100, now_pic->overview_pjpginfo);
	middle_show(plcdinfo, 600, 380, 200, 100, pnext->overview_pjpginfo);

}





int calculate_resize(int src_width, int src_height, int *dst_width, int *dst_height, int limit_width, int limit_height)
{
	int ratio = 0 , ratio_w, ratio_h = 0;
		
	//长或宽大于
	if(src_width > limit_width || src_height > limit_height)
	{
		ratio_w = 100 * src_width / limit_width;
		ratio_h = 100 * src_height / limit_height;
		//取大者
		ratio = ratio_w > ratio_h?ratio_w:ratio_h;
		
		*dst_width = src_width * 100 / ratio;
		*dst_height = src_height * 100 / ratio;

	}
	
	//长宽均小于
	else if(src_width <= limit_width && src_height <= limit_height)
	{
		ratio_w = 100*limit_width / src_width;
		ratio_h = 100*limit_height / src_height;

		ratio = ratio_w < ratio_h?ratio_w:ratio_h;
		
		*dst_width = ratio * src_width / 100;
		*dst_height = ratio * src_height / 100;
	}
	return 0;

}

pDlinkList_t create_head()
{
	int ret;

	pDlinkList_t phead = (DlinkList_t *)malloc(sizeof(DlinkList_t));
	if(phead == NULL)
	{
		perror("fail to malloc in create head");
		return NULL;
	}

	//初始化
	phead->prev = phead;
	phead->index = 0;
	phead->src_pjpginfo 	 = NULL;
	phead->album_pjpginfo 	 = NULL;
	phead->overview_pjpginfo = NULL;
	phead->next = phead;

	return phead;
}

int add_nodeintohead(pDlinkList_t phead, pDlinkList_t node)
{
	if(phead == NULL || node == NULL)
	{
		printf("phead and node can't be NULL in add nodeintohead\n");
		return -1;
	}

	//插入操作
	node->next  = phead->next;
	node->prev = phead; 
	phead->next->prev = node;
	phead->next = node;
	
	return 0;

}


int add_nodeintotail(pDlinkList_t phead, pDlinkList_t node)
{
	if(phead == NULL || node == NULL)
	{
		printf("phead and node can't be NULLin add nodeintotail\n");
		return -1;
	}

	//插入操作
	node->prev  = phead->prev;
	node->next = phead; 
	phead->prev->next = node;
	phead->prev = node;
	
	return 0;

}


int printList(pDlinkList_t phead)
{
	if(phead == NULL)
	{
		printf("phead can't be NULL in printList\n");
		return -1;
	}
	
	//跳过头
	pDlinkList_t p = phead->next;

	printf("head->");
	while(p != phead)
	{
		printf("%d->", p->index);
		p = p->next;
	}
	printf("head\n");

	return 0;
}

int clearList(pDlinkList_t phead)
{
	if(phead == NULL)
	{
		printf("phead can't be NULL in clearList\n");
		return -1;
	}
	//跳过头
	pDlinkList_t p = phead->next;
	

	while(p != phead)
	{
		pDlinkList_t next = p->next;
		p->next = NULL;
		p->prev = NULL;
		free(p->src_pjpginfo->buff);
		free(p->album_pjpginfo->buff);
		free(p->overview_pjpginfo->buff);
		free(p);
		p = next;
	}

	//改变头
	phead->next = phead;
	phead->prev = phead;
	
	return 0;

}

int destroyList(pDlinkList_t *pphead)
{
	if(pphead == NULL || *pphead == NULL)
	{
		printf("pphead and phead can't be NULL in destroyList\n");
		return -1;
	}

	//清空链表
	clearList(*pphead);

	free(*pphead);

	pphead = NULL;

}






