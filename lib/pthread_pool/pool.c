#include "pool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


pPool_t pool_create(int pthread_num)
{
	int ret1, ret2;

	//创建线程池
	pPool_t ppool = (Pool_t *)malloc(sizeof(Pool_t));	
	
	//初始化互斥锁、条件变量
	ret1 = pthread_mutex_init(&(ppool->mutex), NULL);
	ret2 = pthread_cond_init(&(ppool->cond), NULL);
	if(ret1 < 0 || ret2 < 0)
	{
		perror("fail to init mutex or cond");
		return NULL;
	}


	//定义任务链表空表头
	ppool->ptask_head = (Task_t *)malloc(sizeof(Task_t));
	if(ppool->ptask_head == NULL)
	{
		perror("fail to malloc ptask_head");
		return NULL;
	}
	
	
	ppool->ptask_head->proccess = NULL;
	ppool->ptask_head->arg1 = NULL;
	ppool->ptask_head->arg2 = NULL;
	ppool->ptask_head->next = NULL;

	//定义线程池线程数目、任务链表任务数
	ppool->pthread_num = pthread_num;
	ppool->cur_task_num = 0;

	//线程池生存标志位
	ppool->shutdown_flag = false;
	
	//创建线程
	ppool->pthread_ids = (pthread_t *)malloc(sizeof(pthread_t) * pthread_num);

	for(int i = 0; i < pthread_num; i++)
	{
		pthread_create(ppool->pthread_ids + i, NULL, thread_routine, (void *)ppool);
		
	}
	
	return ppool;
}



/*
 *强制销毁线程池
 */
int pool_destroy(pPool_t ppool)
{
	int ret1, ret2;

	//置位线程池销毁标志位
	ppool->shutdown_flag = true;
	
	//唤醒全部的线程
	pthread_cond_broadcast(&(ppool->cond));
	
	//等待线程退出
	for(int i = 0; i < ppool->pthread_num; i++)
	{
		pthread_join(ppool->pthread_ids[i], NULL);

	}


	//销毁互斥锁、条件变量
	ret1 = pthread_mutex_destroy(&(ppool->mutex));
	ret2 = pthread_cond_destroy(&(ppool->cond));
	if(ret1 < 0 || ret2 < 0)
	{
		perror("fail to destroy mutex or cond");
		return -1;
	}

	//释放线程池资源
	//销毁任务链表
	pTask_t ptask = ppool->ptask_head->next;
	free(ppool->ptask_head);
	while(ptask != NULL)
	{
		ppool->ptask_head = ptask;
		ptask = ptask->next;

		//释放资源:路径、任务结构体
		free(ppool->ptask_head->arg1);
		free(ppool->ptask_head->arg2);
		free(ppool->ptask_head);
	}
	ppool->ptask_head = NULL;
	//销毁线程id组
	free(ppool->pthread_ids);
	//销毁线程池
	free(ppool);
	ppool = NULL;


}


int wait_task_finish(pPool_t ppool)
{
	
	while(ppool->cur_task_num != 0);

}


int pool_add_task(pPool_t ppool, int (*proccess)(unsigned char *, unsigned char *), unsigned char *src, unsigned char *dst)
{
	
	int ret;

	//创建新的任务节点
	pTask_t pnew_task = (Task_t *)malloc(sizeof(Task_t));
	if(pnew_task == NULL)
	{
		perror("fail to setup new task");
		return -1;
	}

	pnew_task->proccess = proccess;
	pnew_task->arg1 = src;
	pnew_task->arg2 = dst;
	//printf("\nadd arg: %s\t, %s\n", pnew_task->arg1, pnew_task->arg2);
	pnew_task->next = NULL;


	//将新的任务节点加入任务链表中
	/*----------------上锁---------------*/
	ret = pthread_mutex_lock(&(ppool->mutex));
	if(ret < 0)
	{
		perror("fail to lock mutex");
		return -1;
	}

	pTask_t head = ppool->ptask_head;
	
	//寻找尾节点
	while(head->next != NULL)
	{
		head = head->next;
	}
	
	head->next = pnew_task;
	head = pnew_task;
	head->next = NULL;
	
	ppool->cur_task_num++;

	ret = pthread_mutex_unlock(&(ppool->mutex));
	if(ret < 0)
	{
		perror("fail to unlock mutex");
		return -1;
	}
	/*----------------解锁---------------*/
	//唤醒
	pthread_cond_signal(&(ppool->cond));
	
	return 0;

}




void *thread_routine(void *arg)
{
	int ret;	


	pPool_t ppool = (Pool_t *)arg;

	while(1)
	{
	
		/*----------------解锁---------------*/
		pthread_mutex_lock(&(ppool->mutex));
		if(ret < 0)
		{
			perror("fail to unlock mutex");
			pthread_exit(NULL);
		}

		//线程被唤醒若任务数为0, 线程池尚未销毁则继续等待
		while(ppool->cur_task_num == 0 && ppool->shutdown_flag == false)
		{

			pthread_cond_wait(&(ppool->cond), &(ppool->mutex));
			if(ret < 0)
			{
				perror("fail to unlock mutex");
				pthread_exit(NULL);
			}
		}

		//如果线程池被销毁
		if(ppool->shutdown_flag == true)
		{
			/*----------------解锁---------------*/
			pthread_mutex_unlock(&(ppool->mutex));
			if(ret < 0)
			{
				perror("fail to unlock mutex");
				pthread_exit(NULL);
			}

			pthread_exit(NULL);

		}

		//从任务链表中读取任务

		pTask_t ptask = ppool->ptask_head->next;
		ppool->ptask_head->next = ppool->ptask_head->next->next;
		ppool->cur_task_num--;

		pthread_mutex_unlock(&(ppool->mutex));
		if(ret < 0)
		{
			perror("fail to unlock mutex");
			pthread_exit(NULL);
		}
		/*----------------解锁---------------*/
		
		//调用任务函数
		//printf("i am %d, my args are :%s\t%s\n", (int)pthread_self(), ptask->arg1, ptask->arg2);
		(*ptask->proccess)(ptask->arg1, ptask->arg2);
		
		//释放当前任务节点资源
		
		free(ptask->arg1);
		free(ptask->arg2);
		free(ptask);
		ptask = NULL;
	
	}


}

void travel(pTask_t head)
{

	printf("task: head");
	head = head->next;
	while(head != NULL)
	{
		printf("->%s", head->arg1);
		head = head->next;
	
	}
	printf("\n");
}




