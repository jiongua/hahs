/******************************************************************************
 * Filename : Conector.h
 * Copyright: 
 * Created  : 2018-1-31 by gongzenghao
 * Description: 
 ******************************************************************************/
//仅对TcpClient使用
//已非阻塞方式发起tcp连接

#pragma once
#include <memory>
#include <functional>

#include "noncopyable.h"

class Channel;
class EventLoop;
class InetAddr;
class Connector:noncopyable
{
public:
	typedef std::function<void(int)> newConnectCallBack;
	Connector(InetAddr *addr, EventLoop *loop);
	~Connector(){}

	void start();//start connect

	//called by tcpClient
	void setConnectionCallBack(newConnectCallBack &&cb) {onConnection_ = cb;}
	
	//注册到Channnel的回调
	void handleWrite();
	//void handleRead();
	
private:
	void doRetry(int);//重连
	void doConnect(int);
	void doError(int);

	//Socket connSock_;
	EventLoop *loop_;
	int connfd_;
	std::unique_ptr<Channel> channel_;
	InetAddr *serverAddr_;
	newConnectCallBack onConnection_;
};
