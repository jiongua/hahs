/******************************************************************************
 * Filename : Channel.h
 * Copyright: 
 * Created  : 2018-1-10 by gongzenghao
 * Description: 
 ******************************************************************************/
#include <sys/epoll.h>
#include "Channel.h"
#include "EventLoop.h"

#include  <iostream>

const int Channel::kempty = 0;
const int Channel::kread  = EPOLLIN;
const int Channel::kwrite = EPOLLOUT;

void Channel::handleEvent()
{
	//std::cout << "fd = " << fd_ << ", aevents_ == " << aevents_ << " when handlEvent" << std::endl;

	if(userHandleRead_ && (kread & aevents_))
	{
		userHandleRead_();
	}

	if(userHandleWrite_ && (kwrite & aevents_))
	{
		userHandleWrite_();
	}
	aevents_ = 0;
}

void Channel::update()
{
	if(!isEmpty())
	{
		loop_->updateChannel(this);
	}
	else
	{
		std::cout << "removeChannel fd = " << fd_ << std::endl;
		loop_->removeChannel(this);
	}
}

Channel::~Channel()
{
	std::cout << "channel del, fd = " << fd_ << std::endl;
}


