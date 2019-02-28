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

void *gif_routine(void *arg);
bool gif_shutdown;

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
	//pBtn_SqList_t pause_node = draw_btn(plcdinfo, 367, 383, &pause_jpginfo);
	//AddFromTail_btn_sqlist(head, pause_node);
	
	int opt;
	int index = 1;
	int play_count = 1;

	//创建gif播放子线程
	pthread_t gif_pth_id;
	pthread_create(&gif_pth_id, NULL, gif_routine, plcdinfo);
	gif_shutdown = false;

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
				case 1: 
					gif_shutdown = true;
				       	system("killall -CONT madplay");
					system("killall madplay");
					goto exit;
				
				//上一首
				case 2:index--;
				       system("killall -CONT madplay");
				       system("killall madplay");
				       play_music(&index);
				       play_count = 2; 
				       draw_pic(plcdinfo, 367, 383, &pause_jpginfo);
				       break;

				//下一首
				case 3:index++;
				       system("killall -CONT madplay");
				       system("killall madplay");
				       play_music(&index);
				       play_count = 2;
				       draw_pic(plcdinfo, 367, 383, &pause_jpginfo);
				       break;

				case 4:
				       	//播放
					if(play_count == 1)
					{
						play_music(&index);
						play_count = 2;
						draw_pic(plcdinfo, 367, 383, &pause_jpginfo);
						break;
					}
					//暂停
					else if(play_count == 2)
					{
						system("killall -STOP madplay");
						play_count = 3;
						draw_pic(plcdinfo, 367, 383, &play_jpginfo);
						break;
					}
					//继续
					else if(play_count == 3)
					{
						system("killall -CONT madplay");
						play_count = 2;
						draw_pic(plcdinfo, 367, 383, &pause_jpginfo);
						break;
					}
				//暂停
				//case 5:system("killall -STOP madplay");printf("pause\n");break;
			
			
			}

			pts_point->update = false;
			pcommand->update = false;
				
		}
	
	}

exit:
	pthread_join(gif_pth_id, NULL);
	//释放资源
	free(bg_pjpginfo->buff);
	free(bg_pjpginfo);
	destroy_btn_sqlist(&head);
	
	free(exit_jpginfo.buff);
	free(last_jpginfo.buff);
	free(next_jpginfo.buff);
	free(play_jpginfo.buff);
	free(pause_jpginfo.buff);
	return 0;


}


void play_music(int *index)
{
	if(*index <= 0)
	{
		*index = TOTAL_MUSIC;
	}
	else if(*index > TOTAL_MUSIC)
	{
		*index = 1;
	}

	char music_name[30];

	bzero(music_name, sizeof(music_name));

	sprintf(music_name, "madplay ./music/%d.mp3 &", *index);
	printf("%s\n", music_name);

	system(music_name);
}

void *gif_routine(void *arg)
{
	pLcdInfo_t plcdinfo = (LcdInfo_t *)arg;

	//读取图片
	JpgInfo_t gif_jpginfo[11];
	for(int i = 0; i < 11; i++)
	{
		char path[40];
		bzero(path, sizeof(path));
		
		sprintf(path, "./image/music/gif/frame%d.jpg", i);

		decompress_jpg2buffer(&gif_jpginfo[i], path);
	
	}
	
	
	while(1)
	{
		for(int i = 0; i < 11; i++)
		{
			draw_pic(plcdinfo, 280, 70, &gif_jpginfo[i]);
			usleep(200000);	
			if(gif_shutdown == true)	
			{
				for(int j = 0; j < 11; j++)
				{
					free(gif_jpginfo[j].buff);
					printf("Y\n");	
				}
				pthread_exit(NULL);
		
			}
		}
	
	}






}




