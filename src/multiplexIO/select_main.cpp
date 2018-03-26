/******************************************************************************
 * Filename : select_main.cpp
 * Copyright: 
 * Created  : 2018-1-10 by gongzenghao
 * Description: the example of using select
 ******************************************************************************/
 
 /*
 Manual:
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int select(int nfds, fd_set *readfds, fd_set *writefds,
		  fd_set *exceptfds, struct timeval *timeout);

void FD_CLR(int fd, fd_set *set);
int  FD_ISSET(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_ZERO(fd_set *set);
 */