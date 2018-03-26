/******************************************************************************
 * Filename : Thread.h
 * Copyright: 
 * Created  : 2018-1-8 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once

#include <string>
#include <functional>
#include <pthread.h>

#include "CountDownLatch.h"

namespace hahs{

class Thread : noncopyable
{
public:
//user use function&bind like this:
//Thread(bind(newCalled, arg1, arg2...), "threadName");
//the function " void newCalled(arg1, arg2..) " user can define
	typedef std::function<void()> threadFunc;	
	//typedef void*(*threadFunc)(void *);
	Thread() = default;
	
	explicit Thread(threadFunc, const std::string &name = std::string());
	~Thread();

	//返回的是新线程的tid?
	pid_t tid() const {return tid_;}
	//启动新线程
	void start();
	int join();

	//todo add getThreadId()
	std::string getThreadName() const {return name_;}
	bool started() const {return started_;}
	bool joined() const {return joined_;}
	
private:
	pthread_t pthreadId_;			//可读性差
	pid_t tid_;						//新线程的tid
	std::string name_;
	bool started_;
	bool joined_;
	threadFunc func_;
	CountDownLatch latch_;
};

}
