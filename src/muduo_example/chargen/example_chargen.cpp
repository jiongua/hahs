/******************************************************************************
 * Filename : example_chargen.cpp
 * Copyright: 
 * Created  : 2018-2 -3 by gongzenghao
 * Description: 
 TCP连接建立后,服务器不断传送任意的字符到客户端，直到客户端关闭连接。
 服务器只发不收，发送速度不能快于接收速度
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

class ChargenServer
{
public:
	ChargenServer(InetAddr *addr, EventLoop *loop)
	:server_(addr, loop)
	{
		server_.setConnectCallBack(std::bind(&ChargenServer::onConnect, this, _1));
		server_.setDisConnectCallBack(std::bind(&ChargenServer::onDisConnect, this));
		server_.setMessageCallBack(std::bind(&ChargenServer::onMessage, this,_1, _2, _3));
		
	}
	//服务器tcp连接成功
	void onConnect(connPtr conn)
	{
		cout << "server connection ok" << endl;
	}
	
	//对端关闭连接
	void onDisConnect()
	{
		cout << "server connection diconnect!!" << endl;
	}

	void onMessage(connPtr conn, Buffer *rbuf, int len)
	{
		std::string msg = rbuf->getAllAsString();
		//discard
	}
	void start()
	{
		server_.start();	
	}
private:
	TcpServer server_;
};


class ChargenClient
{
public:
	ChargenClient(InetAddr *addr, EventLoop *loop)
	:client_(addr, loop)
	{
		client_.setConnectCallBack(std::bind(&ChargenClient::onConnect, this, _1));
		client_.setDisConnectCallBack(std::bind(&ChargenClient::onDisConnect, this));
		
		client_.setMessageCallBack(std::bind(&ChargenClient::onMessage, this,_1, _2, _3));
		client_.setWriteCompleteCallBack(std::bind(&ChargenClient::onWriteComplete, this,_1));
	}
	//服务器tcp连接成功
	void onConnect(connPtr conn)
	{
		cout << "client connection ok" << endl;

		//set highWater
		conn->setHighWaterCallBack(std::bind(&ChargenClient::onHighWater, this, _1), 5*1024);
		
		std::string message = "1234567";
		if(conn->isConnected())
		{
			conn->send(message);
		}
	}
	
	//服务器tcp连接断开
	void onDisConnect()
	{
		cout << "clinet connection diconnect!!" << endl;
	}

	void onWriteComplete(const connPtr& conn)
	{
		cout << "client write complete !!" << endl;
		const std::string message(1024,'a');
		//int shared_count = conn.use_count();
		//cout << "count = " << shared_count << endl;
		
		if(conn->isConnected())
		{
			conn->send(message);
		}
	}
	
	//just check onHighWater is work or not
	void onHighWater(const connPtr& conn)
	{
		cout << "client onHighWater !!" << endl;
	}

	void onMessage(connPtr conn, Buffer *rbuf, int len)
	{
		cout << "don't hope this call:onMessage" << endl;
		std::string msg = rbuf->getAllAsString();
		//discard
	}
	void start()
	{
		client_.start();	
	}
private:
	TcpClient client_;
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
		ChargenServer server(&addr, &loop);
		server.start();
		loop.loop();
		
	}else if(strcmp(argv[1], "-c") == 0)
	{
		EventLoop loop;		
		InetAddr remote_addr("192.168.12.32", 9002);
		ChargenClient client(&remote_addr, &loop);

		client.start();
		loop.loop();
	}
	
}





