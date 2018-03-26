/******************************************************************************
 * Filename : EventLoopThreadPool.cpp
 * Copyright: 
 * Created  : 2018-2-11 by gongzenghao
 * Description: 
 ******************************************************************************/
#include "EventLoopThreadPool.h"

void EventLoopThreadPool::start()
{
	assert(started_ == false);
	for(int i = 0; i < maxLoop_; i++)
	{
		pool_.emplace_back(new EventLoopThread());
		loops_.push_back(pool_[i]->startLoop());
	}
	started_ = true;
}

