/******************************************************************************
 * Filename : TcpClient.h
 * Copyright: 
 * Created  : 2018-1-31 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once
#include <memory>
#include <functional>
#include <map>

#include "noncopyable.h"
#include "Connector.h"
#include "CallBack.h"

class EventLoop;
class InetAddr;
class TcpConnection;
class Buffer;
using std::placeholders::_1;

class TcpClient:noncopyable
{
public:
	//typedef std::shared_ptr<TcpConnection> connPtr;
	//typedef std::function<void(void)> userDisConnectCallBack;
	//typedef std::function<void(connPtr&)> userConnectCallBack;
	//typedef std::function<void(connPtr, Buffer*, int)> messagecallBack;
	//typedef std::function<void(const connPtr&)> userCompleteCallBack;

	TcpClient(InetAddr *serveraddr, EventLoop *loop)
	:loop_(loop),
	 connector_(serveraddr, loop_)
	{
		connector_.setConnectionCallBack(std::bind(&TcpClient::onConnectEstablish, this, _1));
	}

	
	~TcpClient(){}
	
	void setConnectCallBack(connectCallBack &&cb) {onConnect_ = cb;}

	//userOnMessage_传递到tcpConnection
	void setMessageCallBack(messageCallBack &&cb) {onMessage_ = cb;}

	//userOnDisConnect_传递到tcpConnection
	void setDisConnectCallBack(disConnectCallBack &&cb){onDisConnect_ = cb;}

	void setWriteCompleteCallBack(completeCallBack &&cb) {onWriteComplete_ = cb;}

	void start() {connector_.start();}

private:
	//TcpClient自己定义的新连接处理函数，注册到Connetor
	void onConnectEstablish(int connfd);
	//TcpClient自己定义的连接断开处理函数，注册到Connetor
	void removeConnect(TcpConnection *);

	EventLoop *loop_;
	Connector connector_;
	//Socket connSock_;  
	//断开tcp连接时，应删除对应的connPtr
	std::map<int, connPtr> connsMap_;
	connectCallBack onConnect_;
	disConnectCallBack onDisConnect_;
	messageCallBack onMessage_;
	completeCallBack onWriteComplete_;
};
