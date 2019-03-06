#include <stdio.h>
#include "./lib/jpg/JPG.h"
#include <unistd.h>
#include <stdlib.h>


int main()
{
	
	JpgInfo_t jpginfo;

	while(1)
	{
		decompress_jpg2buffer(&jpginfo, "./image/desktop/bg.jpg");
	
		free(jpginfo.buff);
		sleep(1);
	
	}




	return 0;
}
