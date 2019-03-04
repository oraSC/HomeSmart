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

	//*打开串口
	//**1.打开设备文件
	int serial1_fd = open("/dev/ttySAC1", O_RDWR | O_NOCTTY);
	if(serial1_fd < 0)
	{
		perror("fail to open serial1");
		return -1;
	}

	//**2.配置串口属性
	serial_setattr(serial1_fd);


	while(1)
	{
		ret = send_A(serial1_fd);
		if(ret < 0)
		{
			return -1;
		}
		else if(ret >= 1)
		{
			printf("no card\n");
			continue;
		}
		else if(ret == 0)
		{
			int id;
			id = send_B(serial1_fd);
			if(id == 0)
			{
				continue;
			
			}
			else if(id > 0)
			{
				//printf("id:0x%08x\n", id);
				//进入车库
				bool ret_bool;
				ret_bool = enter_garage(&garage_manage, id);
				if(ret_bool == false)
				{
					printf("please go to exit\n");
				
				}
				else if(ret_bool == true)
				{
					printf("welcome\n");
				
				}
			}
		
		
		}
	
	
	
	
	}
}


bool enter_garage(pGarage_Manage_t pgarage_manage, int id)
{
	
	//查询id是否已存在车库
	for(int i = 0; i < pgarage_manage->num; i++)
	{
		if(pgarage_manage->car[i].id == id )
		return false;
	
	}
	
	//将车辆信息添加进车库管理器
	pgarage_manage->car[pgarage_manage->num].id 		= id;
	pgarage_manage->car[pgarage_manage->num].park_pos 	= pgarage_manage->park_positions[pgarage_manage->num];
	pgarage_manage->car[pgarage_manage->num].time_sec 	= 0;
	pgarage_manage->car[pgarage_manage->num].time_min 	= 0;
	pgarage_manage->car[pgarage_manage->num].time_hour 	= 0;
	pgarage_manage->car[pgarage_manage->num].charge 	= 0;
	
	//
	(pgarage_manage->num) ++;

	return true;

}






