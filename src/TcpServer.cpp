#include "TcpServer.h"
#include "TcpConnection.h"

//注册到Accptor,  accept后回调
//成功接受一条新连接
//run in main_thread
void TcpServer::onConnectEstablish(int connfd)
{
	//新建tcpConnection对象
	//将对象挂到某个eventLoop
	//todo: loop assigned to TcpConnection could get from EventLoopThreadPool
	EventLoop *loop = etPool_.getNextLoop();
	connPtr newConn(new TcpConnection(connfd, loop));
	{
		MutexLockGuard lock(connsMapMutex_);
		connsMap_.insert(make_pair(connfd, newConn));
	}

	
	//配置tcpConnection
	newConn->setReadCallBack(std::move(onMessage_));
	newConn->setDisconnectCallBack(std::bind(&TcpServer::removeConnect, this, _1));
	//std::cout << "set userWriteComplete_" << std::endl;
	newConn->setWriteCompleteCallBack(std::move(onWriteComplete_));

	//注意：回调userOnConnect_与set*函数调用的顺序
	//回调用户注册的连接处理函数
	if(onConnect_)
	{
		onConnect_(newConn);
	}
}
//可被多线程回调
//必须线程安全
void TcpServer::removeConnect(TcpConnection *pConn)
{
	int fd = pConn->fd();
	if(connsMap_.find(fd) == connsMap_.end())
	{
		assert(0);
	}
	//从map删除<fd, connPtr>,会始引用计数减1
	//注意1：不要手动删除tcpConnetion，它由智能指针管理
	//注意2：不要手动close fd，当tcpConnetion销毁时，Socket析构函数close
	{
		MutexLockGuard lock(connsMapMutex_);
		connsMap_.erase(fd);
	}
	//fixme: onDisConnect_需要线程安全吗？
	if(onDisConnect_) onDisConnect_();
}

