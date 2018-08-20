/******************************************************************************
 * Filename : example_echo.cpp
 * Copyright: 
 * Created  : 2018-2 -6 by gongzenghao
 * Description: 
 ******************************************************************************/
#include <unistd.h>
#include <cstring>

#include <iostream>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <fstream>

//#include "SocketOps.h"
#include "EventLoop.h"
#include "InetAddr.h"
//#include "Acceptor.h"
//#include "Buffer.h"
//#include "Channel.h"
#include "TcpConnection.h"
#include "EPoller.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "Thread.h"
#include "EventLoopThread.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

using std::cin;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using namespace hahs;

typedef std::shared_ptr<TcpConnection> connPtr;

class EchoServer
{
public:
	EchoServer(InetAddr *addr, EventLoop *loop)
	:server_(addr, loop)
	{
		//server_.setConnectCallBack(std::bind(&EchoServer::onConnect, this, _1));
		//server_.setDisConnectCallBack(std::bind(&EchoServer::onDisConnect, this));
		server_.setMessageCallBack(std::bind(&EchoServer::onMessage, this,_1, _2, _3));
	}

	void onMessage(connPtr conn, Buffer *rbuf, int len)
	{
		std::string msg = rbuf->getAllAsString();
		conn->send(msg);
	}
	
	void start()
	{
		server_.start();	
	}
	
private:
	TcpServer server_;
};


class EchoClient
{
public:
	EchoClient(InetAddr *addr, EventLoop *loop)
	:client_(addr, loop)
	{
		client_.setConnectCallBack(std::bind(&EchoClient::onConnect, this, _1));
		//client_.setDisConnectCallBack(std::bind(&EchoClient::onDisConnect, this));
		
		client_.setMessageCallBack(std::bind(&EchoClient::onMessage, this,_1, _2, _3));
		//client_.setWriteCompleteCallBack(std::bind(&EchoClient::onWriteComplete, this,_1));
	}
	//服务器tcp连接成功
	void onConnect(connPtr conn)
	{
		cout << "client connection ok" << endl;

		//set highWater
		//conn->setHighWaterCallBack(std::bind(&EchoClient::onHighWater, this, _1), 5*1024);
		
		std::string message = "hello world";
		if(conn->isConnected())
		{
			conn->send(message);
			cout << "send: " << message << endl;
		}	
	}

	void onMessage(connPtr conn, Buffer *rbuf, int len)
	{
		cout << "recv: " << rbuf->getAllAsString() << endl;
	}
	
	void start()
	{
		client_.start();	
	}
private:
	TcpClient client_;
	//int totalCount_;
};

int main(int argc, char *argv[])
{
	if(argc < 2) {
		cout << "./test -c"<<endl;
		cout << "./test -s" << endl;
		return 0;
	}
	
	if(strcmp(argv[1], "-s") == 0)
	{
		cout << "start server (in port 9002)"<<endl;
		EventLoop loop;
		InetAddr addr(9002);
		EchoServer server(&addr, &loop);
		server.start();
		loop.loop();
		
	}else if(strcmp(argv[1], "-c") == 0)
	{
		EventLoop loop;
		InetAddr remote("192.168.12.32", 9002);
		EchoClient cliet(&remote, &loop);
		cliet.start();
		loop.loop();
	}
	
}






