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

#include "./lib/lcd/LCD.h"
#include "./lib/jpg/JPG.h"
#include <linux/input.h>
#include "./app/album.h"
#include "./lib/socket/mysocket.h"

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
int soc_fd;


void *ts_monitor(void *arg);
void *remote_control(void *arg);

int main()
{
	//先将soc_fd绑定到标准出错
	soc_fd = 3;
	
	int ret;

	char app_icon_name[3][30] = {"./image/desktop/album.jpg", "./image/desktop/music.jpg", "./image/desktop/exit.jpg"};

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
	JpgInfo_t app_jpginfo[3];
	for(int i = 0; i < 3; i++)
	{
		
		decompress_jpg2buffer(&app_jpginfo[i], app_icon_name[i]);
		
		pBtn_SqList_t newnode = draw_btn(plcdinfo, 150*(i) + 100, 100, &app_jpginfo[i]);
		AddFromTail_btn_sqlist(head, newnode);
		
	}
	//5.创建触摸屏监控子线程
	pthread_t ts_pth_id;
	pthread_create(&ts_pth_id, NULL, ts_monitor, NULL);
	
	//创建socket通讯子线程
	
	
	pthread_t soc_pth_id;
	pthread_create(&soc_pth_id, NULL, remote_control, NULL);
	
	/*
	 *bug:远程控制客户端未连接情况
	 */
	ret = send(soc_fd, "desktop", strlen("desktop"), 0);
	if(ret < 0)
	{
		printf("fail to send, no client online\n");

	}

	//*6.读取按键
	int app_num = 0;
	char app[][20] = {"./app/album", "./app/2"};

	while(1)
	{
		
		/*
		 *backlog:加入线程互次锁，保护数据
		 *
		 */
		if(ts_point.update == true || command.update == true)
		{	
			if(ts_point.update == true)
			{
				app_num = find_which_btn_click(head, ts_point.X, ts_point.Y);
			}
			else if(command.update == true)
			{
				app_num = command.ascii[0] - '0';
			}
			if(app_num != 0)
			{
				if(app_num == 3)
				{
					break;
				}
				else if(app_num == 1)
				{
					ret = send(soc_fd, "album", strlen("album"), 0);
					if(ret < 0)
					{
						printf("fail to send, no client online\n");

					}
					
					album(plcdinfo, &ts_point, &command);
					
					ret = send(soc_fd, "desktop", strlen("desktop"), 0);
					if(ret < 0)
					{
						printf("fail to send, no client online\n");

					}
				}
				//再次刷新桌面
				draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
				draw_pic(plcdinfo, 100, 100, &app_jpginfo[0]);
				draw_pic(plcdinfo, 250, 100, &app_jpginfo[1]);
				draw_pic(plcdinfo, 400, 100, &app_jpginfo[2]);

				printf("%d app\n", app_num);
				
			}
			
			ts_point.update = false;	
			command.update = false;
		
		}
	
	}


	//销毁lcd
	ret = lcd_destroy(plcdinfo);

	//释放背景图片资源
	free(bg_pjpginfo->buff);
	free(bg_pjpginfo);
	
	//销毁链表
	destroy_btn_sqlist(&head);
	
	printf("HomeSmart desktop exit\n");
	return 0;

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
		}

	}

}

void *remote_control(void *arg)
{
	
	int ret;

	//线程分离
	pthread_detach(pthread_self());
	unsigned char *client_ip;
	int client_port;

	//创建服务器
	soc_fd = server_create(3000, NULL, &client_port, &client_ip);
	
	printf("ip: %s, port: %d online\n", client_ip, client_port);	
	
	while(1)
	{
		bzero(command.ascii, sizeof(command.ascii));
		
		ret = recv(soc_fd, &command.ascii, sizeof(command.ascii), 0);
		if(ret < 0)
		{
			perror("error exits in recv");
			shutdown(soc_fd, SHUT_RDWR);
		}
		else if(ret == 0)
		{
			printf("client offline\n");
			shutdown(soc_fd, SHUT_RDWR);
			pthread_exit(NULL);	
		}

		printf("command:%s\n", command.ascii);
		command.update = true;	
	
	}






}



