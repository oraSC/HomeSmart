#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include "./mysocket.h"
#include <stdlib.h>

int server_create(int s_port, unsigned char *s_ip, int *c_port, unsigned char **c_ip)
{
	
	int ret;
	
	//创建socket(套接字)
	int soc_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(soc_fd < 0)
	{
		perror("fail to create socket");
		return -1;
	}

	//绑定套接字与网络地址
	//1.初始化服务器 IPv4 地址结构体
	struct sockaddr_in server_addr;
	int server_addr_len = sizeof(server_addr);
	bzero(&server_addr, server_addr_len);

	server_addr.sin_family 	= AF_INET;
	server_addr.sin_port	= htons(s_port);
	
	if(s_ip == NULL)
	{
		//自动获取本机所有ip,选取在使用ip
		htonl(INADDR_ANY);
	}
	else
	{
		server_addr.sin_addr.s_addr = inet_addr(s_ip);
	}

	//2.绑定
	ret = bind(soc_fd, (struct sockaddr *)&server_addr, server_addr_len);
	if(ret < 0)
	{
		perror("fail to bind socket");
		goto error;
	}
	
	//将待链接套接字设置为监听套接字
	ret = listen(soc_fd, 5);
	if(ret < 0)
	{
		perror("error exits in listen");
		goto error;
	}

	printf("server is waiting for connection...\n");

	//等待对端连接请求
	//1.声明变量存储对端信息
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	bzero(&client_addr, client_addr_len);

	int acc_fd = accept(soc_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if(acc_fd < 0)
	{
		perror("error exits when accept client connect");
		goto error;
	}


	int ret1, ret2;

	int soc_fd1 = socket(AF_INET, SOCK_STREAM, 0);
	if(soc_fd < 0)
	{
		perror("error");
		return -1;
	}

	//设置缓存区大小
	int recv_buff_size = 1024 ;
	int recv_buff_len = 4;
	ret1 = setsockopt(soc_fd1, SOL_SOCKET, SO_RCVBUF, (char *)&recv_buff_size, recv_buff_len);
	if(ret1 < 0)
	{
		perror("error");
		return -1;
	}
	
	recv_buff_size = 0;
	recv_buff_len = 0;
	//recv缓存区大小
	ret1 = getsockopt(soc_fd1, SOL_SOCKET, SO_RCVBUF, (char *)&recv_buff_size, &recv_buff_len);
	if(ret1 < 0)
	{
		perror("error");
		return -1;
	}
	printf("recv buff size:%d, len:%d\n",recv_buff_size, recv_buff_len);
	
	//send缓存区大小
	int send_buff_size = 0;
	int send_buff_len = 0;
	ret2 = getsockopt(soc_fd1, SOL_SOCKET, SO_SNDBUF, &send_buff_size, &send_buff_len);
	if(ret2 < 0)
	{
		perror("error:");
		return -1;
	}
	printf("send buff size:%d, len:%d\n", send_buff_size, send_buff_len);





	//客户端端口、地址
	if(c_port != NULL)
	{
		*c_port = ntohs(client_addr.sin_port);
	}
	if(c_ip != NULL)
	{

		//inet_ntoa返回的是什么内存的变量,需不需要free
		*c_ip = inet_ntoa(client_addr.sin_addr);
		

	}

	//printf("connecting with client.\nip: %s, port: %hd\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	//关闭监听套接字
	shutdown(soc_fd, SHUT_RDWR);

	return acc_fd;

error:
	shutdown(soc_fd, SHUT_RDWR);
	return -1;
}


int client_create(int s_port, unsigned char *s_ip)
{
	
	int ret;
	
	//创建socket(套接字)
	int soc_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(soc_fd < 0)
	{
		perror("fail to create socket");
		return -1;
	}

	//定义服务器网络地址
	struct sockaddr_in server_addr;
	int server_addr_len = sizeof(server_addr);
	bzero(&server_addr, server_addr_len);

	server_addr.sin_family 	= AF_INET;
	server_addr.sin_port	= htons(s_port);
	server_addr.sin_addr.s_addr	= inet_addr(s_ip);
	
	ret = connect(soc_fd, (struct sockaddr *)&server_addr, server_addr_len);
	if(ret < 0)
	{
		perror("fail to connect with server");
		shutdown(soc_fd, SHUT_RDWR);
		return -1;
	}

	return soc_fd;

}
	
