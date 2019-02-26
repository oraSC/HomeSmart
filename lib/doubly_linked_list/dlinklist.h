#ifndef _DLINKLIST_H_
#define _DLINKLIST_H_


typedef int elemType_t;


typedef struct Dlinklist{
	
	struct Dlinklist *prev;
	elemType_t data;
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

#endif
