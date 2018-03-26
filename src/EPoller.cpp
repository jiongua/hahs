/******************************************************************************
 * Filename : EPoller.cpp
 * Copyright: 
 * Created  : 2018-1-10 by gongzenghao
 * Description: 
 ******************************************************************************/

//called by Channel
//#include <sys/epoll.h>

#include "EPoller.h"
#include <iostream>
#include <string.h>


//fixme:同一个fd上的可写事件需要经常添加和删除，会影响性能
//epoll_ctl开销
void EPoller::updateChannel(Channel *channel)
{
	//如何区分EPOLL_CTL_ADD/EPOLL_CTL_MOD

	int fd = channel->fd();
	if(ueventMap_.find(fd) == ueventMap_.end())
	{
		//new fd event
		struct epoll_event ev;
		ev.data.ptr = channel;
		ev.events = channel->uevents();
		ueventMap_.insert({fd, ev});
		::epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ueventMap_[fd]);
		std::cout << "add new event,  fd == " << fd << ", uevent = " << channel->uevents() << std::endl;
	}
	else
	{
		ueventMap_[fd].data.ptr = channel;
		ueventMap_[fd].events = channel->uevents();
		::epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ueventMap_[fd]);
		//std::cout << "modify fd == " << fd << ", uevent = " << channel->uevents() << std::endl;
	}
}

void EPoller::removeChannel(Channel *channel)
{
	assert(ueventMap_.find(channel->fd()) != ueventMap_.end());
	ueventMap_.erase(channel->fd());
	::epoll_ctl(epollfd_, EPOLL_CTL_DEL, channel->fd(), NULL);
	std::cout << "remove fd" << channel->fd() << std::endl;
}


//called by eventLoop::loop
int EPoller::wait(int timeout, std::vector<Channel*> *activeChannels)
{
	int activeNums = 0;
	while(run_)
	{
		activeNums = ::epoll_wait(epollfd_, aevents_, 
							 MAXEVENTS, timeout);
		if(activeNums < 0) 
		{
			if(errno != EINTR)
			{
				std::cerr << "epoll_wait error " << strerror(errno)<<std::endl;
				run_ = false;
				assert(0);
			}		
		}
		else 
		{
			//std::cout << "activeNums == " << activeNums << std::endl;
			
			for(int i = 0; i < activeNums; i++)
			{
				Channel* curChannel = static_cast<Channel *>(aevents_[i].data.ptr);
		
				assert(!(curChannel == NULL));
				//std::cout << "active fd = "<< curChannel->fd() << ", Channel's uev = " << curChannel->uevents() << std::endl;

				//std::cout <<"aevents_'s aev = " << aevents_[i].events << std::endl;

				if(ueventMap_.find(curChannel->fd()) == ueventMap_.end()) 
				{
					std::cerr << "fd " << curChannel->fd() << "not exist in map" << std::endl;

					assert(0);
				}
				curChannel->setActiveEvent(aevents_[i].events);
				activeChannels->push_back(curChannel);				
			}
			break;
		}
	}
	return activeNums;
}

