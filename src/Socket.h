/******************************************************************************
 * Filename : Socket.h
 * Copyright: 
 * Created  : 2018-1-29 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once
#include "noncopyable.h"
#include "SocketOps.h"
#include <iostream>

//RAII 封装、管理socket fd
class Socket:noncopyable
{
public:
	Socket() = default;
	Socket(int socketFD)
	:fd_(socketFD)
	{}
	
	~Socket()
	{
		std::cout << "Socket destructor, close fd " << fd_ << std::endl;
		sockets::close(fd_);
	}
	
	int fd() const {return fd_;}
private:
	
	int fd_;
};
