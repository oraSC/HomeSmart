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



pJpgInfo_t      decompress_jpg2buffer(pJpgInfo_t pdst_jpginfo, char *path);
bool 		decompress_jpg2bmp(char *src_path, char *dst_path);
int		jpg_resize(pJpgInfo_t pjpginfo, pJpgInfo_t dst_pjpginfo, int width, int height);

/*
bug:当ROWs > COLS 出错
*/

pJpgInfo_t	div_jpg(pJpgInfo_t pSrc_jpginfo, int COLS, int ROWS, pJpgInfo_t pdiv_jpginfo[]);
int		select_decompress_jpg2buffer(pJpgInfo_t pdst_jpginfo, char *path, int x, int y, int width, int height);



#endif

