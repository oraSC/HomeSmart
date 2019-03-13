#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>

#include "./lib/lcd/LCD.h"
#include "./lib/jpg/JPG.h"
#include <linux/input.h>
#include "./app/album.h"
#include "./lib/socket/mysocket.h"
#include "./app/music.h"
#include "./app/garage.h"
#include "./app/camera.h"
//#include "./lib/font/font.h"

#define FIND_MAX_FD(x,y) (x)>(y)?(x):(y) 
#define APP_NUM 	5



/*
struct point{
	
	int X;
	int Y;
	int lastX;
	int lastY;
	bool update;

}ts_point;
*/
struct point 	ts_point;
struct Command 	command;


void update_clients(char *message);
void *ts_monitor(void *arg);
void *remote_control(void *arg);

//声明待添加集合列表
int soc_fds[10];
int soc_fds_len;
int max_fd;

//线程互斥锁、条件变量
pthread_mutex_t mutex;
pthread_cond_t	cond;


//声明状态值(记录当前处于的功能)
char state[20];

int main()
{
	
	//初始化多路复用待添加集合列表
	bzero(soc_fds, sizeof(soc_fds));
	soc_fds_len = 0;
	max_fd = -1;

	//初始化状态值
	strcpy(state, "desktop");

	int ret;

	char app_icon_name[APP_NUM][30] = {	"./image/desktop/album.jpg", 
										"./image/desktop/music.jpg", 
										"./image/desktop/garage.jpg",
										"./image/desktop/camera.jpg", 
										"./image/desktop/exit.jpg"};

	//1.创建lcd
	pLcdInfo_t plcdinfo = (LcdInfo_t *)malloc(sizeof(LcdInfo_t));
	if(plcdinfo == NULL)
	{
		perror("fail to malloc lcd");
		return -1;
	}
	plcdinfo = lcd_create("/dev/fb0", plcdinfo);

	//2.打开背景图片
	pJpgInfo_t bg_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
	if(bg_pjpginfo == NULL)
	{
		perror("fail to malloc bg_pjpginfo");
		return -1;
	}
	decompress_jpg2buffer(bg_pjpginfo, "./image/desktop/bg.jpg");

	//3.加载背景
	draw_pic(plcdinfo, 0, 0, bg_pjpginfo);

	//*4.加载app图标
	//**创建按键链表头
	pBtn_SqList_t head = create_btn_sqlist();

	//**添加按键
	JpgInfo_t app_jpginfo[APP_NUM];
	for(int i = 0; i < APP_NUM; i++)
	{
		
		decompress_jpg2buffer(&app_jpginfo[i], app_icon_name[i]);
		
		pBtn_SqList_t newnode = draw_btn(plcdinfo, 150*(i) + 30, 100, &app_jpginfo[i]);
		AddFromTail_btn_sqlist(head, newnode);
		
	}
	
	//5.初始化互斥锁和条件变量、创建触摸屏监控和socket通讯子线程
	ret = pthread_mutex_init(&mutex, NULL);
	if(ret < 0)
	{
		perror("fail to init mutex");
		goto err;
	}
	
	ret = pthread_cond_init(&cond, NULL);
	if(ret < 0)
	{
		perror("fail to init cond");
		goto err;
	}

	pthread_t ts_pth_id;
	ret = pthread_create(&ts_pth_id, NULL, ts_monitor, NULL);
	if(ret < 0)
	{
		perror("fail to create ts_monitor pthread");
		goto err;
	}
	
	pthread_t soc_pth_id;
	ret = pthread_create(&soc_pth_id, NULL, remote_control, NULL);
	if(ret < 0)
	{
		perror("fail to create remote control pthread");
		goto err;
	}

	//*6.读取按键
	int app_num = 0;

	while(1)
	{
		
		/************************上锁**************************************/
		pthread_mutex_lock(&mutex);
		
		while(ts_point.update != true && command.update != true)
		{
			//挂起等待
			pthread_cond_wait(&cond, &mutex);
		}
		
		//检测到用户动作
		if(ts_point.update == true)
		{
			ts_point.update = false;
			app_num = find_which_btn_click(head, ts_point.X, ts_point.Y);
		}
		else if(command.update == true)
		{
			command.update = false;
			app_num = command.ascii[0] - '0';
		}
		
		pthread_mutex_unlock(&mutex);
		/***************************解锁*************************************/
		if(app_num != 0)
		{
			if(app_num == 5)
			{
				break;

			}
			else if(app_num == 4)
			{
				camera(plcdinfo, &ts_point, &command);
				
			
			}

			//车库	
			else if(app_num == 3)
			{
				strcpy(state, "garage");
				garage(plcdinfo, &ts_point);
				strcpy(state, "desktop");
			}

			else if(app_num == 1)
			{
				update_clients("album");	
				album(plcdinfo, &ts_point, &command);
				update_clients("desktop");
			}
			else if(app_num == 2)
			{
				update_clients("music");	
				music(plcdinfo, &ts_point, &command);
				update_clients("desktop");
			}
			
			
			//再次刷新桌面
			draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
			for(int i = 0; i < APP_NUM; i++)
			{
				draw_pic(plcdinfo, i*150 + 100, 100, &app_jpginfo[i]);
			
			}
						
		}
	
	}


	//销毁lcd
	ret = lcd_destroy(plcdinfo);

	//释放背景图片资源
	free(bg_pjpginfo->buff);
	free(bg_pjpginfo);
	for(int i = 0; i < APP_NUM; i++)
	{
		free(app_jpginfo[i].buff);
	}
	//清空链表
	clear_btn_sqlist(&head);	

	//销毁链表
	destroy_btn_sqlist(&head);
	
	printf("HomeSmart desktop exit\n");
	return 0;
err:
	lcd_destroy(plcdinfo);
	free(bg_pjpginfo->buff);
	free(bg_pjpginfo);
	for(int i = 0; i < APP_NUM; i++)
	{
		free(app_jpginfo[i].buff);
	}
	
	//关闭文件描述符中的soc_fd
	for(int i = 0; i < soc_fds_len; i++)
	{
		shutdown(soc_fds[i], SHUT_RDWR);
	
	}
	soc_fds_len = 0;

	clear_btn_sqlist(&head);
	destroy_btn_sqlist(&head);
	printf("HomeSmart desktop exit because of error\n");
	return -1;

}

void *ts_monitor(void *arg)
{
	int ret;

	//线程分离
	pthread_detach(pthread_self());

	//触摸屏
	struct input_event tsinfo;
	int ts_fd = open("/dev/input/event0", O_RDWR);
	if(ts_fd < 0)
	{
		perror("fail to open touch screen");
		pthread_exit(NULL);
	}

	while(1)
	{
		
		ret = read(ts_fd, &tsinfo, sizeof(struct input_event));
		if(ret < 0)
		{
			perror("error exits in read tsinfo");
			pthread_exit(NULL);
		
		}
		/************************上锁********************************/
		pthread_mutex_lock(&mutex);

		//X
		if(tsinfo.type == EV_ABS && tsinfo.code == ABS_X)
		{
			printf("x = %d\t", tsinfo.value);
			ts_point.X = tsinfo.value;
		}

		//Y
		if(tsinfo.type == EV_ABS && tsinfo.code == ABS_Y)
		{
			printf("y = %d\n", tsinfo.value);
			ts_point.Y = tsinfo.value;
		}
		if(ts_point.lastX != ts_point.X && ts_point.lastY != ts_point.Y)
		{
			//坐标更新
			ts_point.lastX= ts_point.X;
			ts_point.lastY = ts_point.Y;
			ts_point.update = true;
			pthread_cond_signal(&cond);
		}
		
		pthread_mutex_unlock(&mutex);
		/************************解锁*********************************/
	}

}

void *remote_control(void *arg)
{

	int ret;

	//线程分离
	/*
	 *bug:许多soc文件描述符未正常关闭
	 *
	 */
	pthread_detach(pthread_self());

	//创建套接字
	int soc_fd;
	soc_server_init(&soc_fd, NULL, 3000);


	//等待对端连接请求
	//1.声明变量存储对端信息
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	bzero(&client_addr, client_addr_len);
	
	//将监听套接字(soc_fd)加入待待添加集合列表(应该用链表代替)
	soc_fds[soc_fds_len] = soc_fd;
	max_fd = FIND_MAX_FD(soc_fd, max_fd);
	soc_fds_len++;

	char buff[100];

	fd_set fdset;

	//发收信息
	while(1)
	{
		bzero(buff, sizeof(buff));

		//配置多路复用描述符集
		FD_ZERO(&fdset);

		//将待添加集合列表添加
		for(int i = 0; i < soc_fds_len; i++)
		{
			FD_SET(soc_fds[i], &fdset);

		}
		
		//多路复用
		ret = select(max_fd+1, &fdset, NULL, NULL, NULL);
		if(ret < 0)
		{
			perror("error exits in select");
			//return -1;
		}
		else if(ret == 0)
		{
			printf("timeout\n");
			continue;
		}
		
		
		/**********************上锁********************************/
		/*
		pthread_mutex_lock(&mutex);

		//判断当前状态是否为garage
		while(strcmp(state, "garage") == 0)
		{
			//挂起
			printf("into garage, remote control sleep\n");
			pthread_cond_wait(&cond, &mutex);
			printf("outof garage, remote control wakeup\n");
			
			//清空多路复用描述集
			FD_ZERO(&fdset);
		}
		
		pthread_mutex_unlock(&mutex);
		*/
		/***********************解锁********************************/
		

		//有新的客户端连接请求
		if(FD_ISSET(soc_fd, &fdset))
		{
		
			int acc_fd = accept(soc_fd, (struct sockaddr *)&client_addr, &client_addr_len);
			if(acc_fd < 0)
			{
				perror("error exits when accept client connect");
				//return -1;
			}
			
			printf("/**********************desktop server**********************/\n");
			printf("connecting with client.\nip: %s, port: %hd\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			//发送当前状态值
			ret = send(acc_fd, state, strlen(state), 0);
			if(ret < 0)
			{
				perror("error exits in send state when accept");
			
			}

			//更新待添加集合列表
			soc_fds[soc_fds_len] = acc_fd;
			max_fd = FIND_MAX_FD(acc_fd, max_fd);
			soc_fds_len++;

		}
		//从客户端接受消息
		else
		{
			for(int j = 1; j < soc_fds_len; j++)
			{
				if(FD_ISSET(soc_fds[j], &fdset))
				{
					/*******************上锁*********************/
					pthread_mutex_lock(&mutex);

					bzero(command.ascii, sizeof(command.ascii));
		
					ret = recv(soc_fds[j], &command.ascii, sizeof(command.ascii), 0);
					if(ret < 0)
					{
						perror("error exits in recv");
						shutdown(soc_fds[j], SHUT_RDWR);
						//在集合中删除
						soc_fds[j] = soc_fds[soc_fds_len - 1];
						soc_fds_len--;
						
					}
					else if(ret == 0)
					{
						printf("a client offline\n");
						shutdown(soc_fds[j], SHUT_RDWR);
						//在集合中删去
						soc_fds[j] = soc_fds[soc_fds_len - 1];
						soc_fds_len--;
					}
					else 
					{
						printf("command:%s", command.ascii);
						command.update = true;
						
						//发送信号给条件变量
						pthread_cond_signal(&cond);
					}

					pthread_mutex_unlock(&mutex);
					/*******************解锁************************/
				}
			}
		}
	}
}


void update_clients(char *message)
{
	
	int ret;
	
	//更新状态值
	bzero(state, sizeof(state));
	strcpy(state, message);
	
	for(int i = 1; i < soc_fds_len; i++)
	{
		ret = send(soc_fds[i], message, strlen(message), 0);
		printf("send %s successfully\n", message);
		if(ret < 0)
		{
			perror("fail to update client");
		
		}
	
	}

}



