/******************************************************************************
 * Filename : Acceptor.cpp
 * Copyright: 
 * Created  : 2018-1-19 by gongzenghao
 * Description: deal new incoming connection request
 ******************************************************************************/
#include "Acceptor.h"
//#include "Channel.h"
#include "SocketOps.h"
#include "InetAddr.h"

void Acceptor::start()
{
	sockets::listenOrDie(listenSock_.fd(), 5);
	channel_->enableRead();
}

Acceptor::Acceptor(InetAddr *localAddr, EventLoop *loop)
 :listenSock_(sockets::createNonblockingOrDie(localAddr->family(), SOCK_STREAM)),
 channel_(new Channel(listenSock_.fd(), loop))
{
	//fixme: error deal
	sockets::setOptReuseAddr(listenSock_.fd());
	sockets::bindOrDie(listenSock_.fd(), localAddr->getSockAddr());
	channel_->setReadCallBack(std::bind(&Acceptor::handleRead, this));
}
//如果采用阻塞式的accpet，将阻塞在等待新连接到来，直到三次握手完成accept才返回
//使用非阻塞模式，当listenfd可读时，代表有新连接到来,调用accept完成连接
void Acceptor::handleRead()
{
	InetAddr peerAddr;
	int connfd = sockets::accept(listenSock_.fd(), peerAddr.getSockInAddr(), 1);
	if(connfd < 0)
	{
		sockets::close(connfd);
	}
	
	if(onConnection_) onConnection_(connfd);
}


