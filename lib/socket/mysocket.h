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










#endif

