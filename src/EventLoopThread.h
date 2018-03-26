/******************************************************************************
 * Filename : EventLoopThread.h
 * Copyright: 
 * Created  : 2018-2-6 by gongzenghao
 * Description: 新建线程，在新线程中创建EventLoop对象并执行loop()
 ******************************************************************************/
#pragma once
#include "noncopyable.h"
#include "Thread.h"
#include "Condition.h"
//#include "EventLoop.h"

using hahs::Thread;
class EventLoop;

//loop所在线程为I/O线程
//EventLoopThread创建新线程，在新线程中创建EventLoop并loop()
class EventLoopThread:noncopyable
{
public:
	EventLoopThread();
	~EventLoopThread();
	EventLoop *startLoop();
	
private:
	EventLoop *loop_;
	MutexLock mutex_;
	Condition cond_;
	Thread loopThread_;
	bool started_;
	//新线程主函数
	//threadFunc要访问data_member,应该声明为成员函数
	//是否需要声明为static_function_member
	void threadFunc();
};

