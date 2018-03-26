/******************************************************************************
 * Filename : InetAddr.cpp
 * Copyright: 
 * Created  : 2018-1-5 by gongzenghao
 * Description: 
 ******************************************************************************/
//for tcp/udp server 
#include "InetAddr.h"

InetAddr::InetAddr(const int port)
{
	bzero(&addr_, sizeof(struct sockaddr_in));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
}

InetAddr::InetAddr(const std::string &ip, const int port)
{
	bzero(&addr_, sizeof(struct sockaddr_in));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}


