#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <strings.h>
#include <string.h>
#include "serial.h"

/* 
int main()
{
	//1、打开文件
	int tty_fd1 = open("/dev/ttySAC1", O_RDWR | O_NOCTTY);
	if(tty_fd1 < 0)
	{
		
		perror("open ttySAC1 failed1\n");
		return -1;
		
	}
	
	
	//1.1配置串口属性
	serial_setattr(tty_fd1);
	

	int ret;
	//2、读和写-->接收和发送数据
	while(1)
	{			
		ret = send_A(tty_fd1);
		if(ret < 0)
		{
			return -1;
		}
		else if(ret >= 1)
		{
			continue;
		}
		
		else if(ret == 0)
		{
			int id;
			id = send_B(tty_fd1);
			if(id == 0)
			{
				continue;
			
			}
			else if(id > 0)
			{
				printf("id: 0x%08x\n", id);
				
			}


		}

	}
	
	
	
	//3、关闭文件
	close(tty_fd1);
	

		
			return 0;
}*/







//计算校验和
unsigned char calculate_BCC(unsigned char *buff)
{
	unsigned char BCC=0;
	int i;
	for(i=0; i<buff[0]-2; i++)
	{			
		BCC ^= buff[i];
	}
	
	return (~BCC); 
	
}

struct termios * serial_setattr(int fd)
{
	
	struct termios old_cfg, new_cfg;
	bzero(&old_cfg, sizeof(old_cfg));
	bzero(&new_cfg, sizeof(old_cfg));
	
	//1、保存串口原先属性
	tcgetattr(fd, &old_cfg);
		
		
	//2、激活本地连接和接收使能			
	 new_cfg.c_cflag  |=  CLOCAL | CREAD; 
	
	//3、设置波特率
	cfsetispeed(&new_cfg, B9600);  //B9600
	cfsetospeed(&new_cfg, B9600);
	
	//4．设置数据位大小
	new_cfg.c_cflag &= ~CSIZE; /* 用数据位掩码清空数据位设置 */
	new_cfg.c_cflag |= CS8;  //CS5,CS6
	
	//5、设置无检验
	new_cfg.c_cflag &= ~PARENB;
	
	//6、设置停止位
	new_cfg.c_cflag &= ~CSTOPB; /* 将停止位设置为一个比特 */
	//new_cfg.c_cflag |= CSTOPB; /* 将停止位设置为两个比特 */

	//7．设置最少字符和等待时间
	new_cfg.c_cc[VTIME] = 0;
	new_cfg.c_cc[VMIN]  = 1;
	
	//8、清空串口缓冲区
	tcflush(fd, TCIOFLUSH);
	
	//9、让配置立即生效
	tcsetattr(fd, TCSANOW, &new_cfg);
}




int send_A(int fd)
{

	int ret;

	unsigned char buf[8];
	bzero(buf, 8);
	
	//A命令发送buff
	buf[0] = 0x07;//帧长
	buf[1] = 0x02;//命令类型ISO14443A 类命令
	buf[2] = 0x41;//选择'A'命令
	buf[3] = 0x01;//信息长度
	buf[4] = 0x52;//信息内容
	buf[5] = calculate_BCC(buf); //获取校验和
	buf[6] = 0x03; //结束符
	
	//A命令接收buff
	unsigned char rbuf[10];
	bzero(rbuf, 10);
	
	//发送cmdA
	ret = write(fd, buf, buf[0]);
	if(ret < 0)
	{
		perror("error exists when write in send_A");
		return -1;
	}
	ret = read(fd, rbuf, sizeof(rbuf));
	if(ret < 0)
	{
		perror("error exists when read in send_A");
		return -1;
	}
	
	//检测状态值
	if(rbuf[2] == 0)
	{
		return 0;
	
	}
	else 
	{
		return 1;
	}

}



unsigned int send_B(int fd)
{
	
	int ret;

	//B命令发送buff
	unsigned char Bbuff[9] = {0};
	Bbuff[0] = 0x08;
	Bbuff[1] = 0x02;
	Bbuff[2] = 0x42;
	Bbuff[3] = 0x02;
	Bbuff[4] = 0x93;
	Bbuff[5] = 0x00;
	Bbuff[6] = calculate_BCC(Bbuff);
	Bbuff[7] = 0x03;
	
	//B命令接受
	unsigned char Brbuff[11] = {0};
		
	ret = write(fd, Bbuff, Bbuff[0]);
	if(ret < 0)
	{
		perror("error exists when write in send_B");
		return -1;
	}
	ret = read(fd, Brbuff, sizeof(Brbuff));
	if(ret < 0)
	{
		perror("error exist when read in send_B");
		return -1;
	}
	//检测是否获取到id
	if(Brbuff[2] == 0)
	{
		unsigned int id;
		//低位在前，进行高低位转化
		id = (Brbuff[7]<<24) | (Brbuff[6]<<16) | (Brbuff[5]<<8) | (Brbuff[4]);
		return id;
	}

	return 0;


}

