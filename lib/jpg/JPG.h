#ifndef _JPG_H_
#define _JPG_H_

#include <stdbool.h>

typedef struct{ 
	unsigned char *buff;	//图像像素数据指针
	int width;		//图像宽度
	int height;		//图像高度
	long rowsize;		//一行的像素大小
	int bicount;		//图像色深：24、32 



}JpgInfo_t, *pJpgInfo_t;


typedef struct jpg_data
{
	unsigned char data[614400];   //用于存放摄像头数据640*480
	int size;  //用于存放摄像头数据大小
}JpgData_t, *pJpgData_t ;



pJpgInfo_t      decompress_jpg2buffer(pJpgInfo_t pdst_jpginfo, char *path);
int			loadBmp2buffer(pJpgInfo_t pdst_jpinfo, char *path);
bool 		decompress_jpg2bmp(char *src_path, char *dst_path);
int 		decompress_jpgdata2buffer(unsigned char *pjpgdata, int datasize, pJpgInfo_t pdst_jpginfo);
int			create_jpgbyjpgdata(unsigned char *path, pJpgData_t pjpgdata);
int			jpg_resize(pJpgInfo_t pjpginfo, pJpgInfo_t dst_pjpginfo, int width, int height);

/*
bug:当ROWs > COLS 出错
*/

pJpgInfo_t	div_jpg(pJpgInfo_t pSrc_jpginfo, int COLS, int ROWS, pJpgInfo_t pdiv_jpginfo[]);
int		select_decompress_jpg2buffer(pJpgInfo_t pdst_jpginfo, char *path, int x, int y, int width, int height);



#endif

