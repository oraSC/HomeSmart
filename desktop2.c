#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#include "./lib/lcd/LCD.h"
#include "./lib/jpg/JPG.h"
#include <linux/input.h>
#include "./app/album.h"
#include "./lib/socket/mysocket.h"
#include "./app/music.h"
#include "./app/garage.h"
#include "./app/camera.h"
#include "./app/chat2.h"
#include "./lib/cJSON/cJSON.h"
#include "./lib/font/font.h"


#define ALBUM				1
#define MUSIC				2
#define	GARAGE				3
#define CAMERA				4
#define CHAT				5
#define EXIT				6

#define APP_START_x			30
#define APP_WIDTH			150
#define APP_HEIGHT			150
#define APP_START_y			100			
#define FIND_MAX_FD(x,y) 	(x)>(y)?(x):(y) 
#define APP_NUM 			6


struct point 	ts_point;
struct Command 	command;

void update_clients(char *message);
/*
*功能：监控触摸屏子线程
*/
void *ts_monitor(void *arg);
/*
*功能：socket通信服务器子线程
*/
void *remote_control(void *arg);
/*
*功能：桌面状态栏子线程
*/
void *statBarRoutine(void *arg);

//声明待添加集合列表
int soc_fds[10];
int soc_fds_len;
int max_fd;

//线程互斥锁、条件变量
pthread_mutex_t mutex;
pthread_cond_t	cond;
//状态互斥锁、条件变量
pthread_mutex_t statBar_mutex;
pthread_cond_t	statBar_cond;

//声明状态值(记录当前处于的功能)
char state[20];

int main()
{
	//初始化字库
	font_lib_init();

	//初始化多路复用待添加集合列表
	bzero(soc_fds, sizeof(soc_fds));
	soc_fds_len = 0;
	max_fd = -1;

	//初始化状态值
	strcpy(state, "desktop");

	int ret;

	char app_icon_name[APP_NUM][30] = {	"./image/desktop/album.jpg", 
										"./image/desktop/music.jpg", 
										"./image/desktop/garage.jpg",
										"./image/desktop/camera.jpg", 
										"./image/desktop/chat.jpg",
										"./image/desktop/exit.jpg"};

	//1.创建lcd
	pLcdInfo_t plcdinfo = (LcdInfo_t *)malloc(sizeof(LcdInfo_t));
	if(plcdinfo == NULL)
	{
		perror("fail to malloc lcd");
		return -1;
	}
	plcdinfo = lcd_create("/dev/fb0", plcdinfo);

	//2.打开背景图片
	pJpgInfo_t bg_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
	if(bg_pjpginfo == NULL)
	{
		perror("fail to malloc bg_pjpginfo");
		return -1;
	}
	decompress_jpg2buffer(bg_pjpginfo, "./image/desktop/bg.jpg");

	//3.加载背景
	draw_pic(plcdinfo, 0, 0, bg_pjpginfo);

	//*4.加载app图标
	//**创建按键链表头
	pBtn_SqList_t head = create_btn_sqlist();

	//**添加按键
	JpgInfo_t app_jpginfo[APP_NUM];
	for(int i = 0; i < APP_NUM; i++)
	{
		
		decompress_jpg2buffer(&app_jpginfo[i], app_icon_name[i]);
		pBtn_SqList_t newnode;
		if(i < 5)
		{
			newnode = draw_btn(plcdinfo, APP_WIDTH*(i) + APP_START_x, APP_START_y, &app_jpginfo[i]);
		}
		else 
		{
			newnode = draw_btn(plcdinfo, APP_WIDTH*(i - 5) + APP_START_x, APP_HEIGHT + APP_START_y, &app_jpginfo[i]);
		}
		AddFromTail_btn_sqlist(head, newnode);
		
	}
	
	//5.初始化互斥锁和条件变量、创建触摸屏监控和socket通讯子线程
	ret = pthread_mutex_init(&mutex, NULL);
	if(ret < 0)
	{
		perror("fail to init mutex");
		goto err;
	}
	
	ret = pthread_cond_init(&cond, NULL);
	if(ret < 0)
	{
		perror("fail to init cond");
		goto err;
	}

	ret = pthread_mutex_init(&statBar_mutex, NULL);
	if(ret < 0)
	{
		perror("fail to init mutex");
		goto err;
	}
	
	ret = pthread_cond_init(&statBar_cond, NULL);
	if(ret < 0)
	{
		perror("fail to init cond");
		goto err;
	}

	//触摸屏监控子线程
	pthread_t ts_pth_id;
	ret = pthread_create(&ts_pth_id, NULL, ts_monitor, NULL);
	if(ret < 0)
	{
		perror("fail to create ts_monitor pthread");
		goto err;
	}
	
	//远程控制服务器子线程
	pthread_t soc_pth_id;
	ret = pthread_create(&soc_pth_id, NULL, remote_control, NULL);
	if(ret < 0)
	{
		perror("fail to create remote control pthread");
		goto err;
	}

	//状态栏更新子线程
	pthread_t statBarPthId;
	ret = pthread_create(&statBarPthId, NULL, statBarRoutine, (void *)plcdinfo);
	if(ret < 0)
	{
		perror("fail to create statBar pthread");
		goto err;
	}


	//*6.读取按键
	int app_num = 0;

	while(1)
	{
		
		/************************上锁**************************************/
		pthread_mutex_lock(&mutex);
		
		while(ts_point.update != true && command.update != true)
		{
			//挂起等待
			pthread_cond_wait(&cond, &mutex);
		}
		
		//检测到用户动作
		if(ts_point.update == true)
		{
			ts_point.update = false;
			app_num = find_which_btn_click(head, ts_point.X, ts_point.Y);
		}
		else if(command.update == true)
		{
			command.update = false;
			app_num = command.ascii[0] - '0';
		}
		
		pthread_mutex_unlock(&mutex);
		/***************************解锁*************************************/
		if(app_num != 0)
		{
			if(app_num == EXIT)
			{
				break;

			}
			else if(app_num == CHAT)
			{
				strcpy(state, "chat");
				chat2(plcdinfo, &ts_point);
				strcpy(state, "desktop");
			}
			else if(app_num == CAMERA)
			{
				strcpy(state, "camera");
				camera(plcdinfo, &ts_point, &command);
				strcpy(state, "desktop");
			}

			//车库	
			else if(app_num == GARAGE)
			{
				strcpy(state, "garage");
				garage(plcdinfo, &ts_point);
				strcpy(state, "desktop");
			}

			else if(app_num == ALBUM)
			{
				update_clients("album");	
				album(plcdinfo, &ts_point, &command);
				update_clients("desktop");
			}
			else if(app_num == MUSIC)
			{
				update_clients("music");	
				music(plcdinfo, &ts_point, &command);
				update_clients("desktop");
			}
			
			//解锁状态栏
			pthread_cond_signal(&statBar_cond);

			//再次刷新桌面
			draw_pic(plcdinfo, 0, 0, bg_pjpginfo);

			/*
			*bug:不添加 printf(), 程序发生段错误
			*/
			printf("\n");
			for(int i = 0; i < APP_NUM; i++)
			{
				if(i < 5)
				{
					draw_pic(plcdinfo, APP_WIDTH*(i) + APP_START_x, APP_START_y, &app_jpginfo[i]);
				}
				else
				{
					draw_pic(plcdinfo, APP_WIDTH*(i - 5) + APP_START_x, APP_HEIGHT + APP_START_y, &app_jpginfo[i]);
				}
				
			}
						
		}
	
	}


	//销毁lcd
	ret = lcd_destroy(plcdinfo);

	//释放背景图片资源
	free(bg_pjpginfo->buff);
	free(bg_pjpginfo);
	for(int i = 0; i < APP_NUM; i++)
	{
		free(app_jpginfo[i].buff);
	}
	//清空链表
	clear_btn_sqlist(&head);	

	//销毁链表
	destroy_btn_sqlist(&head);
	
	printf("HomeSmart desktop exit\n");
	return 0;
err:
	lcd_destroy(plcdinfo);
	free(bg_pjpginfo->buff);
	free(bg_pjpginfo);
	for(int i = 0; i < APP_NUM; i++)
	{
		free(app_jpginfo[i].buff);
	}
	
	//关闭文件描述符中的soc_fd
	for(int i = 0; i < soc_fds_len; i++)
	{
		shutdown(soc_fds[i], SHUT_RDWR);
	
	}
	soc_fds_len = 0;

	clear_btn_sqlist(&head);
	destroy_btn_sqlist(&head);
	printf("HomeSmart desktop exit because of error\n");
	return -1;

}


void *ts_monitor(void *arg)
{
	int ret;

	//线程分离
	pthread_detach(pthread_self());

	//触摸屏
	struct input_event tsinfo;
	int ts_fd = open("/dev/input/event0", O_RDWR);
	if(ts_fd < 0)
	{
		perror("fail to open touch screen");
		pthread_exit(NULL);
	}

	while(1)
	{
		
		ret = read(ts_fd, &tsinfo, sizeof(struct input_event));
		if(ret < 0)
		{
			perror("error exits in read tsinfo");
			pthread_exit(NULL);
		
		}
		/************************上锁********************************/
		pthread_mutex_lock(&mutex);

		//X
		if(tsinfo.type == EV_ABS && tsinfo.code == ABS_X)
		{
			printf("2\n");
			printf("x = %d\t", tsinfo.value);
			ts_point.X = tsinfo.value;
		}

		//Y
		if(tsinfo.type == EV_ABS && tsinfo.code == ABS_Y)
		{
			printf("y = %d\n", tsinfo.value);
			ts_point.Y = tsinfo.value;
		}
		if(ts_point.lastX != ts_point.X && ts_point.lastY != ts_point.Y)
		{
			//坐标更新
			ts_point.lastX= ts_point.X;
			ts_point.lastY = ts_point.Y;
			ts_point.update = true;
			pthread_cond_signal(&cond);
		}
		
		pthread_mutex_unlock(&mutex);
		/************************解锁*********************************/
	}

}


void *remote_control(void *arg)
{

	int ret;

	//线程分离
	/*
	 *bug:许多soc文件描述符未正常关闭
	 *
	 */
	pthread_detach(pthread_self());

	//创建套接字
	int soc_fd;
	soc_server_init(&soc_fd, NULL, 3002);


	//等待对端连接请求
	//1.声明变量存储对端信息
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	bzero(&client_addr, client_addr_len);
	
	//将监听套接字(soc_fd)加入待待添加集合列表(应该用链表代替)
	soc_fds[soc_fds_len] = soc_fd;
	max_fd = FIND_MAX_FD(soc_fd, max_fd);
	soc_fds_len++;

	char buff[100];

	fd_set fdset;

	//发收信息
	while(1)
	{
		bzero(buff, sizeof(buff));

		//配置多路复用描述符集
		FD_ZERO(&fdset);

		//将待添加集合列表添加
		for(int i = 0; i < soc_fds_len; i++)
		{
			FD_SET(soc_fds[i], &fdset);

		}
		
		//多路复用
		ret = select(max_fd+1, &fdset, NULL, NULL, NULL);
		if(ret < 0)
		{
			perror("error exits in select");
			//return -1;
		}
		else if(ret == 0)
		{
			printf("timeout\n");
			continue;
		}
		
		
		/**********************上锁********************************/
		/*
		pthread_mutex_lock(&mutex);

		//判断当前状态是否为garage
		while(strcmp(state, "garage") == 0)
		{
			//挂起
			printf("into garage, remote control sleep\n");
			pthread_cond_wait(&cond, &mutex);
			printf("outof garage, remote control wakeup\n");
			
			//清空多路复用描述集
			FD_ZERO(&fdset);
		}
		
		pthread_mutex_unlock(&mutex);
		*/
		/***********************解锁********************************/
		

		//有新的客户端连接请求
		if(FD_ISSET(soc_fd, &fdset))
		{
		
			int acc_fd = accept(soc_fd, (struct sockaddr *)&client_addr, &client_addr_len);
			if(acc_fd < 0)
			{
				perror("error exits when accept client connect");
				//return -1;
			}
			
			printf("/**********************desktop server**********************/\n");
			printf("connecting with client.\nip: %s, port: %hd\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			//发送当前状态值
			ret = send(acc_fd, state, strlen(state), 0);
			if(ret < 0)
			{
				perror("error exits in send state when accept");
			
			}

			//更新待添加集合列表
			soc_fds[soc_fds_len] = acc_fd;
			max_fd = FIND_MAX_FD(acc_fd, max_fd);
			soc_fds_len++;

		}
		//从客户端接受消息
		else
		{
			for(int j = 1; j < soc_fds_len; j++)
			{
				if(FD_ISSET(soc_fds[j], &fdset))
				{
					/*******************上锁*********************/
					pthread_mutex_lock(&mutex);

					bzero(command.ascii, sizeof(command.ascii));
		
					ret = recv(soc_fds[j], &command.ascii, sizeof(command.ascii), 0);
					if(ret < 0)
					{
						perror("error exits in recv");
						shutdown(soc_fds[j], SHUT_RDWR);
						//在集合中删除
						soc_fds[j] = soc_fds[soc_fds_len - 1];
						soc_fds_len--;
						
					}
					else if(ret == 0)
					{
						printf("a client offline\n");
						shutdown(soc_fds[j], SHUT_RDWR);
						//在集合中删去
						soc_fds[j] = soc_fds[soc_fds_len - 1];
						soc_fds_len--;
					}
					else 
					{
						printf("command:%s", command.ascii);
						command.update = true;
						
						//发送信号给条件变量
						pthread_cond_signal(&cond);
					}

					pthread_mutex_unlock(&mutex);
					/*******************解锁************************/
				}
			}
		}
	}
}

#define COLOR				0x0000FFFF
#define TEMPRATURE_x		30
#define TEMPRATURE_y		5
#define HUMIDITY_x			130
#define HUMIDITY_y			5
#define DATE_x				316
#define DATE_y				5
#define WEEK_x				490
#define WEEK_y				5
#define STATBAR_FONTSIZE	16

/*
*功能：获取网络天气同时更新 statBar 信息
*返回值：
*	成功：0
*	失败：-1
*/
int getWeatherInfo(char *tempratureStirng, char *humidityString);
/*
*功能：获取网络时间同时更新系统时间
*返回值：
*	成功：0
*	失败：-1
*/
int getInternetTimeInfo(char *weekString);


void *statBarRoutine(void *arg)
{

	int ret;

	pLcdInfo_t plcdinfo = (LcdInfo_t *)arg;

	//读取状态栏背景和相关图标
	pJpgInfo_t statBarBg_pjpginfo = (JpgInfo_t *)malloc(sizeof(JpgInfo_t));
	if(statBarBg_pjpginfo == NULL)
	{
		perror("fail to malloc statBarbg_pjpginfo");
		free(statBarBg_pjpginfo);
		pthread_exit(0);
	}
	decompress_jpg2buffer(statBarBg_pjpginfo, "./image/desktop/statBarBg.jpg");

	//3.加载背景
	draw_pic(plcdinfo, 0, 0, statBarBg_pjpginfo);

	//获取网络天气
	char tempratureString[5] = {0}, humidityString[5] = {0}; 
	getWeatherInfo(tempratureString, humidityString);

	//获取并设置系统时间
	char weekString[10] = {0}; 
	getInternetTimeInfo(weekString);

	time_t clk_time;
	//为什么可以传指针
	struct tm *timeInfo;
	//秒级更新
	char dateString[20] = {0};
	while(1)
	{
		//仅在桌面启动状态栏
		while(strcmp(state, "desktop") != 0)
		{
			pthread_cond_wait(&statBar_cond, &statBar_mutex);

		}
		bzero(dateString, sizeof(dateString));
		//获取时间
		clk_time = time(NULL);
		//不可重入函数
		timeInfo = localtime(&clk_time);
		//读取当前时间
		sprintf(dateString, "%d-%d-%d %d:%d:%d", 	(timeInfo->tm_year)+1900,
												(timeInfo->tm_mon)+1,
												timeInfo->tm_mday,
												timeInfo->tm_hour,
												timeInfo->tm_min,
												timeInfo->tm_sec);
		//整点查询天气
		if(timeInfo->tm_hour == 0 && timeInfo->tm_sec == 0)
		{
			getWeatherInfo(tempratureString, humidityString);
		}
		//3.加载背景
		draw_pic(plcdinfo, 0, 0, statBarBg_pjpginfo);
		print_string(plcdinfo, TEMPRATURE_x, TEMPRATURE_y, tempratureString, STATBAR_FONTSIZE, COLOR);
		print_string(plcdinfo, HUMIDITY_x, HUMIDITY_y, humidityString, STATBAR_FONTSIZE, COLOR);
		print_string(plcdinfo, DATE_x, DATE_y,dateString, STATBAR_FONTSIZE, COLOR);
		print_string(plcdinfo, WEEK_x, WEEK_y,weekString, STATBAR_FONTSIZE, COLOR);

		sleep(1);
	}

}


int getWeatherInfo(char *tempratureString, char *humidityString)
{
	int ret;
	/******************************获取网络天气**********************/
	int getWeatherInfo_soc_fd = client_create(80, "218.14.248.103");

	//定义请求头
	char getBuff[200] ="GET /data/sk/101010100.html HTTP/1.0\r\n";
	strcat(getBuff, "Host: www.weather.com.cn\r\n\r\n");

	/*
	*notice:数组大小必须足够大
	*/
	char dataBuff[1000] = {0};

	//GET
	ret = send(getWeatherInfo_soc_fd, getBuff, 200, 0);
	if(ret <0)
	{
		perror("fail to send get request");
		return -1;
	}

	//等待数据返回
	ret = recv(getWeatherInfo_soc_fd, dataBuff, sizeof(dataBuff), 0);
	if(ret <0)
	{
		perror("recv failed!");
		return -1;
	}
	//定位 json 字符串段
	char *jsonString = strstr(dataBuff, "weatherinfo") - 2;

	//分析json
	cJSON *weatherJson = cJSON_Parse(jsonString);
	if(weatherJson == NULL)
	{
		perror("fail to parse jsonString");
		return -1;
	}
	cJSON *weatherInfoJson = cJSON_GetObjectItem(weatherJson, "weatherinfo");

	//温度
	cJSON *tempratureJson = cJSON_GetObjectItem(weatherInfoJson, "temp");
	strcpy(tempratureString, tempratureJson->valuestring);

	//湿度
	cJSON *humidityJson = cJSON_GetObjectItem(weatherInfoJson, "SD");
	strcpy(humidityString, humidityJson->valuestring);

	//销毁cJSON对象
	cJSON_Delete(weatherJson);

}

int getInternetTimeInfo(char *weekString)
{
	int ret;

	/***********************获取网络时间**********************/
	int getTimeInfo_soc_fd = client_create(80, "139.199.7.215");

	//定义请求头
	char getTimeBuff[200] ="GET /?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json HTTP/1.0\r\n";
	strcat(getTimeBuff, "Host: api.k780.com\r\n\r\n");

	/*
	*notice:数组大小必须足够大
	*/
	char TimeResponseBuff[1000] = {0};

	//GET
	ret = send(getTimeInfo_soc_fd, getTimeBuff, 200, 0);
	if(ret <0)
	{
		perror("fail to send get request");
		return -1;
	}

	//等待数据返回
	ret = recv(getTimeInfo_soc_fd, TimeResponseBuff, sizeof(TimeResponseBuff), 0);
	if(ret <0)
	{
		perror("recv failed!");
		return -1;
	}

	//定位 json 字符串段
	char *TimejsonString = strstr(TimeResponseBuff, "success") - 2;

	//分析json
	cJSON *timeJson = cJSON_Parse(TimejsonString);
	if(timeJson == NULL)
	{
		perror("fail to parse TimeResponseBuff");
		return -1;
	}
	cJSON *timeInfoJson = cJSON_GetObjectItem(timeJson, "result");

	//时间：2019-03-19 15:22:10
	cJSON *dateTimeJson = cJSON_GetObjectItem(timeInfoJson, "datetime_1");
	//设置系统时间
	char setDateCmd[30] = {0};
	sprintf(setDateCmd, "date -s \"%s\"", dateTimeJson->valuestring);
	system(setDateCmd);

	//星期：Tuesday
	cJSON *weekTimeJson = cJSON_GetObjectItem(timeInfoJson, "week_4");
	strcpy(weekString, weekTimeJson->valuestring);

	//销毁cJSON对象
	cJSON_Delete(timeJson);

}

void update_clients(char *message)
{
	
	int ret;
	
	//更新状态值
	bzero(state, sizeof(state));
	strcpy(state, message);
	
	for(int i = 1; i < soc_fds_len; i++)
	{
		ret = send(soc_fds[i], message, strlen(message), 0);
		printf("send %s successfully\n", message);
		if(ret < 0)
		{
			perror("fail to update client");
		
		}
	
	}

}



