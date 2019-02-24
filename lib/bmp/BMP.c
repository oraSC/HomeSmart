#include "BMP.h"
#include <stdio.h>




void describ_BmpHead(pBmpHead_t pbmphead)
{

	printf("bfType:%hd\n", 		pbmphead->bfType);
	printf("bfSize:%d\n",  		pbmphead->bfSize);
	printf("bfReserved1:%hd\n", 	pbmphead->bfReserved1);
	printf("bfReserved:%hd\n", 	pbmphead->bfReserved2);
	printf("bfOffBit:%hd\n", 	pbmphead->bfOffBit);

	printf("biSize:%d\n", 		pbmphead->biSize);
	printf("biWidth:%d\n",  	pbmphead->biWidth);
	printf("biHeight:%d\n", 	pbmphead->biHeight);
	printf("biPlanes:%hd\n", 	pbmphead->biPlanes);
	printf("biBitCount:%hd\n", 	pbmphead->biBitCount);
	printf("biCompression:%d\n", 	pbmphead->biCompression);
	printf("biSizeImage:%d\n",  	pbmphead->biSizeImage);
	printf("biXPelsPerMeter:%d\n", pbmphead->biXPelsPerMeter);
	printf("biYPelsPerMeter:%d\n",	pbmphead->biYPelsPerMeter);
	printf("biClrUsed:%d\n", 	pbmphead->biClrUsed);
	printf("biClrImportant:%d\n", 	pbmphead->biClrImportant);
	
}
void init_BmpHead(pBmpHead_t pbmphead)
{

	//文件头信息
	pbmphead->bfType = 0x4d42;      	//固定为0x4d42
	//pbmphead->bfSize = 0;            	//文件大小
	pbmphead->bfReserved1 = 0;      	//保留字，不考虑
	pbmphead->bfReserved2 = 0;       	//保留字，不考虑
	pbmphead->bfOffBit = 54;         	//实际位图数据的偏移字数

	//文件位图信息
	pbmphead->biSize = 40;	   		//指定此结构体的长度，40？？？？
	//pbmphead->biWidth = 0;		//位图宽
	//pbmphead->biHeight = 0;		//位图高
	pbmphead->biPlanes = 1;			//平面数
	pbmphead->biBitCount = 24;		//采用的颜色位数
	pbmphead->biCompression = 0;		//压缩方式，0表示不压缩
	//pbmphead->biSizeImage = 0;		//实际位图数据采用的字节数
	pbmphead->biXPelsPerMeter = 2834;	//X方向分辨率
	pbmphead->biYPelsPerMeter = 2834;  	//Y方向的分辨率
	pbmphead->biClrUsed = 0;		//使用的颜色数
	pbmphead->biClrImportant = 0;		//重要的颜色数，0表示所有颜色均重要


}


