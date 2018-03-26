/******************************************************************************
 * Filename : Acceptor.h
 * Copyright: 
 * Created  : 2018-1-19 by gongzenghao
 * Description: deal new incoming connection request
 ******************************************************************************/

//由tcpServer创建，负责接受listenfd上新到连接
//生存期应有tcpServer负责
#pragma once
#include <memory>
#include <functional>
#include "noncopyable.h"
#include "Socket.h"
#include "Channel.h"

//class Channel;
class EventLoop;
class InetAddr;
class Acceptor:noncopyable
{
public:
	typedef std::function<void(int)> newConnectionCallBack;
	//localAddr is created outside
	//loop is created outside
	Acceptor() = default;
	Acceptor(InetAddr *localAddr, EventLoop *loop);
	//Acceptor注册到channel的事件回调函数，当listenfd可读时触发

	//只被Channel调用
	void handleRead();
	//TcpServer注册的新的连接回调
	//在onConnection_实现中创建tcpConnection对象
	void setConnectionCallBack(newConnectionCallBack &&cb){onConnection_ = cb;} 

	void start();
private:
	Socket listenSock_;
	std::unique_ptr<Channel> channel_; 
	newConnectionCallBack onConnection_;
};
