#include <stdio.h>
#include "garage.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include "../lib/serial/serial.h"

int garage(pLcdInfo_t plcdinfo, struct point *pts_point)
{

	int ret;

	//初始化车库管理结构体
	Garage_Manage_t garage_manage;
	bzero(&garage_manage, sizeof(garage_manage));
	for(int i = 0; i < CAPACITY; i++)
	{
		//定义停车位编号
		garage_manage.park_positions[i] = i + 1;
	
	}

	//加载背景
	pJpgInfo_t bg_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));

	decompress_jpg2buffer(bg_pjpginfo, "./image/garage/bg.jpg");
	draw_pic(plcdinfo, 0, 0, bg_pjpginfo);

	//加载blank、car
	pJpgInfo_t blank_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
	decompress_jpg2buffer(blank_pjpginfo, "./image/garage/blank.jpg");
	pJpgInfo_t car_pjpginfo =(JpgInfo_t *)malloc(sizeof(JpgInfo_t));
	decompress_jpg2buffer(car_pjpginfo, "./image/garage/car.jpg");

	//加载exit
	//创建head
	pBtn_SqList_t head = create_btn_sqlist();

	pJpgInfo_t exit_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
	decompress_jpg2buffer(exit_pjpginfo, "./image/garage/exit.jpg");
	pBtnInfo_t exit_node = draw_btn(plcdinfo, 730, 0, exit_pjpginfo);
	AddFromTail_btn_sqlist(head, exit_node);

	//*打开串口1->（进入车库RFID）
	//*打开串口2->（离开车库RFID）
	//**1.打开设备文件
	int serial1_fd = open("/dev/ttySAC1", O_RDWR | O_NOCTTY);
	if(serial1_fd < 0)
	{
		perror("fail to open serial1");
		return -1;
	}

	int serial2_fd = open("/dev/ttySAC2", O_RDWR | O_NOCTTY);
	if(serial2_fd < 0)
	{
		perror("fail to open serial2");
		return -1;
	}

	//**2.配置串口属性
	serial_setattr(serial1_fd);
	serial_setattr(serial2_fd);
	

	printf("RFID starts to work\n");
	while(1)
	{
		if(pts_point->update == true)
		{
			ret = find_which_btn_click(head, pts_point->X, pts_point->Y);
			if(ret == 1)
			{
				//退出车库管理系统
				goto exit_garage;
			}
		
		}
			
		//检测进入RFID
		ret = send_A(serial1_fd);
		if(ret < 0)
		{
			return -1;
		}
		else if(ret >= 1)
		{
			//printf("no card\n");
			//continue;
		}
		else if(ret == 0)
		{
			int id;
			id = send_B(serial1_fd);
			if(id == 0)
			{
				//continue;
			
			}
			else if(id > 0)
			{
				//printf("id:0x%08x\n", id);
				//进入车库
				pCar_t pnew_car;
				pnew_car = enter_garage(&garage_manage, id);
				if(pnew_car == NULL)
				{
					printf("please go to exit\n");
				
				}
				else if(pnew_car)
				{
					printf("wlecome\nid:ox%08x\npark_pos:%d\n", pnew_car->id, pnew_car->park_pos);
					park_update(plcdinfo, car_pjpginfo, pnew_car->park_pos);	
					//sleep(1);
					
				
				}
			}
		
		
		}
	
		//检测离开RFID
		ret = send_A(serial2_fd);
		if(ret < 0)
		{
			return -1;
		}
		else if(ret >= 1)
		{
			printf("no card\n");
			//continue;
		}
		else if(ret == 0)
		{
			int id;
			id = send_B(serial2_fd);
			if(id == 0)
			{
				//continue;
			
			}
			else if(id > 0)
			{
				//printf("id:0x%08x\n", id);
				//进入车库
				pCar_t pexit_car;
				pexit_car = exit_garage(&garage_manage, id);
				if(pexit_car == NULL)
				{
					printf("your car isn't in the garage\n");
				
				}
				else if(pexit_car)
				{
					printf("byebye:\nid:ox%08x\npark_pos:%d\n", pexit_car->id, pexit_car->park_pos);
					park_update(plcdinfo, blank_pjpginfo, pexit_car->park_pos);	
					sleep(1);

				
				}
			}
		
		
		}
	

	
	
	}

exit_garage:
	//释放堆资源
	free(bg_pjpginfo->buff);
	free(bg_pjpginfo);
	free(blank_pjpginfo->buff);
	free(blank_pjpginfo);
	free(car_pjpginfo->buff);
	free(car_pjpginfo);
	free(exit_pjpginfo->buff);
	free(exit_pjpginfo);

	close(serial1_fd);
	
}


pCar_t enter_garage(pGarage_Manage_t pgarage_manage, int id)
{
	
	//查询id是否已存在车库
	for(int i = 0; i < pgarage_manage->num; i++)
	{
		if(pgarage_manage->car[i].id == id )
		return NULL;
	
	}
	
	//将车辆信息添加进车库管理器
	pgarage_manage->car[pgarage_manage->num].id 		= id;
	//始终取第一个（第一个不一定为1车位，乱序数组）
	pgarage_manage->car[pgarage_manage->num].park_pos 	= pgarage_manage->park_positions[0];
	//每一次与最后一个有效车位交换
	/*
	 *bug:
	 */
	int temp = pgarage_manage->park_positions[0];
	pgarage_manage->park_positions[0] = pgarage_manage->park_positions[CAPACITY - pgarage_manage->num - 1];
	pgarage_manage->park_positions[CAPACITY - pgarage_manage->num - 1] = temp;
	
	pgarage_manage->car[pgarage_manage->num].time_sec 	= 0;
	pgarage_manage->car[pgarage_manage->num].time_min 	= 0;
	pgarage_manage->car[pgarage_manage->num].time_hour 	= 0;
	pgarage_manage->car[pgarage_manage->num].charge 	= 0;
	
	//
	(pgarage_manage->num) ++;


	//返回刚进入车库车辆的停车信息
	return &(pgarage_manage->car[pgarage_manage->num - 1]);

}

pCar_t exit_garage(pGarage_Manage_t pgarage_manage, int id)
{
	
	int i = 0;

	//查询id是否已存在车库
	for(i = 0; i < pgarage_manage->num; i++)
	{
		if(pgarage_manage->car[i].id == id)	
		{
			break;
		}
	}

	//不存在
	if(i == pgarage_manage->num)
	{
		return NULL;
	
	}

	printf("car:id:0x%08x,pos:%d\n", pgarage_manage->car[i].id, pgarage_manage->car[i].park_pos);


	//将离开车位的信息从车库管理器中去除(尾除法)
	Car_t temp;
	temp = pgarage_manage->car[i];
	pgarage_manage->car[i] = pgarage_manage->car[pgarage_manage->num - 1];
	pgarage_manage->car[pgarage_manage->num - 1] = temp;

	//调整车位数组
	int tmp = pgarage_manage->park_positions[CAPACITY - i - 1];
	pgarage_manage->park_positions[CAPACITY - i - 1] = pgarage_manage->park_positions[CAPACITY - pgarage_manage->num];
	pgarage_manage->park_positions[CAPACITY - pgarage_manage->num] = tmp;

	//
	pgarage_manage->num--;

	return &(pgarage_manage->car[pgarage_manage->num]);


}




int park_update(pLcdInfo_t plcdinfo, pJpgInfo_t pjpginfo, int pos)
{
	if(plcdinfo == NULL || pjpginfo == NULL || pos == 0)
	{
		printf("plcdinfo or pjpginfo or pos can't be NULL\n");
		return -1;
	}

	int x, y;

	if(pos <= CAPACITY / 2)
	{
		x, y;
		//计算x,y坐标
		x = (pos-1)*200 + 40;
		y = 20;
	
	}

	else if(pos > CAPACITY / 2)
	{
		x, y;
		//计算x,y坐标
		x = (pos - 1 - CAPACITY / 2)*200 + 40;
		y = 240 + 20;
	
	}
	draw_pic(plcdinfo, x, y, pjpginfo);	
	
	return 0;


}



