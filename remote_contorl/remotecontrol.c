#include <stdio.h>
#include "../lib/socket/mysocket.h"
#include "string.h"
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define CONTROL_PORT 	3000
#define CHECK_INFO_PORT	4000

void *recv_routine(void *arg);
void *update_interface(void *arg);

bool interface_update, desktop_enable, album_enable, music_enable, garage_enable;


int main()
{

	//初始化使能桌面
	interface_update = true;

	int acc_fd;
	

	printf("\033c");
	printf("-------------------------HomeSmart---------------------\n");
	printf("<1> control\t<2> check information\ninput:");
	fflush(stdout);

	char choose[3] = {0};
	fgets(choose, sizeof(choose), stdin);
	if(choose[0] == '1')
	{
		desktop_enable 	= true;
		album_enable 	= false;
		music_enable 	= false;
		garage_enable 	= false;

		acc_fd = client_create(CONTROL_PORT, "202.192.32.12");
	}
	else if(choose[0] == '2')
	{
		
		desktop_enable 	= false;
		album_enable 	= false;
		music_enable 	= false;
		garage_enable 	= true;

		acc_fd = client_create(CHECK_INFO_PORT, "202.192.32.12");	
	}

	//创建读线程
	pthread_t recv_pth_id;
	pthread_create(&recv_pth_id, NULL, recv_routine, &acc_fd);

	//创建update_interface线程
	pthread_t update_pth_id;
	pthread_create(&update_pth_id, NULL, update_interface, NULL);

	unsigned char command[10];
	while(1)
	{
		bzero(command, sizeof(command));
		
		fgets(command, sizeof(command), stdin);
		send(acc_fd, &command, strlen(command), 0);
		interface_update = true;
	
	}

	return 0;
		
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
		else 
		{
			printf("your car charge: $%s\n", buff);
			sleep(2);
			interface_update = true;
			garage_enable	= true;
		}
		
	}

}

void *update_interface(void *arg)
{

	while(1)
	{
		if(interface_update == true)
		{
			
			if(desktop_enable == true)
			{
					//清屏
				printf("\033c");
				printf("----------------------desktop-----------------------\n");
				printf("<1> album\t<2> music\t<3> exit\n");
				printf("input: ");
				fflush(stdout);
				interface_update = false;
			}
			else if(album_enable == true)
			{
				//清屏
				printf("\033c");
				printf("-----------------------album-----------------------\n");
				printf("<1> exit\t<2> last\t<3> next\n");
				printf("input: ");
				fflush(stdout);
				interface_update = false;
			}
			else if(music_enable == true)
			{
				//清屏
				printf("\033c");
				printf("-----------------------music-----------------------\n");
				printf("<1> exit\t<2> last\t<3> next\t<4>play/pause \n");
				printf("input: ");
				fflush(stdout);
				interface_update = false;
			}
			else if(garage_enable == true)
			{
				//清屏
				printf("\033c");
				printf("------------garage check information---------------\n");
				printf("please input your card id: ");
				fflush(stdout);
				interface_update = false;
				garage_enable	= false;
			}
				
		}
	
	
	}


}


