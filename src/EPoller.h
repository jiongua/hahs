/******************************************************************************
 * Filename : EPoller.h
 * Copyright: 
 * Created  : 2018-1-10 by gongzenghao
 * Description: linux epoll* API
 ******************************************************************************/
#pragma once
#include <sys/epoll.h>
#include <vector>
#include <functional>
#include <map>
#include <memory>
#include <assert.h>
#include <unistd.h>

#include "Channel.h"

/*
The struct epoll_event is defined as :

	typedef union epoll_data {
		void	*ptr;
		int 	 fd;
		uint32_t u32;
		uint64_t u64;
	} epoll_data_t;

	struct epoll_event {
		uint32_t	 events;	// Epoll events 
		epoll_data_t data;		/ User data variable 
	};
*/
class Channel;
class EPoller : noncopyable
{
public:
	explicit EPoller(int size = MAXEVENTS)
	:run_(true)
	{
		epollfd_ = ::epoll_create(size);
		assert(epollfd_ >= 0);
	}
	~EPoller()
	{
		::close(epollfd_);
	}
	
	int wait(int timeout, std::vector<Channel*> *activeChannels);//epoll_wait
	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);

private:
	int epollfd_;
	bool run_;
	//std::map<int, Channel*> uchannelsMap_;

	static const int MAXEVENTS = 1024;
	//std::vector<struct epoll_event> aevents_(MAXEVENTS); //声明error
	std::map<int, struct epoll_event> ueventMap_;
	struct epoll_event aevents_[MAXEVENTS]; 
};

