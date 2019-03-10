#ifndef _GARAGE_H_
#define _GARAGE_H_

#include <stdbool.h>
#include "../lib/lcd/LCD.h"
#include "album.h"


#define CAPACITY	6

typedef struct Car{
	
	int id;
	int park_pos;
	int time_sec;
	int time_min;
	int time_hour;
	int charge;

}Car_t, *pCar_t;



typedef struct Garage_Manage{
	
	Car_t 	car[CAPACITY];
	int 	num;
	int	park_positions[CAPACITY];

}Garage_Manage_t, *pGarage_Manage_t;





//车库管理主程序
int garage(pLcdInfo_t plcdinfo, struct point *pts_point);

pCar_t enter_garage(pGarage_Manage_t pgarage_manage, int id);
pCar_t exit_garage(pGarage_Manage_t pgarage_manage, int id);
int park_update(pLcdInfo_t plcdinfo, pJpgInfo_t pjpginfo, int pos, pGarage_Manage_t pgarage_manage);
int info_update(pLcdInfo_t plcdinfo, pGarage_Manage_t pgarage_manage, pJpgInfo_t pjpginfo);

struct charge_routine_arg{

	pLcdInfo_t 		plcdinfo;
	pGarage_Manage_t 	pgarage_manage;
	pJpgInfo_t		pjpginfo;
};

//计算时常、收费信息子线程
void *charge_routine(void *arg);


struct checkinfo_routine_arg{

	pGarage_Manage_t 	pgarage_manage;
	int soc_fd;

};
//客户端查询停车信息子线程
void *check_info_routine(void *arg);




#endif
