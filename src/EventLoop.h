/******************************************************************************
 * Filename : EventLoop.h
 * Copyright: 
 * Created  : 2018-1-17 by gongzenghao
 * Description: 
 ******************************************************************************/
//It's a Reactor
//one EventLoop per one thread
//create by user

#pragma once
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <sys/eventfd.h>	//for eventfd()
#include "noncopyable.h"
#include "BlockingQueue.h"
#include "Channel.h"
#include "EPoller.h"
#include "CurrentThread.h"

//class Channel;
//class EPoller;

class EventLoop:noncopyable
{
public:	
	typedef std::function<void(void)> taskFactor;
	EventLoop();
	~EventLoop();
	void loop();
	//终止loop循环，loop线程退出
	void stop();
	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);
	
	void runInloop(taskFactor &&);
	void queueInloop(taskFactor &&);
	bool isInLoopThread() const {return hahs::CurrentThread::tid() == tid_;}
	pid_t tid() const {return tid_;}

	void handleRead();	//for wakeup

private:
	std::unique_ptr<EPoller> poll_;
	int wakeupFD_;	//eventfd();
	const pid_t tid_;	//由构造eventLoop对象的线程初始化
	bool running_;
	std::map<int, Channel*> uchannelMap_;
	std::vector<Channel*> activeChannels_;
	std::unique_ptr<Channel> wakeupChannel_;//本对象使用的channel

	//非I/O线程需要执行的操作转移到I/O线程
	MutexLock mutexTask_;
	std::vector<taskFactor> taskqueue_;
	static const int ktimeoutMS = 10000;

	
	void wakeup();
	void handleTask();
	
};
