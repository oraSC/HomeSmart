#include <stdio.h>
#include <unistd.h>


int main()
{

	printf("here is 2.c\n");
	
	int time = 10;
	while(1)
	{
		printf("2 will  exit after %d\n", time--);
		sleep(1);
		if(time == 0)
		{
			break;
		}
	}

	return 0;
}
