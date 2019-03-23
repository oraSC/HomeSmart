/*record:采用如下通话模型：
*       主线程：call_routine(负责打电话)
*       子线程：accept_routine(负责接电话)
*通话流程：
*       打电话方(主动方)，连接接电话方(被动方)服务器，接电话方子线程通知主线程连接打电话方服务器，形成x型通话模型
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "chat.h"
#include "../lib/jpg/JPG.h"
#include "../lib/camera/yuyv.h"
#include "../lib/lcd/LCD.h"
#include "../lib/libjpeg/jpeglib.h"
#include "../lib/socket/mysocket.h"

//CMD
#define ANSWER_CMD              4
#define END_OF_CALL_CMD         5

//PORT
#define VIDEOCALL_PORT          4001
#define VOICECALL_PORT          4002

#define CHAT_STATE_INCALL       1
#define CHAT_STATE_RINGOFF      2
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
#define RECV_SINGLE_SIZE        15360

/*
*功能：解压jpgddata并在lcd中显示
*返回值：
*   成功：0
*   失败：-1
*/
int decompress_jpgdataAndshow(  pLcdInfo_t plcdinfo, int x, int y, 
                                pJpgData_t pjpgdata);
static bool		draw_pic_notAcolor(pLcdInfo_t plcdinfo, int x, int y, pJpgInfo_t pjpginfo, int color);
/*
*功能：chat初始化 加载图片
*返回值：
*   成功：0
*   失败：-1
*/
static int chat_init_loadpic(   pJpgInfo_t *bg_pjpginfo, 
                                pJpgInfo_t *voiceCall_pjpginfo,
                                pJpgInfo_t *videoCall_pjpginfo,
                                pJpgInfo_t *ringOn_pjpginfo,
                                pJpgInfo_t *ringOff_pjpginfo,
                                pJpgInfo_t *exit_pjpginfo);
/*
*功能：准备接电话，可选择ringOn / ringOff
*返回值：CMD值
*/
static int ready_to_answer( pLcdInfo_t plcdinfo,
                            pPoint_t pts_point,
                            int *psocFd,
                            pBtn_SqList_t phead,
                            pJpgInfo_t bg_pjpginfo, 
                            pJpgInfo_t ringOn_pjpginfo,
                            pJpgInfo_t ringOff_pjpginfo
                            );
/*
*功能：通话中,接受功能
*返回值：CMD
*/
static int in_call_recv(pLcdInfo_t plcdinfo,
                        int socFd,
                        pJpgInfo_t ringOff_pjpginfo
                        );

struct wait_for_callme_arg{ pLcdInfo_t      plcdinfo;
                            pBtn_SqList_t   phead;
                            pPoint_t        pts_point;
                            pJpgInfo_t      bg_pjpginfo;
                            pJpgInfo_t      ringOn_pjpginfo;
                            pJpgInfo_t      ringOff_pjpginfo;
                            pJpgInfo_t      exit_jpginfo;

};
void *wait_for_callme(void *arg);
/*
*功能：发送语音线程功能函数
*/
void *voiceRecvRoutine(void *arg);

//通话状态
static int chat_state;
int chat(pLcdInfo_t plcdinfo, pPoint_t pts_point)
{
    
    int ret;

    pJpgInfo_t bg_pjpginfo; 
    pJpgInfo_t voiceCall_pjpginfo; 
    pJpgInfo_t videoCall_pjpginfo;
    pJpgInfo_t ringOn_pjpginfo;
    pJpgInfo_t ringOff_pjpginfo;
    pJpgInfo_t exit_pjpginfo;
    //初始化
    chat_init_loadpic(  &bg_pjpginfo, 
                        &voiceCall_pjpginfo,
                        &videoCall_pjpginfo,
                        &ringOn_pjpginfo,
                        &ringOff_pjpginfo,
                        &exit_pjpginfo);

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

    // //voice recv
    // pthread_t voiceRecvPthId;
    // pthread_create(&voiceRecvPthId, NULL, &voiceRecvRoutine, NULL);

    //捕获的jpg数据
    JpgData_t jpgdata;

    //初始化摄像头
    //linux_v4l2_yuyv_init("/dev/video7");

    //开启摄像头捕捉
    //linux_v4l2_start_yuyv_capturing();
    chat_state = CHAT_STATE_INCALL;
    JpgInfo_t jpginfo;
    while(1)
    {
        sleep(1);
        if(chat_state == CHAT_STATE_RINGOFF)
        {
            //break;
        }
        
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

    
    while(1)
    {
        int socFd;
        ret = ready_to_answer(  plcdinfo,
                                pts_point,
                                &socFd,
                                phead,
                                bg_pjpginfo, 
                                ringOn_pjpginfo,
                                ringOff_pjpginfo
                                );
        if(ret != ANSWER_CMD)
        {
            printf("what! not ANSWER_CMD\n");
            continue;
        }

        //in call recv
        ret = in_call_recv(plcdinfo,socFd,ringOff_pjpginfo);
        if(ret != END_OF_CALL_CMD)
        {   
            printf("what! not END_OF_CALL_CMD\n");
            continue;
        }
        chat_state = CHAT_STATE_RINGOFF;
        printf("stop to recv in chat\n");

    }


}


void *voiceRecvRoutine(void *arg)
{
    int ret;

    while(1)
    {
        //创建服务器
        int c_port;
        unsigned char *c_ip = NULL;
        int socFd = server_create(VOICECALL_PORT, NULL, &c_port, &c_ip);
        printf("\nsomebody connect your voice recv server\nip:%s\nport:%d\n", c_ip, c_port);
        
        //声明wav音频buffer
        char recvWavBuff[100000];
        while(1)
        {
            bzero(recvWavBuff, sizeof(recvWavBuff));
            //接受语音
            /************************ 1.接收音频data大小 ***************************/
            int totalSize;

            ret = Recv_andreply(socFd, &totalSize, sizeof(totalSize), 0);
            printf("recv size:%d\n", totalSize);
            /************************ 2.接收data ***************************/
            int rest_size = totalSize;
            int recv_num = totalSize / RECV_SINGLE_SIZE + 1;
            for(int i = 0; i < recv_num; i++)
            {
                if(rest_size > RECV_SINGLE_SIZE)
                {
                    ret = Recv_andreply(socFd, recvWavBuff + i*RECV_SINGLE_SIZE, RECV_SINGLE_SIZE, 0);
                }
                else if(rest_size != 0)
                {
                    ret = Recv_andreply(socFd, recvWavBuff + i*RECV_SINGLE_SIZE, rest_size, 0);

                }
                
                //判断对端是否下线
                if(ret == 0)
                {
                    break;
                }
                //修改剩余大小
                rest_size = rest_size - RECV_SINGLE_SIZE;
            }
            //判断对端是否下线
            if(ret == 0)
            {       
                    shutdown(socFd, SHUT_RDWR);
                    break;
            }
            //创建、写入recv.wav
            int recvWavFd = open("./tmp/recv.wav", O_RDWR | O_CREAT);
            if(recvWavFd < 0)
            {
                perror("fail to open ./tmp/recv.wav");

            }
            ret = write(recvWavFd, recvWavBuff, totalSize);
            if(ret < 0)
            {
                perror("fail to write recvWavBuff into ./tmp/recv.wav");
            }
            ret = close(recvWavFd);
            if(ret < 0)
            {
                perror("fail to close ./tmp/recv.wav");
            }

            //播放
            system("aplay ./tmp/recv.wav");

        }



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


static int chat_init_loadpic(   pJpgInfo_t *bg_pjpginfo, 
                                pJpgInfo_t *voiceCall_pjpginfo,
                                pJpgInfo_t *videoCall_pjpginfo,
                                pJpgInfo_t *ringOn_pjpginfo,
                                pJpgInfo_t *ringOff_pjpginfo,
                                pJpgInfo_t *exit_pjpginfo)
{

    //加载背景
    *bg_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for bg_pjpginfo");
        return -1;
    }

    decompress_jpg2buffer(*bg_pjpginfo, "./image/chat/bg.jpg");


    //加载 voiceCall、videoCall
    *voiceCall_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for voiceCall_pjpginfo");
        return -1;
    }
    decompress_jpg2buffer(*voiceCall_pjpginfo, "./image/chat/voiceCall.jpg");

    *videoCall_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for videoCall_pjpginfo");
        return -1;
    }
    decompress_jpg2buffer(*videoCall_pjpginfo, "./image/chat/videoCall.jpg");

    //加载 ring on/off 按键
    *ringOn_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for ringOn_pjpginfo");
        return -1;
    }

    decompress_jpg2buffer(*ringOn_pjpginfo, "./image/chat/ringOn.jpg");

    *ringOff_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for ringOff_pjpginfo");
        return -1;
    }

    decompress_jpg2buffer(*ringOff_pjpginfo, "./image/chat/ringOff.jpg");
    
    //加载 exit
    *exit_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("fail to malloc for exit_pjpginfo");
        return -1;
    }
    decompress_jpg2buffer(*exit_pjpginfo, "./image/chat/exit.jpg");






}


static int ready_to_answer( pLcdInfo_t plcdinfo,
                            pPoint_t pts_point,
                            int *psocFd,
                            pBtn_SqList_t phead,
                            pJpgInfo_t bg_pjpginfo, 
                            pJpgInfo_t ringOn_pjpginfo,
                            pJpgInfo_t ringOff_pjpginfo
                            )
{

    int ret;

    //创建服务器
    int c_port;
    unsigned char *c_ip = NULL;
    *psocFd = server_create(VIDEOCALL_PORT, NULL, &c_port, &c_ip);
    printf("\nsomebody connect your video recv server\nip:%s\nport:%d\n", c_ip, c_port);
    
    //加载有电话来界面
    clear_btn_sqlist(&phead);
    draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
    pBtn_SqList_t ringOnBtn = draw_btn(plcdinfo, RINGON_ANSWER_x, RINGON_ANSWER_y, ringOn_pjpginfo);
    AddFromTail_btn_sqlist(phead, ringOnBtn);
    pBtn_SqList_t ringOffBtn = draw_btn(plcdinfo, RINGOFF_ANSWER_x, RINGOFF_ANSWER_y, ringOff_pjpginfo);
    AddFromTail_btn_sqlist(phead, ringOffBtn);

    int click;
    //等待用户接电话
    while(1)
    {

        if(pts_point->update == true)
        {
            pts_point->update == false;
            click = find_which_btn_click(phead, pts_point->X, pts_point->Y);
            if(click <= 0)
            {
                continue;
            }
            //answer
            else if(click == 1)
            {
                ret = send(*psocFd, "ring on", strlen("ring on"), 0);
                if(ret < 0)
                {
                    perror("error exists in send");
                    return -1;
                }
                return ANSWER_CMD;
            }
            //ring off
            else if(click == 2)
            {
                ret = send(*psocFd, "ring off", strlen("ring off"), 0);
                if(ret < 0)
                {
                    perror("error exists in send");
                }
                shutdown(*psocFd, SHUT_RDWR);
                break;
            }
        }
    }
    

}


int in_call_recv( pLcdInfo_t plcdinfo,
                    int socFd,
                    pJpgInfo_t ringOff_pjpginfo
                    )
{

    int ret;

    JpgInfo_t recv_jpginfo;

    //通话
    JpgData_t recv_jpgdata;
    while(1)
    {
    
        //接受图像
        /*
        *bug:偶尔出现丢失某一端，造成图像数据整体上移(单次发送超过20480 Byte)
        */
        /************************ 1.接收图像data大小 ***************************/
        int totalSize;
        ret = Recv_andreply(socFd, &totalSize, sizeof(totalSize), 0);
        recv_jpgdata.size = totalSize;

        /************************ 2.接收图像data ***************************/
        int rest_size = totalSize;
        int recv_num = totalSize / RECV_SINGLE_SIZE + 1;
        for(int i = 0; i < recv_num; i++)
        {
            if(rest_size > RECV_SINGLE_SIZE)
            {
                ret = Recv_andreply(socFd, recv_jpgdata.data + i*RECV_SINGLE_SIZE, RECV_SINGLE_SIZE, 0);
            }
            else if(rest_size != 0)
            {
                ret = Recv_andreply(socFd, recv_jpgdata.data + i*RECV_SINGLE_SIZE, rest_size, 0);

            }
            
            //判断对端是否下线
            if(ret == 0)
            {
                return END_OF_CALL_CMD;
            }
            //修改剩余大小
            rest_size = rest_size - RECV_SINGLE_SIZE;
        }
        

        //解压成buff
        decompress_jpgdata2buffer(recv_jpgdata.data, recv_jpgdata.size, &recv_jpginfo);
        draw_pic(plcdinfo, 80, 0, &recv_jpginfo);

        draw_pic_notAcolor(plcdinfo, RINGOFF_INCALL_x, RINGOFF_INCALL_y, ringOff_pjpginfo, 0x00000000);

        free(recv_jpginfo.buff);
    }
}
    