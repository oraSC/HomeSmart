#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "camera.h"
#include "../lib/camera/yuyv.h"
#include "../lib/jpg/JPG.h"
#include "../lib/lcd/LCD.h"
#include "../lib/libjpeg/jpeglib.h"

#define TAKE_BTN_x      680
#define TAKE_BTN_y      196
#define SIMPLIFY_x      660
#define SIMPLIFY_y      20

/*
*功能：解压jpgddata并在lcd中显示
*返回值：
*   成功：0
*   失败：-1
*/
int decompress_jpgdataAndshow(pLcdInfo_t plcdinfo, int x, int y, pJpgData_t pjpgdata);
/*
*功能：查看刚拍的照片
*返回值：
*   成功：0
*   失败：-1
*/
static int check_photo(pLcdInfo_t plcdinfo, unsigned char *path, pJpgInfo_t bg_pjpginfo);


int camera(pLcdInfo_t plcdinfo, struct point *pts_point, struct Command *pcommand)
{
      int ret;


    //加载背景
    pJpgInfo_t bg_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(bg_pjpginfo == NULL)
    {
        perror("failee to malloc for bg_pjpginfo");
        return -1;
    }
    decompress_jpg2buffer(bg_pjpginfo, "./image/camera/bg.jpg");
    draw_pic(plcdinfo, 0, 0, bg_pjpginfo);

    //加载按键
    pJpgInfo_t take_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(take_pjpginfo == NULL)
    {
        perror("failed to malloc for take_pjpginfo");
        //释放资源
        free(bg_pjpginfo->buff);
        free(bg_pjpginfo);
        return -1;
    }
    decompress_jpg2buffer(take_pjpginfo, "./image/camera/take.jpg");

    //
    pJpgInfo_t simplify_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
    if(simplify_pjpginfo == NULL)
    {
        perror("failed to malloc for take_pjpginfo");
        //释放资源
        free(bg_pjpginfo->buff);
        free(bg_pjpginfo);
        return -1;
    }
    decompress_jpg2buffer(simplify_pjpginfo, "./image/camera/simplify.jpg");

    //创建按键链表
    //添加 take、 simplify 按键
    pBtn_SqList_t phead = create_btn_sqlist();
    pBtn_SqList_t node = draw_btn(plcdinfo, TAKE_BTN_x, TAKE_BTN_y, take_pjpginfo);
    AddFromTail_btn_sqlist(phead, node);
    pBtn_SqList_t node1 = draw_btn(plcdinfo, SIMPLIFY_x, SIMPLIFY_y, simplify_pjpginfo);
    AddFromTail_btn_sqlist(phead, node1);
  
    //捕获的jpg数据
    JpgData_t jpgdata;

    //初始化摄像头
    linux_v4l2_yuyv_init("/dev/video7");

    //开启摄像头捕捉
    linux_v4l2_start_yuyv_capturing();

    JpgInfo_t jpginfo;
    
    int have_took = 0;
    while(1)
    {
        //获取数据
        linux_v4l2_get_yuyv_data(&jpgdata);

        //转换成jpginfo
        decompress_jpgdataAndshow(plcdinfo, 0, 0,  &jpgdata);

        if(pts_point->update == true)
        {
            //
            pts_point->update = false;
            
            int click = find_which_btn_click(phead, pts_point->X, pts_point->Y);
            if(click == 1)
            {
   
                unsigned char path[10] = {0};
                sprintf(path, "./album/camera/%d.jpg", have_took);
                ret = create_jpgbyjpgdata(path, &jpgdata);
                if(ret != 0)
                {
                    printf("failed to take a picture\n");
                }

                else printf("succeed to take a picture\n");
                have_took++;

                //读取照片，制作简缩图
                JpgInfo_t photo_jpginfo;
                JpgInfo_t resize_photo_jpginfo;
                decompress_jpg2buffer(&photo_jpginfo, path);
                jpg_resize(&photo_jpginfo, &resize_photo_jpginfo, 128, 96);
                draw_pic(plcdinfo, SIMPLIFY_x, SIMPLIFY_y, &resize_photo_jpginfo);
                //释放资源
                free(photo_jpginfo.buff);
                free(resize_photo_jpginfo.buff);

            }
            else if(click == 2)
            {
                //已经有拍照
                if(have_took != 0)
                {
                    unsigned char path[10] = {0};
                    sprintf(path, "./album/camera/%d.jpg", have_took - 1);
                    
                    //查看照片
                    check_photo(plcdinfo, path, bg_pjpginfo);

                }


            }

        }


    }




    return 0;
}

int check_photo(pLcdInfo_t plcdinfo, unsigned char *path, pJpgInfo_t bg_pjpginfo)
{
    //读取照片
    JpgInfo_t photo_jpginfo;
    decompress_jpg2buffer(&photo_jpginfo, path);
    draw_pic(plcdinfo, 0, 0, bg_pjpginfo);
    middle_show(plcdinfo, 0, 0, 800, 480, &photo_jpginfo);
    while(1);



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