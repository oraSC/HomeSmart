#ifndef _SERIAL_H_
#define _SERAIL_H_

#include <termios.h>
#include <unistd.h>


//返回old_flag
struct termios *serial_setattr(int fd);
int 		send_A(int fd);
unsigned int 		send_B(int fd);
unsigned char	calculate_BCC(unsigned char *buff);



#endif
