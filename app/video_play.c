#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "./video_play.h"
#include "../lib/jpg/JPG.h"

#define HIDETIME        3000

//声明全局错误码
extern int errno;

int video_play(pLcdInfo_t plcdinfo, pPoint_t pts_point)
{
    int ret;

    //挂载目录下出现权限不足问题
    //创建有名管道进行控制 fifo
    ret = mkfifo("./tmp/mplayerFifo", O_CREAT | 0777);
    if(ret < 0)
    {
        //如果错误为"file exit",继续执行
        if(strcmp(strerror(errno), "File exits"))	
        {
            //continue;
        }
        else 
        {
            perror("fail to create mplayer fifo");
            return -1;
        }
    }
    

    int mplayerFifoFd = open("./tmp/mplayerFifo", O_RDWR | O_NONBLOCK);
    if(mplayerFifoFd < 0)
    {
        perror("fail to open mplayer fifo");
        return -1;
    }
    //指定为控制有名管道
    system("mplayer  -slave -quiet -input file=./tmp/mplayerFifo ./video/11.avi &");
    write(mplayerFifoFd, "pause\n", strlen("pause\n"));

    //加载图片
    pJpgInfo_t exit_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(exit_pjpginfo == NULL)
    {
        perror("fail to malloc for exit_pjpginfo");
        return -1;
    }
    loadBmp2buffer(exit_pjpginfo, "./image/video_play/exit.bmp");

    pJpgInfo_t pause_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(pause_pjpginfo == NULL)
    {
        perror("fail to malloc for pause_pjpginfo");
        return -1;
    }
    loadBmp2buffer(pause_pjpginfo, "./image/video_play/pause.bmp");

    pJpgInfo_t continue_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(continue_pjpginfo == NULL)
    {
        perror("fail to malloc for continue_pjpginfo");
        return -1;
    }
    loadBmp2buffer(continue_pjpginfo, "./image/video_play/continue.bmp");

    pJpgInfo_t forward_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(forward_pjpginfo == NULL)
    {
        perror("fail to malloc for forward_pjpginfo");
        return -1;
    }
    loadBmp2buffer(forward_pjpginfo, "./image/video_play/forward.bmp");

    pJpgInfo_t backward_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(backward_pjpginfo == NULL)
    {
        perror("fail to malloc for backward_pjpginfo");
        return -1;
    }
    loadBmp2buffer(backward_pjpginfo, "./image/video_play/backward.bmp");

    pJpgInfo_t notmute_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(notmute_pjpginfo == NULL)
    {
        perror("fail to malloc for notmute_pjpginfo");
        return -1;
    }
    loadBmp2buffer(notmute_pjpginfo, "./image/video_play/notmute.bmp");

    pJpgInfo_t mute_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(mute_pjpginfo == NULL)
    {
        perror("fail to malloc for mute_pjpginfo");
        return -1;
    }
    loadBmp2buffer(mute_pjpginfo, "./image/video_play/mute.bmp");


    //创建按键链表
    pBtn_SqList_t head = create_btn_sqlist();
    pBtn_SqList_t node = draw_btn_onlyAcolor( plcdinfo, 10, 20, exit_pjpginfo, 0x00000000);
    AddFromTail_btn_sqlist(head, node);
    node = draw_btn_onlyAcolor(plcdinfo, 350, 190, pause_pjpginfo, 0x00000000);
    AddFromTail_btn_sqlist(head, node);
    node = draw_btn_onlyAcolor(plcdinfo, 500, 190, forward_pjpginfo, 0x00000000);
    AddFromTail_btn_sqlist(head, node);
    node = draw_btn_onlyAcolor(plcdinfo, 200, 190, pause_pjpginfo, 0x00000000);
    AddFromTail_btn_sqlist(head, node);
    node = draw_btn_onlyAcolor(plcdinfo, 730, 140, notmute_pjpginfo, 0x00000000);
    AddFromTail_btn_sqlist(head, node);

    //初始化为暂停
    pJpgInfo_t pauseOrNot_pjpginfo = pause_pjpginfo;
    pJpgInfo_t muteOrNot_pjpginfo = notmute_pjpginfo;
    int pause_num = 0;
    int mute_num = 0;

    //计数5000次隐藏图标
    int keyIsHide = HIDETIME;
    while(1)
    {
        if(pts_point->update == true)
        {
            pts_point->update = false;

            //按键隐藏
            if(keyIsHide == 0)
            {    
                //重新加载按键
                node = draw_btn_onlyAcolor( plcdinfo, 10, 20, exit_pjpginfo, 0x00000000);
                AddFromTail_btn_sqlist(head, node);
                node = draw_btn_onlyAcolor(plcdinfo, 350, 190, pauseOrNot_pjpginfo, 0x00000000);
                AddFromTail_btn_sqlist(head, node);
                node = draw_btn_onlyAcolor(plcdinfo, 500, 190, forward_pjpginfo, 0x00000000);
                AddFromTail_btn_sqlist(head, node);
                node = draw_btn_onlyAcolor(plcdinfo, 200, 190, backward_pjpginfo, 0x00000000);
                AddFromTail_btn_sqlist(head, node);
                node = draw_btn_onlyAcolor(plcdinfo, 730, 140, muteOrNot_pjpginfo, 0x00000000);
                AddFromTail_btn_sqlist(head, node);
                //重新加载“延时”
                keyIsHide = HIDETIME;
                //此次按下无效
                continue;

            }
            //按键未隐藏
            else 
            {
                //重新加载“延时”
                keyIsHide = HIDETIME;
            }
            int click = find_which_btn_click(head, pts_point->X, pts_point->Y);
            if(click > 0)
            {
                if(click == 1)
                {
                    //system("killall -KILL mplayer");
                    write(mplayerFifoFd, "quit\n", strlen("quit\n"));
                    return 0;
                }
                else if(click == 2)
                {
                    pause_num++;
                    if(pause_num % 2 == 0)
                    {
                        pause_num = 0;
                        pauseOrNot_pjpginfo = pause_pjpginfo;
                    }
                    else 
                    {
                        pause_num = 1;
                        pauseOrNot_pjpginfo = continue_pjpginfo;
                    }
                    write(mplayerFifoFd, "pause\n", strlen("pause\n"));
                }
                /*
                *bug：快进快退、切换静音会播放视频
                */
                else if(click == 3)
                {
                    pause_num = 1;
                    pauseOrNot_pjpginfo = continue_pjpginfo;
                    write(mplayerFifoFd, "seek 10\n", strlen("seek 10\n"));
                    
                }
                else if(click == 4)
                {
                    pause_num = 1;
                    pauseOrNot_pjpginfo = continue_pjpginfo;
                    write(mplayerFifoFd, "seek -10\n", strlen("seek -10\n"));
                    
                }
                else if(click == 5)
                {
                    mute_num++;
                    if(mute_num % 2 == 0)
                    {
                        mute_num = 0;
                        muteOrNot_pjpginfo = notmute_pjpginfo;
                    }
                    else 
                    {
                        mute_num = 1;
                        muteOrNot_pjpginfo = mute_pjpginfo;
                    }
                    write(mplayerFifoFd, "mute\n", strlen("mute\n"));
                    pause_num = 1;
                    pauseOrNot_pjpginfo = continue_pjpginfo;
                }

                

            }

        }
        if(keyIsHide > 0)
        {
            keyIsHide--;
            draw_pic_onlyAcolor(plcdinfo, 10, 20, exit_pjpginfo, 0x00000000);
            draw_pic_onlyAcolor(plcdinfo, 350, 190, pauseOrNot_pjpginfo, 0x00000000);
            draw_pic_onlyAcolor(plcdinfo, 500, 190, forward_pjpginfo, 0x00000000);
            draw_pic_onlyAcolor(plcdinfo, 200, 190, backward_pjpginfo, 0x00000000);
            draw_pic_onlyAcolor(plcdinfo, 730, 140, muteOrNot_pjpginfo, 0x00000000);
        }
        //图标隐藏
        else if(keyIsHide == 0)
        {
            //销毁按键
            clear_btn_sqlist(&head);
            
        }

    }

    /*
    *backlog:释放资源
    */







}




