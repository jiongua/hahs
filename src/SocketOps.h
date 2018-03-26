/******************************************************************************
 * Filename : SocketOps.h
 * Copyright: 
 * Created  : 2018-1-2 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>  // snprintf
#include <strings.h>  // bzero
#include <sys/socket.h>
#include <sys/uio.h>  // readv
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */

namespace sockets{
	//create block socket
	int create(sa_family_t family, int sock_type);
	//create non-block socket
	int createNonblockingOrDie(sa_family_t family, int sock_type);
	//非阻塞connect, 独立使用
	//@waitTimeMs 等待3次握手
	//@retryCnt: 重连次数
	//@-1 or 0
	int  connectNonblocking(int sockfd, const struct sockaddr* addr);
	void bindOrDie(int sockfd, const struct sockaddr* addr);
	void listenOrDie(int sockfd, int backlog);
	//@non_block: set connected-fd non-block
	int accept(int sockfd, struct sockaddr_in* addr, bool non_block);

	//tcp non-blocking send/recv
	ssize_t sendNonblocking(int sockfd, const void *buf, size_t len);
	ssize_t recvNonblocking(int sockfd, void *buf, size_t len);
	
	//todo(done)
	//hahs要设计接收Buffer，不必在意每次接收数据的长度
	//每次从sockfd读取数据到Buffer
	//上层用户处理数据前先查看Buffer中的数据是否完整，完整后才从Buffer读取
	ssize_t read(int sockfd, void *buf, size_t count);
	ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
	ssize_t write(int sockfd, const void *buf, size_t count);
	
	//recvTcpAll: sockfd must be block_socket
	ssize_t recvTcpAll(int sockfd, void *buf, size_t len);
	ssize_t recvUdp(int sockfd, void *buf, size_t len, struct sockaddr_in* addr);
	
	void close(int sockfd);
	//void shutdownWrite(int sockfd);

	int getOptSoError(int);
	void setOptReuseAddr(int);
	void setOptReusePort();

	const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
	struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);
	const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);

	int getLocalAddr(int sockfd, struct sockaddr_in *);
	int getPeerAddr(int sockfd, struct sockaddr_in *);

	void toIpPort(char* buf, size_t size, const struct sockaddr* addr);
	void toIp(char* buf, size_t size, const struct sockaddr* addr);
	void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);
	//void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr);

}
 
