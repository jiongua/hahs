/******************************************************************************
 * Filename : EventLoopThread.cpp
 * Copyright: 
 * Created  : 2018-2-6 by gongzenghao
 * Description: 
 ******************************************************************************/
#include <functional>
#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread()
:loop_(nullptr),
 mutex_(),
 cond_(mutex_),
 loopThread_(std::bind(&EventLoopThread::threadFunc, this)),
 started_(false)
{

}

//如果EventLoopThread析构，应该让loop线程终止?
EventLoopThread::~EventLoopThread()
{
	if(!loop_){
		loop_->stop();
		loopThread_.join();
	}
}

//startLoop在主线程调用,新线程创建EventLoop并执行loop
//startLoop返应该在新线程创建EventLoop完成后才可以返回
EventLoop* EventLoopThread::startLoop()
{
	if(started_ == false){
		//启动新线程
		loopThread_.start();
		//loopThread_.start()返回，代表新线程已运行(但不一定是在执行线程函数loop)

		//等待新线程执行线程函数完毕
		//由于主线程和新线程可能同时访问loop_产生竞态,需要加锁保护
		{
			MutexLockGuard lock(mutex_);
			while(loop_ == nullptr)
			{
				//主线程等待新线程创建成功loop
				cond_.wait();	
			}
		}
		//loop_->setTid(loopThread_.tid());
		started_ = true;
		return loop_;
	}
	else
	{
		return nullptr;
	}
}

//threadFunc是在I/O线程中执行，但它是EventLoopThread类成员
//可以访问类声明的数据成员
void EventLoopThread::threadFunc()
{
	EventLoop loop;
	{
		MutexLockGuard lock(mutex_);
		//必须确保loop_被赋值后才notify，防止startLoop返回空指针
		loop_ = &loop;
		cond_.notify();
	}
	loop.loop();
//fixme: 
//EventLoop对象生命期与线程主函数作用域相同
//如果线程主函数退出，EventLoop将析构，外部持有的指针将失效！
}

