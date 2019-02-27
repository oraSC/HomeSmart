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
#include "./album.h"
#include "./music.h"
#include <linux/input.h>
#include <strings.h>

#define TOTAL_MUSIC 5


int music(pLcdInfo_t plcdinfo, struct point *pts_point, struct Command *pcommand)
{
	int ret;

	char btn_name[5][30] = {"./image/music/exit.jpg", "./image/music/last.jpg", "./image/music/next.jpg", "./image/music/play.jpg", "./image/music/pause.jpg"};

	//2.打开背景图片
	pJpgInfo_t bg_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
	if(bg_pjpginfo == NULL)
	{
		perror("fail to malloc bg_pjpginfo");
		return -1;
	}
	decompress_jpg2buffer(bg_pjpginfo, "./image/music/bg.jpg");

	//3.加载背景
	draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
	
	
	//*4.加载按键
	//**创建链表头
	pBtn_SqList_t head = create_btn_sqlist();
	
	//**添加按键
	JpgInfo_t exit_jpginfo;
	JpgInfo_t last_jpginfo;
	JpgInfo_t next_jpginfo;
	JpgInfo_t play_jpginfo;
	JpgInfo_t pause_jpginfo;
	

	//***exit按键
	decompress_jpg2buffer(&exit_jpginfo, btn_name[0]);
	pBtn_SqList_t exit_node = draw_btn(plcdinfo, 741, 0, &exit_jpginfo);
	AddFromTail_btn_sqlist(head, exit_node);

	//***last按键
	decompress_jpg2buffer(&last_jpginfo, btn_name[1]);
	pBtn_SqList_t last_node = draw_btn(plcdinfo, 251, 383, &last_jpginfo);
	AddFromTail_btn_sqlist(head, last_node);
	
	//***next按键
	decompress_jpg2buffer(&next_jpginfo, btn_name[2]);
	pBtn_SqList_t next_node = draw_btn(plcdinfo, 483, 383, &next_jpginfo);
	AddFromTail_btn_sqlist(head, next_node);
	
	//***play按键
	decompress_jpg2buffer(&play_jpginfo, btn_name[3]);
	pBtn_SqList_t play_node = draw_btn(plcdinfo, 367, 383, &play_jpginfo);
	AddFromTail_btn_sqlist(head, play_node);

	//***pause按键
	decompress_jpg2buffer(&pause_jpginfo, btn_name[4]);
	pBtn_SqList_t pause_node = draw_btn(plcdinfo, 367, 383, &pause_jpginfo);
	AddFromTail_btn_sqlist(head, pause_node);
	
	int opt;
	int index = 1;
	while(1)
	{

		if(pts_point->update == true || pcommand->update)
		{

			if(pts_point->update == true)
			{
				opt = find_which_btn_click(head, pts_point->X, pts_point->Y);
			}
			if(pcommand->update == true)
			{
				opt = pcommand->ascii[0] - '0';
			}
			switch(opt)
			{
				//退出，资源未释放
				case 1: return 0;
				
				//上一首
				case 2:index--;play_music(index);break;

				//下一首
				case 3:index++;play_music(index);break;

				//播放
				case 4:play_music(index);break;

				//暂停
				case 5:printf("pause\n");break;
			
			
			}

			pts_point->update = false;
			pcommand->update = false;
				
		}
	
	
	
	
	
	}
}


void play_music(int index)
{
	if(index <= 0)
	{
		index = TOTAL_MUSIC;
	}
	else if(index > TOTAL_MUSIC)
	{
		index = 1;
	}

	char music_name[30];

	bzero(music_name, sizeof(music_name));

	sprintf(music_name, "madplay ./music/%d.mp3 &", index);

	system(music_name);
}



