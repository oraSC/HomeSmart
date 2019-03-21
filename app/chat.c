#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "chat.h"
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
#define A_FRAME_WIDTH           640
#define A_FRAME_HEIGHT          480
#define A_FRAME_SIZE            (A_FRAME_HEIGHT * A_FRAME_WIDTH * 3)
#define RECV_SINGLE_SIZE        30720

/*
*功能：解压jpgddata并在lcd中显示
*返回值：
*   成功：0
*   失败：-1
*/
int decompress_jpgdataAndshow(  pLcdInfo_t plcdinfo, int x, int y, 
                                pJpgData_t pjpgdata);
static bool		draw_pic_notAcolor(pLcdInfo_t plcdinfo, int x, int y, pJpgInfo_t pjpginfo, int color);

struct wait_for_callme_arg{
    
    pLcdInfo_t      plcdinfo;
    pBtn_SqList_t   phead;
    pPoint_t        pts_point;
    pJpgInfo_t      bg_pjpginfo;
    pJpgInfo_t      ringOn_pjpginfo;
    pJpgInfo_t      ringOff_pjpginfo;


};
void *wait_for_callme(void *arg);

int chat(pLcdInfo_t plcdinfo, pPoint_t pts_point)
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

    //加载 ring on/off 
    pJpgInfo_t ringOn_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for ringOn_pjpginfo");
        return -1;
    }
    decompress_jpg2buffer(ringOn_pjpginfo, "./image/chat/ringOn.jpg");

    pJpgInfo_t ringOff_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for ringOff_pjpginfo");
        return -1;
    }
    decompress_jpg2buffer(ringOff_pjpginfo, "./image/chat/ringOff.jpg");

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
    ptharg.bg_pjpginfo = bg_pjpginfo;
    ptharg.ringOff_pjpginfo = ringOff_pjpginfo;
    ptharg.ringOn_pjpginfo = ringOn_pjpginfo;

    pthread_create(&waitForCallMePthId, NULL, wait_for_callme, (void *)&ptharg);

    //捕获的jpg数据
    JpgData_t jpgdata;

    //初始化摄像头
    //linux_v4l2_yuyv_init("/dev/video7");

    //开启摄像头捕捉
    //linux_v4l2_start_yuyv_capturing();
    JpgInfo_t jpginfo;
    while(1)
    {
        sleep(1);
        // if(pts_point->update == true)
        // {
        //     pts_point->update == false;
        //     int click = find_which_btn_click(phead, pts_point->X, pts_point->Y);
        //     //打电话
        //     if(click == 1)
        //     {
        //         client_create(4002, "202.192.32.18");
        //         while(1);

        //     }




        // }
        // //获取数据
        // linux_v4l2_get_yuyv_data(&jpgdata);

        // //转换成jpginfo
        // decompress_jpgdataAndshow(plcdinfo, 80, 0,  &jpgdata);
        // /*
        // *backlog:隐藏图标
        // */
        // draw_pic_notAcolor(plcdinfo, RING_x, RING_y, ringOff_pjpginfo, 0x00000000);
        
       


    }

    
    
}

void *wait_for_callme(void *arg)
{
    int ret;
    
    pLcdInfo_t plcdinfo = ((struct wait_for_callme_arg *)arg)->plcdinfo;
    pBtn_SqList_t phead = ((struct wait_for_callme_arg *)arg)->phead;
    pPoint_t pts_point = ((struct wait_for_callme_arg *)arg)->pts_point;
    pJpgInfo_t bg_pjpginfo = ((struct wait_for_callme_arg *)arg)->bg_pjpginfo;
    pJpgInfo_t ringOff_pjpginfo = ((struct wait_for_callme_arg *)arg)->ringOff_pjpginfo;
    pJpgInfo_t ringOn_pjpginfo = ((struct wait_for_callme_arg *)arg)->ringOn_pjpginfo;

    int c_port;
    unsigned char *c_ip = NULL;
    //创建服务器
    int socFd;
    socFd = server_create(4001, NULL, &c_port, &c_ip);
    printf("----------desktop---------\nsomebody calls you\nip:%s\nport:%d\n", c_ip, c_port);
    
    //加载有电话来界面
    clear_btn_sqlist(&phead);
    draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
    pBtn_SqList_t ringOnBtn = draw_btn(plcdinfo, RINGON_ANSWER_x, RINGON_ANSWER_y, ringOn_pjpginfo);
    AddFromTail_btn_sqlist(phead, ringOnBtn);
    pBtn_SqList_t ringOffBtn = draw_btn(plcdinfo, RINGOFF_ANSWER_x, RINGOFF_ANSWER_y, ringOff_pjpginfo);
    AddFromTail_btn_sqlist(phead, ringOffBtn);

    //等待用户接电话
    while(1)
    {

        if(pts_point->update == true)
        {
            pts_point->update == false;
            int click = find_which_btn_click(phead, pts_point->X, pts_point->Y);
            if(click <= 0)
            {
                continue;
            }
            else if(click == 1)
            {
                ret = send(socFd, "ring on", strlen("ring on"), 0);
                if(ret < 0)
                {
                    perror("error exists in send");
                }
                break;
            }
            else if(click == 2)
            {
                ret = send(socFd, "ring off", strlen("ring off"), 0);
                if(ret < 0)
                {
                    perror("error exists in send");
                }
                break;
            }
        }




    }
    //加载有电话来界面
    clear_btn_sqlist(&phead);
    draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
    pBtn_SqList_t node = draw_btn(plcdinfo, RINGOFF_INCALL_x, RINGOFF_INCALL_y, ringOff_pjpginfo);
    AddFromTail_btn_sqlist(phead, node);
    //in call
    JpgInfo_t recv_jpginfo;
    recv_jpginfo.width = A_FRAME_WIDTH;
    recv_jpginfo.height = A_FRAME_HEIGHT;
    recv_jpginfo.bicount = 24;
    recv_jpginfo.rowsize = recv_jpginfo.width * recv_jpginfo.bicount / 8;
    recv_jpginfo.buff = (unsigned char *)malloc(recv_jpginfo.height *recv_jpginfo.rowsize);

    while(1)
    {
        bzero(recv_jpginfo.buff, sizeof(recv_jpginfo.buff));
        //接受图像
        /*
        *bug:偶尔出现丢失某一端，造成图像数据整体上移
        */
        int rest_size = A_FRAME_SIZE;
        int recv_num = A_FRAME_SIZE / RECV_SINGLE_SIZE + 1;
        for(int i = 0; i < recv_num; i++)
        {
            if(rest_size > RECV_SINGLE_SIZE)
            {
                Recv_andreply(socFd, recv_jpginfo.buff + i*RECV_SINGLE_SIZE, RECV_SINGLE_SIZE, 0);
            }
            else 
            {
                Recv_andreply(socFd, recv_jpginfo.buff + i*RECV_SINGLE_SIZE, rest_size, 0);

            }
            //修改剩余大小
            rest_size = rest_size - RECV_SINGLE_SIZE;

        }

        draw_pic(plcdinfo, 80, 0, &recv_jpginfo);

    }


}


int decompress_jpgdataAndshow(pLcdInfo_t plcdinfo, int x, int y, pJpgData_t pjpgdata)
{
    	//创建、初始化解码对象，错误处理
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr pub;
	
	// //绑定错误处理
	cinfo.err = jpeg_std_error(&pub);
	
	// //创建解码对象
	jpeg_create_decompress(&cinfo);

	// //指定jpg源文件
	// jpeg_stdio_src(&cinfo, src_file);
	//解码内存中的jpg数据
	jpeg_mem_src(&cinfo, pjpgdata->data, pjpgdata->size);

	//读取jpg文件头(文件参数)
	jpeg_read_header(&cinfo, TRUE);

	//设置解压参数
	

	//开始解压
	jpeg_start_decompress(&cinfo);
	
	//读取参数
	int width   = cinfo.output_width;
	int height  = cinfo.output_height;
	int rowsize = cinfo.output_width * cinfo.output_components;
	int bicount = cinfo.output_components * 8;
	//buff    = (unsigned char  *)malloc(pdst_jpginfo->rowsize * pdst_jpginfo->height);

	unsigned char *src_buff = (unsigned char *)malloc(rowsize);

    int *base = plcdinfo->base + plcdinfo->width*y + x;

	while(cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &src_buff, 1);
			
		//将RGB储存顺序
		for(int cols = 0; cols < cinfo.output_width - 1; cols++)
		{
			*(base + cols) = (src_buff[cols*3 + 2] << 0) | (src_buff[cols*3 + 1] << 8) | (src_buff[cols*3] << 16);
		}

		base += plcdinfo->width;

	}

	free(src_buff);

	//完成解压
	jpeg_finish_decompress(&cinfo);
	
	//销毁解压对象
	jpeg_destroy_decompress(&cinfo);

	return 0;







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