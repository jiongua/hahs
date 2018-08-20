/******************************************************************************
 * Filename : transferServer.h
 * Copyright: 
 * Created  : 2018-2-11by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once
//#includ  <functional>
#include <string>
#include "TcpServer.h"
#include "CallBack.h"
#include "noncopyable.h"
#include "ThreadPool.h"
#include "Condition.h"

//transferServer类
//
class EventLoop;
class Buffer;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class transferServer:noncopyable
{
public:
	transferServer(InetAddr *addr, EventLoop *loop, const std::string& fname)
	:server_(addr, loop),
	//tPool_(10),
	filename_(fname)
	{
		server_.setConnectCallBack(std::bind(&transferServer::onConnect, this, _1));
		//server_.setMessageCallBack(std::bind(&transferServer::onMessage, this, _1,_2,_3));
		//server_.setWriteCompleteCallBack(std::bind(&transferServer::onWriteComplete, this, _1));		
	}

	//bind to tcpServer
	void onConnect(connPtr);					 //某条新创建成功的连接

	void start()
	{
		server_.start();//start accept
		//tPool_.start();
	}


private:
	void fileTransTask(connPtr, const std::string&);
	
	TcpServer server_;	
	//ThreadPool tPool_;
	const std::string filename_;
};
