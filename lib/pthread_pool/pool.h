#ifndef _POOL_H_
#define _POOL_H_

#include <pthread.h>
#include <stdbool.h>

typedef struct task {
	
	int  (*proccess)(unsigned char *, unsigned char *);   	//函数指针
	unsigned char  *arg1;			//函数参数
	unsigned char  *arg2;
	struct task *next;		

}Task_t, *pTask_t;			//任务链表结构体

typedef struct{

	pthread_mutex_t mutex;		//线程锁
	pthread_cond_t 	cond;		//条件变量
	pTask_t 	ptask_head;	//任务链表头
	int 		pthread_num;	//线程池线程数
	pthread_t	*pthread_ids;	//线程id指针，指向第一个线程id地址
	int		cur_task_num;	//任务数
	bool		shutdown_flag;	//线程池生存标志位


}Pool_t, *pPool_t;			//线程池结构体


pPool_t pool_create(int pthread_num);

//函数原型，可在此基础上更改功能
int 	pool_add_task(pPool_t ppool, int (*proccess)(unsigned char *, unsigned char *), unsigned char *src, unsigned char *dst);
void 	*thread_routine(void *arg);
int	wait_task_finish(pPool_t ppool);
int	pool_destroy(pPool_t ppool);
void travel(pTask_t head);


#endif

