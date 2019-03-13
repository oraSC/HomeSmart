#ifndef _MYSOCKET_H_
#define _MYSOCKET_H_

/*
 *功能：创建socket点对点服务器(ipv4地址类型)
 *返回值：
 *	成功：已连接套接字描述符
 *	错误：-1
 */

int server_create(int s_port, unsigned char *s_ip, int *c_port, unsigned char **c_ip);

/*
 *功能：创建socket点对点客户端(Ipv44地址类型)
 *返回值：
 *	成功：已连接套接字描述符
 *	失败：-1
 */
int client_create(int s_port, unsigned char *s_ip);

/*
 *功能：将待连接套接字设置为监听套接字
 *返回值：
 *	成功：0
 *	失败：-1
 */
int soc_server_init(int *psoc, unsigned char *s_ip, int s_port);

/*
*功能：具有等待接收完成的单次发送函数
*返回值：
*		成功：0
*		失败：-1
*/
int Send_andwait(int sockfd, const void *buff, size_t len, int flags);

/*
*功能：具有回复接收完成的单次接收函数
*返回值：
*		成功：0
*		失败：-1
*/
ssize_t Recv_andreply(int sockfd, void *buff, size_t len, int flags);









#endif

