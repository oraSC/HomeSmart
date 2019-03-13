#ifndef __YUYV_H__
#define __YUYV_H__
#include "../jpg/JPG.h"



 //参数 设备文件的路径  
int linux_v4l2_yuyv_init(char *dev_name);
//开始摄像头头捕捉画面
int linux_v4l2_start_yuyv_capturing();
//读取一帧的内容  获取摄像头采集数据
int   linux_v4l2_get_yuyv_data (struct jpg_data * jpg_buf);
//退出摄像头
int linux_v4l2_yuyv_quit();

#endif