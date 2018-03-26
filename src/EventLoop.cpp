/******************************************************************************
 * Filename : EventLoop.cpp
 * Copyright: 
 * Created  : 2018-1-17 by gongzenghao
 * Description: 
 ******************************************************************************/
#include "EventLoop.h"
//#include "EPoller.h"
//#include "Channel.h"

//debug
#include <iostream>
using std::cout;
using std::endl;

int createWakeupFD()
{
	int fd = eventfd(0, EFD_NONBLOCK);
	cout << "fd == " << fd << endl;
	assert(fd != -1);
	return fd;
}

EventLoop::EventLoop()
 :poll_(new EPoller()),
 wakeupFD_(createWakeupFD()),
 wakeupChannel_(new Channel(wakeupFD_, this)),
 tid_(hahs::CurrentThread::tid()),
 running_(false)
{
	wakeupChannel_->setReadCallBack(std::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableRead();
}

EventLoop::~EventLoop()
{
//manage wakeupChannel_ and wakeupFD

}

//run in I/O thread
//fixme: IO线程的问题在于一个线程同时只能读一个fd,
//如果一个IO线程管理了多个tcp连接，如果多个fd同时可读或可写，则同一时间处理一个fd
//IOtask放在IO线程执行，如果某个task耗时较长，将影响其他任务/fd事件的响应
void EventLoop::loop()
{
	cout << "start loop....." << endl;
	running_ = true;
	while(running_)
	{
		//must clear!!
		activeChannels_.clear();
		//todo: ktimeoutMS
		//阻塞直到有事件（I/O、定时器）发生
		//回调操作应避免阻塞I/O线程
		//防止回调时间过长影响I/O响应？
		int activeNumers = poll_->wait(ktimeoutMS, &activeChannels_);
		for(int i = 0; i < activeNumers; i++)
		{
		
			//std::cout << "fd[" << activeChannels_[i]->fd() << "], handleEvent in loop" << std::endl;
			//note:"不同Channel对象的同一个接口有不同实现"		
			//std::cout << "call Channel::handleEvent()" << std::endl;
			activeChannels_[i]->handleEvent();
		}
		handleTask();
		
	}
	//NOTE: loop()不应该退出(见class EventLoopThread)
}

//被EventLoopThread析构函数中调用
//注意，loop可能处于阻塞，需要唤醒
void EventLoop::stop()
{
	running_ = false;	
	wakeup();
}

void EventLoop::updateChannel(Channel* channel)
{
	//std::cout << "fd[" << channel->fd() << "], updateChannel in eventLoop" << std::endl;
	uchannelMap_[channel->fd()] = channel;
	poll_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
	//std::cout << "fd[" << channel->fd() << "], removeChannel in eventLoop" << std::endl;
	uchannelMap_.erase(channel->fd());
	poll_->removeChannel(channel);
}

void EventLoop::runInloop(taskFactor &&cb)
{
	if(isInLoopThread())
	//if(hahs::CurrentThread::tid() == tid_)
	{
		cb();
	}
	else
	{
		queueInloop(std::move(cb));
	}
}

//非I/O线程的操作转移到I/O线程loop中处理
void EventLoop::queueInloop(taskFactor &&cb)
{
	{
		MutexLockGuard lock(mutexTask_);
		taskqueue_.push_back(std::move(cb));
	}
	
	//从loop循环中唤醒
	wakeup();
}

void EventLoop::wakeup()
{
	//write sth to eventFD_
	uint64_t x = 1;
	if(::eventfd_write(wakeupFD_, x) < 0)
	{
		cout << "write error" << endl;
	}
	//wakeupFD_将一直触发可读事件直到数据被读取
	//fixme:重复eventfd_write多次是否有问题?
}

void EventLoop::handleRead()
{
	uint64_t x;
	::eventfd_read(wakeupFD_, &x);
}

//在I/O线程中执行
void EventLoop::handleTask()
{
	std::vector<taskFactor> tasks;
	{
		MutexLockGuard lock(mutexTask_);
		tasks.swap(taskqueue_);
	}
	for(auto &cb:tasks)
	{
		cb();
	}
}

