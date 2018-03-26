/******************************************************************************
 * Filename : Conector.cpp
 * Copyright: 
 * Created  : 2018-1-31 by gongzenghao
 * Description: 
 ******************************************************************************/
#include "Connector.h"
#include "SocketOps.h"
#include "InetAddr.h"
#include "Channel.h"
#include <errno.h>
#include <assert.h>
#define DEBUG_CONNECTOR 
#ifdef DEBUG_CONNECTOR
#include <iostream>
#include <string.h>	//for strerror

using std::cout;
using std::endl;
using std::cerr;

#endif


Connector::Connector(InetAddr *addr, EventLoop *loop)
 :serverAddr_(addr)
{
	connfd_ = sockets::createNonblockingOrDie(serverAddr_->family(), SOCK_STREAM);
	assert(connfd_ > 0);
	channel_.reset(new Channel(connfd_, loop));
	channel_->setWriteCallBack(std::bind(&Connector::handleWrite, this));
	//channel_->setReadCallBack(std::bind(&Connector::handRead, this));
	//channel_->enableWrite();
}

//启动一个tcp连接请求
void Connector::start()
{
	//create nonblock socket default
	int ret = sockets::connectNonblocking(connfd_, serverAddr_->getSockAddr());

	if(ret == 0)
	{
		//0: 服务器在本机时可能发生
		//TODO
		cout << "connect return 0" << endl;
		sockets::close(connfd_);
		return;
	}
	
	if(ret < 0)
	{
		switch(errno)
		{
			//EISCONN: 当前socket已建立tcp连接
			case EISCONN:
			//EINPROGRESS： 正在连接...
			case EINPROGRESS:
				doConnect(errno);
			break;

			//可以retry
			case EAGAIN:
			case ETIMEDOUT:
				doRetry(errno);
			break;
				
			//不可建立tcp连接
			case EADDRINUSE:	//Local address is already in use 
			case EACCES:
			case EPERM:
			case EBADF:
			case EINTR:
			case ECONNREFUSED:	// No-one listening on the remote address.
			case ENETUNREACH:
			case ENOTSOCK:
			case EAFNOSUPPORT:
			case EALREADY:
				doError(errno);
			break;
			default:
			//
			break;
		}
	}
	
	
}

void Connector::doConnect(int err)
{
	if(err == EINPROGRESS)
	{
		//已发起连接但尚未成功
		//channel_->enableRead();
		channel_->enableWrite();
	}
	else if(err == EISCONN)
	{
		//当前socket已建立了tcp连接
		//do nothing?
	}
}

//重连
void Connector::doRetry(int err)
{
//todo
	cerr << "retry!  error == " << strerror(err) << endl;
}


void Connector::doError(int err)
{
#ifdef DEBUG_CONNECTOR
	cerr << "doError error: " << strerror(err) << endl;
#endif
	sockets::close(connfd_);
}

//socket可写
void Connector::handleWrite()
{
	//tcp连接成功，socket一定可写
	//tcp连接出错，socket既可读又可写
	//再次调用connect,判断返回

	//fixme:只关注了一个事件， 这里disable后channel事件为空,会从epoll删除
	channel_->disableWrite();

	if(sockets::getOptSoError(connfd_) == 0)
	{
		//connect ok!
		cout << "handleWrite connect ok!" << endl;
		if(onConnection_) onConnection_(connfd_);
	}
	else
	{
		doError(errno);
	}

	//it's also error, ret always return 0
	/*
	int ret = sockets::connectNonblocking(connfd_, serverAddr_->getSockAddr());
	cout << "ret = " << ret << endl;
	if(ret < 0) {
		if(errno == EISCONN)
		{
			//连接成功
			if(onConnection_) 
			{
				cout << "okkkkk" << endl;
				onConnection_(connfd_);
			}
		}
		else
		{
			//连接出错
			//fixme
			doError(ret);
		}
	}
	*/
}





