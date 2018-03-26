/******************************************************************************
 * Filename : EventLoopThreadPool.h
 * Copyright: 
 * Created  : 2018-2-11 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once
#include "EventLoop.h"
#include "EventLoopThread.h"
class EventLoopThreadPool
{
public:
	explicit EventLoopThreadPool(int maxEventLoop)
	:maxLoop_(maxEventLoop),
	 index_(0),
	 started_(false)
	{
		pool_.resize(maxLoop_);
	}
	
	~EventLoopThreadPool()
	{
		//do nothing
		started_ = false;
	}
	
	void start();

	//为新连接分配loop
	//fixme:会多线程同时调用吗？
	EventLoop* getNextLoop()
	{
		if(started_ == false)
		{
			return nullptr;
		}
		
		//均匀分配
		EventLoop *loop = loops_[index_ % maxLoop_];
		assert(loop != nullptr);
		index_++;
		return loop;
	}

	inline int maxLoop() const {return maxLoop_;}
private:
	int maxLoop_;
	int index_; //fixme:原子的?
	bool started_;	//先调用start()才能调用其他接口
	std::vector<std::unique_ptr<EventLoopThread>> pool_;
	std::vector<EventLoop*> loops_;
};
