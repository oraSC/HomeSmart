#ifndef _ALBUM_H_
#define _ALBUM_H_

#include "../lib/lcd/LCD.h"
#include "../lib/jpg/JPG.h"




struct point{

	int X;
	int Y;
	int lastX;
	int lastY;
	bool update;

};
struct Command{

	bool update;
	unsigned char ascii[10];

};



typedef struct Dlinklist{
	
	struct Dlinklist *prev;
	
	int		index;
	pJpgInfo_t	src_pjpginfo;
	pJpgInfo_t	album_pjpginfo;
	pJpgInfo_t	overview_pjpginfo;

	struct Dlinklist *next;

}DlinkList_t, *pDlinkList_t;

//创建双向链表返回头
pDlinkList_t 	create_head();

//*插入节点
//**头插法
int 		add_nodeintohead(pDlinkList_t phead, pDlinkList_t node);
//**尾插法
int		add_nodeintotail(pDlinkList_t phead, pDlinkList_t node);
//遍历链表
int		printList(pDlinkList_t phead);
//清空链表
int		clearList(pDlinkList_t phead);
//删除链表
int		destroyList(pDlinkList_t *pphead);



int album(pLcdInfo_t plcdinfo, struct point *pts_point, struct Command *pcommand);
int calculate_resize(int src_width, int src_height, int *dst_width, int *dst_height, int limit_width, int limit_height);
int update_album(pLcdInfo_t plcdinfo, pDlinkList_t head, pDlinkList_t now_pic);

#endif
