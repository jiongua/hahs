/******************************************************************************
 * Filename : EventLoopThread_test.cpp
 * Copyright: 
 * Created  : 2018-2-6 by gongzenghao
 * Description: 
 ******************************************************************************/
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <unistd.h>
#include <iostream>

int main()
{
	EventLoopThread et;
	EventLoop *loop = et.startLoop();

	loop->queueInloop([](){std::cout << "hello EventLoopThread" << std::endl;});
	
	sleep(10);
}
