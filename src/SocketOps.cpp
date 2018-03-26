/******************************************************************************
 * Filename : SocketOps.h
 * Copyright: 
 * Created  : 2018-1-2 by gongzenghao
 * Description: 
 ******************************************************************************/

#include "SocketOps.h"
#include <assert.h>
#include <iostream>
#include <string.h>

using std::cerr;
using std::endl;

namespace{

void setNonBlockAndCloseOnExec(int sockfd)
{
  // non-block
  int flags = ::fcntl(sockfd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int ret = ::fcntl(sockfd, F_SETFL, flags);
  // FIXME check

  // close-on-exec
  flags = ::fcntl(sockfd, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  ret = ::fcntl(sockfd, F_SETFD, flags);
  // FIXME check

  (void)ret;
}

//todo test
//判断sockfd是否是非阻塞
int checkSocketNonBlock(int sockfd)
{
	const int flags = ::fcntl(sockfd, F_GETFL, 0);
	return flags & O_NONBLOCK;
}

}

int sockets::create(sa_family_t family, int sock_type)
{
	int sockfd = ::socket(family, sock_type, 0);
	if(sockfd < 0)
	{
		//fixme: add log
		cerr << "error in " << __func__ << "socket error" << endl;
	}
	return sockfd;
}

int sockets::createNonblockingOrDie(sa_family_t family, int sock_type)
{
	int sockfd = ::socket(family, sock_type | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if(sockfd < 0)
	{
		//fixme: add log
		cerr << "error in " << __func__ <<"socket error" << endl;
	}
	return sockfd;
}

//todo
//set waitTimeMs 
int  sockets::connectNonblocking(int sockfd, const struct sockaddr* addr)
{
	if(checkSocketNonBlock(sockfd) == 0)
	{
		cerr << "sockfd in " << __func__ <<"is not non-blocking" << endl;
		setNonBlockAndCloseOnExec(sockfd);
	}
	return ::connect(sockfd, addr, sizeof(struct sockaddr));
}

void sockets::bindOrDie(int sockfd, const struct sockaddr* addr)
{
	int ret = ::bind(sockfd, addr, sizeof(struct sockaddr));
	if(ret < 0)
	{
		//fixme: add log	
		cerr << "error in " << __func__ << "bindOrDie error" << endl;
	}
}

void sockets::listenOrDie(int sockfd, int backlog)
{
	int ret = ::listen(sockfd, backlog);
	if(ret < 0)
	{
		//fixme: add log
		cerr << "error in " << __func__ << "listenOrDie error" << endl;
	}
}

//only for Socket::accept
int sockets::accept(int sockfd, struct sockaddr_in* addr, bool non_block)
{
	socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
	int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
	//在 Linux 系统里，accept() 返回的新建的套接口 不会 继承监听套接口的诸如O_NONBLOCK等属性
	if(non_block > 0){
		setNonBlockAndCloseOnExec(connfd);
	}
	//在 Linux 里， accept() 把本属于accept() 的但未处理的网络错误传递给新建的套接口
	//处理accept返回错误信息
	if(connfd < 0)
	{
		//fixme: add log
		cerr << "error in " << __func__ << "accept error" << endl;
	}
	return connfd;
}

ssize_t sockets::sendNonblocking(int sockfd, const void *buf, size_t len)
{
	return ::send(sockfd, buf, len, MSG_DONTWAIT);
}

ssize_t sockets::recvNonblocking(int sockfd, void *buf, size_t len)
{
	return ::recv(sockfd, buf, len, MSG_DONTWAIT);
}


//read/write的sockfd创建时均默认为非阻塞
ssize_t sockets::read(int sockfd, void *buf, size_t count)
{
	return ::read(sockfd, buf, count);
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt)
{
	return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count)
{
	return ::write(sockfd, buf, count);
}

//@sockfd: must be block
//todo: add wait time for recv  all data
ssize_t sockets::recvTcpAll(int sockfd, void *buf, size_t len)
{
	if(checkSocketNonBlock(sockfd))
	{
		cerr << "error in " << __func__ << ": sockfd must be block" << endl;
		return 0;
	}
	return recv(sockfd, buf, len, MSG_WAITALL);
}

ssize_t sockets::recvUdp(int sockfd, void *buf, size_t len, struct sockaddr_in* addr)
{
	socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
	//flag = 0
	return ::recvfrom(sockfd, buf, len, 0, sockaddr_cast(addr), &addrlen);
}

void sockets::close(int sockfd)
{
	if(::close(sockfd) < 0)
	{
		//fixme: add log
	}
}

int sockets::getOptSoError(int fd)
{
	int error = -1;
	socklen_t optLen = static_cast<socklen_t>(sizeof(error));
	::getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &optLen);
	return error;
}

void sockets::setOptReuseAddr(int fd)
{
	int on = 1;
	int ret = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
	if(ret < 0)
	{
		//todo: error deal
	}
}

void sockets::setOptReusePort()
{
	
}



//convert sockaddr_in to  sockaddr
const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr)
{
	 //return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
	 return reinterpret_cast<const struct sockaddr*>(addr);
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in* addr)
{
	 //return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
	 return reinterpret_cast<struct sockaddr*>(addr);
}
//convert sockaddr to  sockaddr_in
const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr)
{
  //return static_cast<const struct sockaddr_in*>(implicit_cast<const void*>(addr));
  return reinterpret_cast<const struct sockaddr_in*>(addr);
}


int sockets::getLocalAddr(int sockfd, struct sockaddr_in *localAddr)
{
	socklen_t addrlen = sizeof(struct sockaddr);
	return ::getsockname(sockfd, sockaddr_cast(localAddr), &addrlen);
}

//for tcp
int sockets::getPeerAddr(int sockfd, struct sockaddr_in *peerAddr)
{
	socklen_t addrlen = sizeof(struct sockaddr);
	return ::getpeername(sockfd, sockaddr_cast(peerAddr), &addrlen);
}

//addr -> ip::port
void sockets::toIpPort(char* buf, size_t size,
                       const struct sockaddr* addr)
{
  //addr->ipstr
  toIp(buf,size, addr);
  size_t end = ::strlen(buf);
  const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
  uint16_t port = ::ntohs(addr4->sin_port);
  assert(size > end);
  snprintf(buf+end, size-end, ":%u", port);
}

//sockaddr -> ipstr
void sockets::toIp(char* buf, size_t size,
                   const struct sockaddr* addr)
{
  if (addr->sa_family == AF_INET)//IPv4
  {
    assert(size >= INET_ADDRSTRLEN);
    const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
    //inet_ntop: converts the network address structure  into a character string
    ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
  }
  //for ipv6
  /*else if (addr->sa_family == AF_INET6)
  {
    assert(size >= INET6_ADDRSTRLEN);
    const struct sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
    ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
  }*/
}

//根据ip::por配置sockaddr_in
//for ipv4
void sockets::fromIpPort(const char* ip, uint16_t port,
                         struct sockaddr_in* addr)
{
  addr->sin_family = AF_INET;
  addr->sin_port = ::htons(port);
  //inet_pton: convert IPv4 and IPv6 addresses from text to binary form
  if(::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
  {
    cerr << "sockets::fromIpPort" << endl;
  }
}

//根据ip::por配置sockaddr_in,
//for ipv6
/*void sockets::fromIpPort(const char* ip, uint16_t port,
                         struct sockaddr_in6* addr)
{
  addr->sin6_family = AF_INET6;
  addr->sin6_port = ::htons(port);
  if(::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
  {
    cerr << "sockets::fromIpPort" << endl;
  }
}*/



