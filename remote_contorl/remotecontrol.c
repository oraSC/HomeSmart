#include <stdio.h>
#include "../lib/socket/mysocket.h"
#include "string.h"
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>

void *recv_routine(void *arg);
void *desktop_interface(void *arg);
void *album_interface(void *arg);
void *music_interface(void *arg);

bool interface_update, desktop_enable, album_enable, music_enable;


int main()
{

	//初始化使能桌面
	interface_update = true;
	desktop_enable = true;
	album_enable = false;
	music_enable = false;
	
	printf("\033c");
	printf("-------------------------HomeSmart---------------------\n");
	printf("<1> control\t<2> check information\ninput:");
	fflush(stdout);

	char choose[2];
	fgets(choose, 2, stdin);
	if(choose[0] == '2')
	{
		//创建客户端
		int acc_fd = client_create(4000, "202.192.32.92");
		unsigned char command[10];
		while(1)
		{
			printf("\033c");
			printf("--------------------garage check infomation-------------\n");
			printf("please input your card id:");
			fflush(stdout);

			bzero(command, sizeof(command));
			
			fgets(command, sizeof(command), stdin);
			send(acc_fd, &command, strlen(command), 0);
		
		}

	
	}
	else {



		//创建客户端
		int acc_fd = client_create(3000, "202.192.32.92");

		//创建读线程
		pthread_t recv_pth_id;
		pthread_create(&recv_pth_id, NULL, recv_routine, &acc_fd);

		//创建desktop界面
		pthread_t desktop_pth_id;
		pthread_create(&desktop_pth_id, NULL, desktop_interface, &acc_fd);
		
		//创建album界面
		pthread_t album_pth_id;
		pthread_create(&album_pth_id, NULL, album_interface, &acc_fd);

		//创建album界面
		pthread_t music_pth_id;
		pthread_create(&music_pth_id, NULL, music_interface, &acc_fd);


		unsigned char command[10];
		while(1)
		{
			bzero(command, sizeof(command));
			
			fgets(command, sizeof(command), stdin);
			send(acc_fd, &command, strlen(command), 0);
			interface_update = true;
		
		}

	}
	return 0;
		
}


void *desktop_interface(void *arg)
{

	int acc_fd = *((int *)arg);

	while(1)
	{
		if(interface_update == true && desktop_enable == true && album_enable == false && music_enable == false)
		{
			//清屏
			printf("\033c");
			printf("-----------------------desktop-----------------------\n");
			printf("<1> album\t<2> music\t<3> exit\n");
			printf("input: ");
			fflush(stdout);
			interface_update = false;
		}
	
	
	}


}


void *album_interface(void *arg)
{
	
	int acc_fd = *((int *)arg);

	while(1)
	{
		
		if(interface_update == true && album_enable == true && desktop_enable == false && music_enable == false)
		{
			//清屏
			printf("\033c");
			printf("-----------------------album-----------------------\n");
			printf("<1> exit\t<2> last\t<3> next\n");
			printf("input: ");
			fflush(stdout);
			interface_update = false;
		}
	
	
	}


}

void *music_interface(void *arg)
{
	
	int acc_fd = *((int *)arg);

	while(1)
	{
		
		if(interface_update == true && album_enable == false && desktop_enable == false && music_enable == true)
		{
			//清屏
			printf("\033c");
			printf("-----------------------music-----------------------\n");
			printf("<1> exit\t<2> last\t<3> next\t<4>play/pause \n");
			printf("input: ");
			fflush(stdout);
			interface_update = false;
		}
	
	
	}


}




void *recv_routine(void *arg)
{
	
	int ret;	
	int acc_fd = *((int *)arg);

	unsigned char buff[10];
	while(1)
	{
		bzero(buff, sizeof(buff));
	
		ret = recv(acc_fd, buff, sizeof(buff), 0);
		if(ret < 0)
		{
			perror("error exits in recv");
			break;	
		}
		if(ret == 0)
		{
			printf("server disconnect");
			break;
		}
		//printf("%s\n", buff);
		/*
		 *
		 * 加入线程互斥锁,不然会造成刷新不及时
		 */
		if(strcmp(buff, "desktop") == 0)
		{
			interface_update = true;
			desktop_enable = true;
			album_enable = false;
			music_enable = false;	
		}
		else if(strcmp(buff, "album") == 0)
		{

			interface_update = true;
			album_enable = true;
			desktop_enable = false;
			music_enable = false;
		}
		else if(strcmp(buff, "music") == 0)
		{
			interface_update = true;
			album_enable = false;
			desktop_enable = false;
			music_enable = true;
			
		
		}
		
	}

}
