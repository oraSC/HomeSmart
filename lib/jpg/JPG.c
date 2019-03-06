#include "JPG.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jpeglib.h"
#include "../bmp/BMP.h"

pJpgInfo_t decompress_jpg2buffer(pJpgInfo_t pdst_jpginfo, char *src_path)
{
	//打开jpg文件
	FILE *src_file = fopen(src_path, "rb");

	if(src_file == NULL)
	{
		perror("fail to open jpg file");
		return NULL;
	}
	//创建、初始化解码对象，错误处理
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr pub;
	
	// //绑定错误处理
	cinfo.err = jpeg_std_error(&pub);
	
	// //创建解码对象
	jpeg_create_decompress(&cinfo);

	//指定jpg源文件
	jpeg_stdio_src(&cinfo, src_file);

	//读取jpg文件头(文件参数)
	jpeg_read_header(&cinfo, TRUE);

	//设置解压参数
	

	//开始解压
	jpeg_start_decompress(&cinfo);
	
	//读取参数
	pdst_jpginfo->width   = cinfo.output_width;
	pdst_jpginfo->height  = cinfo.output_height;
	pdst_jpginfo->rowsize = cinfo.output_width * cinfo.output_components;
	pdst_jpginfo->bicount = cinfo.output_components * 8;
	pdst_jpginfo->buff    = (unsigned char  *)malloc(pdst_jpginfo->rowsize * pdst_jpginfo->height);

	unsigned char *src_buff = (unsigned char *)malloc(pdst_jpginfo->rowsize);
	unsigned char *row_buff = pdst_jpginfo->buff;

	while(cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &src_buff, 1);
			
		//将RGB储存顺序转换成BGR储存顺序
		for(int cols = 0; cols < cinfo.output_width - 1; cols++)
		{
			*(row_buff + cols*3)     = src_buff[cols*3 + 2];
			*(row_buff + cols*3 + 1) = src_buff[cols*3 + 1];
			*(row_buff + cols*3 + 2) = src_buff[cols*3];
				
		}

		row_buff += pdst_jpginfo->rowsize;

		//printf("%d\t",rows*rowsize);	
		//printf("len :%d\n",strlen(dst_buff));	
	}

	free(src_buff);

	fclose(src_file);
	

	return pdst_jpginfo;


}


bool decompress_jpg2bmp(char *src_path, char *dst_path)
{

	//打开jpg文件
	FILE *src_file = fopen(src_path, "rb");

	if(src_file == NULL)
	{
		perror("fail to open jpg file");
		return -1;
	}
	//创建、初始化解码对象，错误处理
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr pub;
	
	// //绑定错误处理
	cinfo.err = jpeg_std_error(&pub);
	
	// //创建解码对象
	jpeg_create_decompress(&cinfo);

	//指定jpg源文件
	jpeg_stdio_src(&cinfo, src_file);

	//读取jpg文件头(文件参数)
	jpeg_read_header(&cinfo, TRUE);

	//设置解压参数
	

	//开始解压
	jpeg_start_decompress(&cinfo);
	
	//读取参数
	
	// //计算一行数据大小(有效数据)
	long row_stride = cinfo.output_width * cinfo.output_components;
	unsigned char *src_buff = malloc(row_stride);
	

	//创建、初始化bmp文件头
	BmpHead_t bmp_head;
	pBmpHead_t pbmp_head = &bmp_head;

	init_BmpHead(pbmp_head);
	pbmp_head->biWidth     = cinfo.output_width;
	pbmp_head->biHeight    = cinfo.output_height;
	// //bmp图片一行数据大小（包括补齐数据）
	long rowsize = pbmp_head->biWidth * 3 + pbmp_head->biWidth % 4;
	pbmp_head->biSizeImage = rowsize * pbmp_head->biHeight;
	pbmp_head->bfSize = pbmp_head->biSizeImage + 54;

	//写入bmp头
	FILE *dst_file = fopen(dst_path, "w");
	if(dst_file == NULL)
	{
		perror("fail to open bmp file");
		return -1;
	}

	fwrite(pbmp_head, 54, 1, dst_file);

	//读取解压数据、写入数据
	unsigned char *dst_buff = malloc(rowsize);
	memset(dst_buff, 0 ,rowsize);
	
	//初始化图片
	for(int rows = 0; rows < cinfo.output_height; rows++)
	{
		fwrite(dst_buff, rowsize, 1, dst_file);
	}

	while(cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &src_buff, 1);
		
		//将RGB储存顺序转换成BGR储存顺序
		for(int cols = 0; cols < cinfo.output_width - 1; cols++)
		{
			dst_buff[cols*3]     = src_buff[cols*3 + 2];
			dst_buff[cols*3 + 1] = src_buff[cols*3 + 1];
			dst_buff[cols*3 + 2] = src_buff[cols*3];
				
		}
		//解决图片倒置		
		int pos = (-1) * rowsize * (cinfo.output_scanline + 1);
		
		fseek(dst_file, pos, SEEK_END);
		fwrite(dst_buff, rowsize, 1, dst_file);

	}

	free(src_buff);
	free(dst_buff);
	fclose(src_file);
	fclose(dst_file);	

	return 0;
}

int jpg_resize(pJpgInfo_t src_pjpginfo, pJpgInfo_t dst_pjpginfo, int width, int height)
{


	//定义dst_pjpginfo
	dst_pjpginfo->width = width;
	dst_pjpginfo->height = height;
	dst_pjpginfo->rowsize = width * 3;
	dst_pjpginfo->bicount = src_pjpginfo->bicount;


	//计算比值
	float ratio_width = (float)(100 * width) / (float)src_pjpginfo->width;
	float ratio_height = (float)(100 * height) / (float)src_pjpginfo->height;

	//申请空间
	dst_pjpginfo->buff = malloc(width * height * 3);
	if(dst_pjpginfo->buff == NULL)
	{
		perror("fail to malloc in jpg resize");
		return -1;
	}
	int last = 0;
	for(int rows = 0; rows < height; rows++)
	{
		for(int cols = 0; cols < width; cols++)
		{
			last = (int)(((float)(rows*100))/ratio_height)*src_pjpginfo->width * 3 + (int)((float)(cols*100)/ratio_width)*3;	
			//printf("%d\n", last);
			//if(last < src_pjpginfo->rowsize * src_pjpginfo->height)
			memcpy(dst_pjpginfo->buff + rows * dst_pjpginfo->rowsize + cols*3, src_pjpginfo->buff + last, 3);
		}
	
	
	}


}




pJpgInfo_t div_jpg(pJpgInfo_t pSrc_jpginfo,int COLS, int ROWS, pJpgInfo_t pdiv_jpginfo[])
{

	//计算分割后的长宽
	int div_width   = pSrc_jpginfo->width   / COLS;
	int div_height  = pSrc_jpginfo->height  / ROWS;
	int div_bicount = pSrc_jpginfo->bicount;  
	long div_rowsize = div_width * div_bicount / 8;
	

	for(int rows = 0; rows < ROWS; rows++)
	{
		for(int cols = 0; cols  < COLS; cols++)
		{
			
			int num = COLS * rows + cols;
			
			//为子jpg结构体分配空间
			pdiv_jpginfo[num] = malloc(sizeof(JpgInfo_t));
			
			pdiv_jpginfo[num]->width   = div_width;
			pdiv_jpginfo[num]->height  = div_height;
			pdiv_jpginfo[num]->bicount = div_bicount;
			pdiv_jpginfo[num]->rowsize = div_rowsize;
			pdiv_jpginfo[num]->buff    = malloc(div_rowsize * div_height);
			
			for(int div_rows = 0; div_rows < div_height; div_rows++)
			{
				memcpy(pdiv_jpginfo[num]->buff + div_rows * div_rowsize, pSrc_jpginfo->buff + (div_rows + rows * div_width) * pSrc_jpginfo->rowsize + cols * div_rowsize, div_rowsize);
			
			}

		}


	
	}







}


int select_decompress_jpg2buffer(pJpgInfo_t pdst_jpginfo, char *path, int x, int y, int width, int height)
{

	//加载原图
	JpgInfo_t src_jpginfo;
	decompress_jpg2buffer(&src_jpginfo, path);
	
	//判断区域是否合法
	if(x > src_jpginfo.width || y > src_jpginfo.height || (x + width) > src_jpginfo.width || (y + height) > src_jpginfo.height)
	{
		printf("select zone if outof src jpg\n");
		return -1;
	}
	
	//读取参数
	pdst_jpginfo->width   = width;
	pdst_jpginfo->height  = height;
	pdst_jpginfo->rowsize = width * (src_jpginfo.bicount / 8);
	pdst_jpginfo->bicount = src_jpginfo.bicount;
	pdst_jpginfo->buff    = (unsigned char  *)malloc(pdst_jpginfo->rowsize * pdst_jpginfo->height);
	//读取制定区域
	//起始点
	int s_index = x*(src_jpginfo.bicount / 8) + y*src_jpginfo.rowsize;
	for(int rows = 0; rows < height; rows++)
	{
	
		memcpy(pdst_jpginfo->buff + rows*pdst_jpginfo->rowsize, src_jpginfo.buff + s_index + rows*src_jpginfo.rowsize, pdst_jpginfo->rowsize);	
	
	}
		
	//释放资源
	free(src_jpginfo.buff);

	return 0;


}











