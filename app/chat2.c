#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "chat2.h"
#include "../lib/jpg/JPG.h"
#include "../lib/camera/yuyv.h"
#include "../lib/lcd/LCD.h"
#include "../lib/libjpeg/jpeglib.h"
#include "../lib/socket/mysocket.h"

#define RINGON_ANSWER_x         450
#define RINGON_ANSWER_y         400
#define RINGOFF_ANSWER_x        300
#define RINGOFF_ANSWER_y        400
#define VOICECALL_x             200
#define VOICECALL_y             350
#define VIDEOCALL_x             500
#define VIDEOCALL_y             350   
#define RINGOFF_INCALL_x        375
#define RINGOFF_INCALL_y        350

/*
*功能：解压jpgddata并在lcd中显示
*返回值：
*   成功：0
*   失败：-1
*/
int videocall(pLcdInfo_t plcdinfo, pJpgData_t pjpgdata, int socfd);
static bool		draw_pic_notAcolor(pLcdInfo_t plcdinfo, int x, int y, pJpgInfo_t pjpginfo, int color);
struct wait_for_callme_arg{
    
    pLcdInfo_t      plcdinfo;
    pBtn_SqList_t   phead;
    pPoint_t        pts_point;

};
void *wait_for_callme(void *arg);

int chat2(pLcdInfo_t plcdinfo, pPoint_t pts_point)
{
    
    int ret;

    //加载背景
    pJpgInfo_t bg_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for bg_pjpginfo");
        return -1;
    }

    decompress_jpg2buffer(bg_pjpginfo, "./image/chat/bg.jpg");
    draw_pic(plcdinfo, 0, 0, bg_pjpginfo);

    //加载 voiceCall、videoCall
    pJpgInfo_t voiceCall_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for voiceCall_pjpginfo");
        return -1;
    }
    decompress_jpg2buffer(voiceCall_pjpginfo, "./image/chat/voiceCall.jpg");

     pJpgInfo_t videoCall_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for videoCall_pjpginfo");
        return -1;
    }
    decompress_jpg2buffer(videoCall_pjpginfo, "./image/chat/videoCall.jpg");

    //加载 ring on/off 按键
    pJpgInfo_t ringOn_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for ringOn_pjpginfo");
        return -1;
    }

    decompress_jpg2buffer(ringOn_pjpginfo, "./image/chat/ringon.jpg");

    pJpgInfo_t ringOff_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for ringOff_pjpginfo");
        return -1;
    }

    decompress_jpg2buffer(ringOff_pjpginfo, "./image/chat/ringoff.jpg");
    
    //添加按键
    pBtn_SqList_t   phead = create_btn_sqlist();
    pBtn_SqList_t voiceCallBtn = draw_btn(plcdinfo, VOICECALL_x, VOICECALL_y, voiceCall_pjpginfo);
    AddFromTail_btn_sqlist(phead, voiceCallBtn);
    pBtn_SqList_t videoCallBtn = draw_btn(plcdinfo, VIDEOCALL_x, VIDEOCALL_y, videoCall_pjpginfo);
    AddFromTail_btn_sqlist(phead, videoCallBtn);
    
    //创建接电话子线程
    pthread_t waitForCallMePthId;
    struct wait_for_callme_arg ptharg;
    ptharg.plcdinfo = plcdinfo;
    ptharg.phead    = phead;
    ptharg.pts_point = pts_point;
    pthread_create(&waitForCallMePthId, NULL, wait_for_callme, (void *)&ptharg);

    int socFd;
    while(1)
    {
        if(pts_point->update == true)
        {
            pts_point->update == false;
            int click = find_which_btn_click(phead, pts_point->X, pts_point->Y);
            //打电话
            if(click == 2)
            {
                
                socFd = client_create(4001, "202.192.32.52");
                //已拨通、等待接电话
                if(socFd > 0)
                {
                    char answerBuff[10] = {0};
                    ret = recv(socFd, answerBuff, sizeof(answerBuff), 0);
                    if(ret < 0)
                    {
                        perror("error exists in recv answer");
                        /*
                        *backlog:资源未释放
                        */

                    }
                    //接通
                    else if(strcmp(answerBuff, "ring on") == 0)
                    {
                        break;
                    }
                    //被挂断
                    else if(strcmp(answerBuff, "ring on") == 0)
                    {
                        continue;
                    }
                }
            }
        }
    }

    //通话
    //初始化摄像头
    linux_v4l2_yuyv_init("/dev/video7");

    //开启摄像头捕捉
    linux_v4l2_start_yuyv_capturing();

    //捕获的jpg数据
    JpgData_t jpgdata;
    JpgInfo_t jpginfo;
    while(1)
    {
        //获取数据
        linux_v4l2_get_yuyv_data(&jpgdata);

        //转换成jpginfo
        videocall(plcdinfo, &jpgdata, socFd);
        /*
        *backlog:隐藏图标
        */
        

    }
    
}

void *wait_for_callme(void *arg)
{
    int c_port;
    unsigned char *c_ip = NULL;
    //创建服务器
    server_create(4002, NULL, &c_port, &c_ip);
    printf("somebody calls you\nip:%s\nport:%d", c_ip, c_port);
    while(1);


}


#define MY_WIN_WIDTH        128
#define MY_WIN_HEIGHT       96
#define MY_WIN_x            672
#define MY_WIN_y            0
#define A_FRAME_WIDTH       640
#define A_FRAME_HEIGHT      480
#define A_FRAME_SIZE        (A_FRAME_HEIGHT * A_FRAME_WIDTH * 3)    
#define SEND_SINGLE_SIZE    30720   

int videocall(pLcdInfo_t plcdinfo, pJpgData_t pjpgdata, int socfd)
{
    JpgInfo_t src_jpginfo;
    JpgInfo_t resize_jpginfo;
    JpgInfo_t send_jpginfo;

    //解压成buff
    decompress_jpgdata2buffer(pjpgdata->data, pjpgdata->size, &src_jpginfo);

    //压缩
    jpg_resize(&src_jpginfo, &resize_jpginfo, MY_WIN_WIDTH, MY_WIN_HEIGHT);
    jpg_resize(&src_jpginfo, &send_jpginfo, A_FRAME_WIDTH, A_FRAME_HEIGHT);

    //发送图像
    int rest_size = A_FRAME_SIZE;
	int send_num = A_FRAME_SIZE / SEND_SINGLE_SIZE + 1;
  	for(int i = 0; i < send_num; i++)
	{
		if(rest_size > SEND_SINGLE_SIZE)
		{
			Send_andwait(socfd, send_jpginfo.buff + i*SEND_SINGLE_SIZE, SEND_SINGLE_SIZE, 0);
		}
		else 
		{
			Send_andwait(socfd, send_jpginfo.buff + i*SEND_SINGLE_SIZE, rest_size, 0);

		}
		//修改剩余大小
		rest_size = rest_size - SEND_SINGLE_SIZE;

	}

    draw_pic(plcdinfo, MY_WIN_x, MY_WIN_y, &resize_jpginfo);
    
    
    //释放资源
    free(src_jpginfo.buff);
    free(send_jpginfo.buff);
    free(resize_jpginfo.buff);

}


static bool draw_pic_notAcolor(pLcdInfo_t plcdinfo, int x, int y, pJpgInfo_t pjpginfo, int color)
{
	unsigned int *base = plcdinfo->base + y * plcdinfo->width + x;
	
	//画图片
	
	
	int min_W = (plcdinfo->width  - x) < pjpginfo->width  ? plcdinfo->width -  x : pjpginfo->width;
	int min_H = (plcdinfo->height - y) < pjpginfo->height ? plcdinfo->height - y : pjpginfo->height;
	//printf("minW:%d, minH:%d",min_W,min_H);
	for(int rows = 0; rows < min_H - 1; rows++)
	{
		
		for(int cols = 0; cols < min_W - 1; cols++)
		{
			unsigned char *pR = pjpginfo->buff + rows * pjpginfo->rowsize + cols * 3;
			unsigned char *pG = pR + 1;
			unsigned char *pB = pR + 2;
			if(!((*pR - ((0xFF<<0)&color) < 10) || (*pG - ((0xFF<<8)&color) < 10)  || (*pB - ((0xFF<<16)&color) < 10)) )
			
			{
			memcpy(base + cols, pjpginfo->buff + rows * pjpginfo->rowsize  + cols * 3, 3 );
			}
			
			//printf("%d\n",cols);	
		}
		//printf("base add%d\n", plcdinfo->width);	
		base += plcdinfo->width;
	
	
	}

	/*
	 *backlog:返回值未完善
	 */

}