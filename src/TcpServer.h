/******************************************************************************
 * Filename : TcpServer.h
 * Copyright: 
 * Created  : 2018-1-29 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once
#include <functional>
#include <memory>
#include <map>
#include "Acceptor.h"
#include "CallBack.h"
#include "Mutex.h"
#include "EventLoopThreadPool.h"
class TcpConnection;
class Buffer;
class EventLoop;
using std::placeholders::_1;

class TcpServer:noncopyable
{
	public:
		//typedef std::shared_ptr<TcpConnection> connPtr;
		//typedef std::function<void(connPtr, Buffer*, int)> messagecallBack;
		//typedef std::function<void(connPtr)> userConnectCallBack;//for user
		//typedef std::function<void()> userDisConnectCallBack;//for user
		//typedef std::function<void(const connPtr&)> userCompleteCallBack;

		TcpServer() = default;
		TcpServer(InetAddr *addr, EventLoop *loop)
		:mainLoop_(loop),
		 acceptor_(addr, mainLoop_),
		 etPool_(10)		//10个子loop
		{
			acceptor_.setConnectionCallBack(std::bind(&TcpServer::onConnectEstablish, this, _1));
		}

		//user注册连接回调
		void setConnectCallBack(connectCallBack &&cb)
		{
			onConnect_ = cb;
		}
		//user注册消息回调
		void setMessageCallBack(messageCallBack &&cb)
		{
			onMessage_ = cb;
		}
		//user注册连接断开处理
		void setDisConnectCallBack(disConnectCallBack &&cb)
		{
			onDisConnect_ = cb;
		}

		//user注册写完成
		void setWriteCompleteCallBack(completeCallBack &&cb)
		{
			onWriteComplete_ = cb;
		}

		void start() 
		{
			acceptor_.start();
			etPool_.start();
		}
	private:
		//TcpServer自己定义的新连接处理函数，注册到Acceptor
		void onConnectEstablish(int connfd);
		//TcpServer自己定义的连接断开处理函数，注册到Acceptor
		void removeConnect(TcpConnection *);
	
		EventLoop *mainLoop_;
		Acceptor acceptor_;
		//int listenCount_;

		//tcpServer管理所有连接
		//新建连接时，新建tcpConnetion对象，将其插入到map
		//断开tcp连接时，从map删除对应的connPtr
		MutexLock connsMapMutex_;
		std::map<int, connPtr> connsMap_;
		connectCallBack onConnect_;
		disConnectCallBack onDisConnect_;
		messageCallBack onMessage_;
		completeCallBack onWriteComplete_;
		EventLoopThreadPool etPool_;
};
