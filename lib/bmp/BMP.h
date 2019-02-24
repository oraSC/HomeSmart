#ifndef _BMP_H_
#define _BMP_H_

/*
@author:czw

*/
#pragma pack(1)
typedef struct BmpHead{
	//文件头信息
	short bfType;           //固定为0x4d42
	int   bfSize;           //文件大小
	short bfReserved1;      //保留字，不考虑
	short bfReserved2;      //保留字，不考虑
	int   bfOffBit;         //实际位图数据的偏移字数

	//文件位图信息
	int   biSize;	   	//指定此结构体的长度，40？？？？
	int   biWidth;		//位图宽
	int   biHeight;		//位图高
	short biPlanes;		//平面数
	short biBitCount;	//采用的颜色位数
	int   biCompression;	//压缩方式，0表示不压缩
	int   biSizeImage;	//实际位图数据采用的字节数
	int   biXPelsPerMeter;	//X方向分辨率
	int   biYPelsPerMeter;  //Y方向的分辨率
	int   biClrUsed;	//使用的颜色数
	int   biClrImportant;	//重要的颜色数，0表示所有颜色均重要


} BmpHead_t, *pBmpHead_t;
#pragma pack()

void describ_BmpHead(pBmpHead_t pbmphead);
void init_BmpHead(pBmpHead_t pbmphead);



#endif





