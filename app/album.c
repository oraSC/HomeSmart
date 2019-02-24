#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "../lib/lcd/LCD.h"
#include "../lib/jpg/JPG.h"
#include <linux/input.h>

struct point{
	
	int X;
	int Y;
	int lastX;
	int lastY;
	bool update;

}ts_point;



void *ts_monitor(void *arg);

int main()
{
	int ret;

	char btn_name[2][30] = {"../image/album/last.jpg", "../image/album/next.jpg"};

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
	decompress_jpg2buffer(bg_pjpginfo, "../image/album/bg.jpg");

	//3.加载背景
	draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
	

	//*4.加载按键
	//**创建链表头
	pBtn_SqList_t head = create_btn_sqlist();

	//**添加按键
	JpgInfo_t btn_jpginfo;
	//overview bg		
	decompress_jpg2buffer(&btn_jpginfo, "../image/album/overview_bg.jpg");
	draw_pic(plcdinfo, 600, 0, &btn_jpginfo);
	free(btn_jpginfo.buff);

	//***last按键
	decompress_jpg2buffer(&btn_jpginfo, btn_name[0]);
	pBtn_SqList_t last_node = draw_btn(plcdinfo, 600, 0, &btn_jpginfo);
	AddFromTail_btn_sqlist(head, last_node);
	free(btn_jpginfo.buff);

	//***next按键
	decompress_jpg2buffer(&btn_jpginfo, btn_name[1]);
	pBtn_SqList_t next_node = draw_btn(plcdinfo, 600, 430, &btn_jpginfo);
	AddFromTail_btn_sqlist(head, next_node);
	free(btn_jpginfo.buff);



	//5.创建触摸屏监控子线程
	pthread_t ts_pth_id;
	pthread_create(&ts_pth_id, NULL, ts_monitor, NULL);


	//*读取相册
	JpgInfo_t album_jpginfo[5];
	char album_jpg_name[5][30] = {"../image/album/pic1.jpg", "../image/album/pic2.jpg","../image/album/pic3.jpg","../image/album/pic4.jpg","../image/album/pic5.jpg"};
	for(int i = 0; i < 5; i++)
	{
		decompress_jpg2buffer(album_jpginfo+i, album_jpg_name[i]);
		
	}
	
	//**默认第一张
	int index = 0;
	draw_pic(plcdinfo, 0, 0, &album_jpginfo[index]);
	while(1)
	{
		if(ts_point.update == true)
		{
			int opt = find_which_btn_click(head, ts_point.X, ts_point.Y);
			if(opt > 0)
			{	if(opt == 1)
				{
					index--;
					if(index < 0)
					{
						index = 5 - 1;
					}

				}	
				else {
					index ++;
					if(index >= 5)
					{
						index = 0;
				
					}	
			
				}
				draw_pic(plcdinfo, 0, 0, &album_jpginfo[index]);
			}
		
			ts_point.update = false;
		}
	
	
	
	
	}






	//销毁lcd
	ret = lcd_destroy(plcdinfo);

	//释放背景图片资源
	free(bg_pjpginfo->buff);
	free(bg_pjpginfo);
	
	//销毁链表
	destroy_btn_sqlist(&head);
	
	printf("album exits\n");
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
