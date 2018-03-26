/******************************************************************************
 * Filename : TcpConnection.h
 * Copyright: 
 * Created  : 2018-1-19 by gongzenghao
 * Description: 
 ******************************************************************************/
//处理已连接的tcp的数据收发、断开
//由tcpServer创建并负责期生命期
//TcpConnection拥有connfd
#pragma once
#include <functional>
#include <memory>
#include "noncopyable.h"
#include "Buffer.h"
#include "Socket.h"
#include "Channel.h"

#include "CallBack.h"

//TcpConnection is managed by TcpServer
//using std::placeholders::_1;
//using std::placeholders::_2;

//class Channel;
class EventLoop;
class TcpConnection:public std::enable_shared_from_this<TcpConnection>, private noncopyable
{
public:
	//typedef std::shared_ptr<TcpConnection> connPtr;
	//typedef std::function<void(connPtr, Buffer*, int)> messagecallBack;
	//typedef std::function<void(const connPtr&)> completeCallBack;
	//typedef std::function<void(TcpConnection *)> disconnectCallBack;
	//typedef std::function<void(connPtr&)> highWaterCallBack;

	//created by TcpServer
	TcpConnection() = default;
	TcpConnection(int fd, EventLoop *loop);
	~TcpConnection();
	//注册到Channel的回调函数.
	void handleRead();
	void handleWrite();

	//供外部提供回调函数接口处理数据收发
	void setReadCallBack(messageCallBack &&cb) {onMessageRead_ = cb;}
	void setWriteCompleteCallBack(completeCallBack &&cb) {onWriteComplete_ = cb;}
	void setDisconnectCallBack(tcpDisconnectCallBack &&cb) {onDisconnect_ = cb;}

	//不在tcpClient中调用此函数，在用户注册的连接建立回调函数中调用
	//这样可以针对每条tcp连接设置不通的highWaterCallBack
	void setHighWaterCallBack(highWaterCallBack &&cb, size_t len)
	{
		onHighWater_ = cb;
		highWaterMark_ = len;
	}
	//发送，线程安全
	void send(const char * data, int len);
	void send(const std::string &data);
	void send(Buffer *data);

	//线程安全
	//sendInloop会注册为回调，应该声明为public
	void sendInloop(const char * data, int len);

	//主动关闭
	void activeClose();
	//如果调用线程不是I/O线程，则将操作转移给I/O线程
	void activeCloseInloop();
	//todo use move
	//void send(std::string &&data);

	int fd() const {return connSock_.fd();}
	bool isConnected() const {return state_ == connected;} 

	int writeableBytesOfoutBuffer() const
	{
		return outputBuffer_.writeableBytes();
	}
private:	

	void handleClose();
	void handleError();
	enum connect_state{connected = 1, disconnecting, disconnect};
	
	Socket connSock_;	//tcpConnection析构时析构,不手动管理connfd
	EventLoop *loop_;
	std::unique_ptr<Channel> channel_;
	int state_;
	messageCallBack onMessageRead_;
	completeCallBack onWriteComplete_;
	tcpDisconnectCallBack onDisconnect_;
	highWaterCallBack onHighWater_;
	//pointer to dynamic memory
	Buffer inputBuffer_;
	Buffer outputBuffer_;
	//限制最大缓冲，防止buffer无限扩大
	int highWaterMark_;
};
