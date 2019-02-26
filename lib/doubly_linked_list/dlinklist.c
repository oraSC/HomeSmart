#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "dlinklist.h"

int main()
{
	pDlinkList_t phead = create_head();

	pDlinkList_t node1 = (DlinkList_t *)malloc(sizeof(DlinkList_t));
	bzero(node1, sizeof(node1));
	node1->data = 1;
	add_nodeintohead(phead, node1);

	pDlinkList_t node2 = (DlinkList_t *)malloc(sizeof(DlinkList_t));
	bzero(node2, sizeof(node2));
	node2->data = 2;
	add_nodeintohead(phead, node2);

	pDlinkList_t node3 = (DlinkList_t *)malloc(sizeof(DlinkList_t));
	bzero(node3, sizeof(node3));
	node3->data = 3;
	add_nodeintohead(phead, node3);

	pDlinkList_t node4 = (DlinkList_t *)malloc(sizeof(DlinkList_t));
	bzero(node4, sizeof(node4));
	node4->data = 4;
	add_nodeintohead(phead, node4);

	pDlinkList_t node5 = (DlinkList_t *)malloc(sizeof(DlinkList_t));
	bzero(node5, sizeof(node5));
	node5->data = 5;
	add_nodeintotail(phead, node5);

	printList(phead);
	
	clearList(phead);

	printList(phead);

	destroyList(&phead);
	

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
	phead->data = 0;
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
		printf("%d->", p->data);
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






