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

#define TOTAL_PIC 7


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
	
	
	//overview bg		
	JpgInfo_t overviewBg_jpginfo;

	decompress_jpg2buffer(&overviewBg_jpginfo, "../image/album/overview_bg.jpg");
	draw_pic(plcdinfo, 600, 50, &overviewBg_jpginfo);


	//*4.加载按键
	//**创建链表头
	pBtn_SqList_t head = create_btn_sqlist();
	
	//**添加按键
	JpgInfo_t btn_jpginfo;
	
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


	/*
	 *backlog:使用双向链表实现，资源未释放
	 *
	 */
	//*读取原图
	JpgInfo_t src_jpginfo[TOTAL_PIC];
	char src_jpg_name[TOTAL_PIC][30] = {"../image/album/pic1.jpg", "../image/album/pic2.jpg","../image/album/pic3.jpg","../image/album/pic4.jpg","../image/album/pic5.jpg", "../image/album/pic6.jpg", "../image/album/pic7.jpg"};
	for(int i = 0; i < TOTAL_PIC; i++)
	{
		decompress_jpg2buffer(src_jpginfo+i, src_jpg_name[i]);
		
	}
	/*
	 *bug:无法缩放图片中的pic2_wrong
	 *
	 */
	//缩放图片
	JpgInfo_t album_jpginfo[TOTAL_PIC];
	for(int i = 0; i < TOTAL_PIC; i++)
	{
		int resize_width = 0, resize_height = 0;
		int ratio = 0 , ratio_w, ratio_h = 0;
		
		//长或宽大于
		if(src_jpginfo[i].width > 600 || src_jpginfo[i].height > 480)
		{
			ratio_w = 100 * src_jpginfo[i].width / 600;
			ratio_h = 100 * src_jpginfo[i].height / 480;
			//取大者
			ratio = ratio_w > ratio_h?ratio_w:ratio_h;
			
			resize_width = src_jpginfo[i].width * 100 / ratio;
			resize_height = src_jpginfo[i].height * 100 / ratio;

		}
		
		//长宽均小于
		else if(src_jpginfo[i].width <= 600 && src_jpginfo[i].height <= 480 )
		{
			ratio_w = 100*600 / src_jpginfo[i].width;
			ratio_h = 100*480 / src_jpginfo[i].height;

			ratio = ratio_w < ratio_h?ratio_w:ratio_h;
			
			resize_width = ratio * src_jpginfo[i].width / 100;
			resize_height = ratio * src_jpginfo[i].height / 100;
		}

		jpg_resize(src_jpginfo + i, album_jpginfo + i, resize_width, resize_height);
	}

	//缩放图片
	JpgInfo_t overview_jpginfo[TOTAL_PIC];
	for(int i = 0; i < TOTAL_PIC; i++)
	{
		int resize_width = 0, resize_height = 0;
		int ratio = 0 , ratio_w, ratio_h = 0;
		
		//长或宽大于
		if(src_jpginfo[i].width > 200 || src_jpginfo[i].height > 100)
		{
			ratio_w = 100 * src_jpginfo[i].width / 200;
			ratio_h = 100 * src_jpginfo[i].height / 100;
			//取大者
			ratio = ratio_w > ratio_h?ratio_w:ratio_h;
			
			resize_width = src_jpginfo[i].width * 100 / ratio;
			resize_height = src_jpginfo[i].height * 100 / ratio;

		}
		
		//长宽均小于
		else if(src_jpginfo[i].width <= 200 && src_jpginfo[i].height <= 100 )
		{
			ratio_w = 100*200 / src_jpginfo[i].width;
			ratio_h = 100*100 / src_jpginfo[i].height;

			ratio = ratio_w < ratio_h?ratio_w:ratio_h;
			
			resize_width = ratio * src_jpginfo[i].width / 100;
			resize_height = ratio * src_jpginfo[i].height / 100;
		}
		
		//printf("%d\t%d\n", resize_width, resize_height);
		jpg_resize(src_jpginfo + i, overview_jpginfo + i, resize_width, resize_height);
		//printf("z\n");
	}
	
		
	//**默认第一张
	int index = 0;
	int last_index = TOTAL_PIC - 1;
	int next_index = index + 1;
	middle_show(plcdinfo, 0, 0, 600, 480, &album_jpginfo[index]);	
	while(1)
	{
		if(ts_point.update == true)
		{
			int opt = find_which_btn_click(head, ts_point.X, ts_point.Y);
			if(opt > 0)
			{	if(opt == 1)
				{
					index--;
					last_index--;
					next_index--;
					if(index < 0)
					{
						index = TOTAL_PIC - 1;
					}
					if(last_index < 0)
					{
						last_index = TOTAL_PIC - 1;
					}
					if(next_index < 0)
					{
						next_index = TOTAL_PIC - 1;
					}
						

				}	
				else {
					index ++;
					last_index++;
					next_index++;
					if(index >= TOTAL_PIC)
					{
						index = 0;
				
					}	
					if(last_index >= TOTAL_PIC)
					{
						last_index = 0;
					}
					if(next_index >= TOTAL_PIC)
					{
						next_index = 0;
					}
			
				}
				//draw_pic(plcdinfo, 0, 0, &album_jpginfo[index]);
				//bg
				draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
				draw_pic(plcdinfo, 600, 50, &overviewBg_jpginfo);
				//相册
				middle_show(plcdinfo, 0, 0, 600, 480, &album_jpginfo[index]);
				//overview

				middle_show(plcdinfo, 600, 50, 200, 100, &overview_jpginfo[last_index]);
				middle_show(plcdinfo, 600, 190, 200, 100, &overview_jpginfo[index]);
				middle_show(plcdinfo, 600, 330, 200, 100, &overview_jpginfo[next_index]);
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
