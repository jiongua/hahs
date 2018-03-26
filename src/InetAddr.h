/******************************************************************************
 * Filename : InetAddr.h
 * Copyright: 
 * Created  : 2018-1-5 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once
#include <string>  
#include <sys/socket.h>
#include <strings.h>  // bzero
#include "SocketOps.h"

class InetAddr
{
public:
	InetAddr()
	{
		bzero(&addr_, sizeof(struct sockaddr_in));
	}
	//for tcp/udp server 
	explicit InetAddr(const int port);
	
	InetAddr(const std::string &ip, const int port);

	//@addr must be valid
	explicit InetAddr(const struct sockaddr_in& addr)
	   : addr_(addr)
	 { }

	const struct sockaddr *getSockAddr() const
	{
		return sockets::sockaddr_cast(&addr_);
	}

	struct sockaddr_in *getSockInAddr()
	{
		return &addr_;
	}

	sa_family_t family() const
	{
		return addr_.sin_family;
	}
	
	~InetAddr() = default;
private:
	struct sockaddr_in addr_;
};

