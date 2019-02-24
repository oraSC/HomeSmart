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
	pdst_jpginfo->buff    = malloc(pdst_jpginfo->rowsize * pdst_jpginfo->height);


	unsigned char *src_buff = malloc(pdst_jpginfo->rowsize);
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
















