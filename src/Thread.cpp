/******************************************************************************
 * Filename : Thread.cpp
 * Copyright: 
 * Created  : 2018-1-8 by gongzenghao
 * Description: 
 ******************************************************************************/
#include <iostream>
#include "Thread.h"
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "CurrentThread.h"

namespace hahs
{
namespace CurrentThread
{
  __thread int t_cachedTid = 0;
}

//detail:一些非成员函数和非成员数据
namespace detail
{
 
pid_t gettid()
{
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

//作为pthread_create最后一个参数
struct ThreadData
{
	typedef hahs::Thread::threadFunc threadFunc;
	threadFunc func_;
	std::string name_;
	pid_t *tid_;
	CountDownLatch *latch_;
	
	ThreadData(threadFunc func, 
				const std::string &name, 
				pid_t *tid, 
				CountDownLatch *latch)
	:func_(func), 
	 name_(name), 
	 tid_(tid),
	 latch_(latch)
	{}
	
	void runInThread()
	{
		*tid_ = hahs::CurrentThread::tid();
		//tid_在主线程，防止主线程退出后latch_已析构
		tid_ = NULL;
		latch_->countDown();
		//latch_在主线程Thread，防止主线程退出后latch_已析构
		latch_ = NULL;
		//TODO: 捕获异常 
		func_();
	}
};

//pthread_create函数指针参数
void *startThread(void *p)
{
	detail::ThreadData *data = static_cast<detail::ThreadData *>(p);
	//这一步才是真正执行外部注册的线程函数
	data->runInThread();
	delete data;
	return NULL;
}

}//detail
}//hahs


using namespace hahs;

void CurrentThread::cacheTid()
{
  if (t_cachedTid == 0)
  {
    t_cachedTid = detail::gettid();
  }
}

Thread::Thread(threadFunc func, const std::string &name)
:pthreadId_(0),
tid_(0),
name_(name),
started_(false),
joined_(false),
func_(func),
latch_(1)
{
	
}

Thread::~Thread()
{
}

void Thread::start()
{
	std::cout << "start Thread::start" << std::endl;
	//todo 
	//1设置线程属性; 2增加参数传递
	assert(!started_);
	started_ = true;

	//作为pthread_create参数
	detail::ThreadData *obj = new detail::ThreadData(func_, name_, &tid_, 
													&latch_);
	if(pthread_create(&pthreadId_, NULL, detail::startThread, 
					 static_cast<void *>(obj)) != 0)
	{
		//do error
		started_ = false;
		delete obj;
		std::cerr << "pthread_create error" << std::endl;
	}else
	{
		//等待新线程开始执行线程函数前返回
		latch_.wait();
		assert(tid_ > 0);
	}
	
}

int Thread::join()
{
	//check thread status
	assert(started_);
  	assert(!joined_);
  	joined_ = true;
	return pthread_join(pthreadId_, NULL);
}

