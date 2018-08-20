/******************************************************************************
 * Filename : example_discard.cpp
 * Copyright: 
 * Created  : 2018-1-29 by gongzenghao
 * Description:
 	test tcpConnection/Buffer with Acceptor/Channel/EventLoop/Epoller
 ******************************************************************************/
#include <unistd.h>
#include <cstring>

#include <iostream>
#include <fstream>
#include <functional>
#include <memory>
#include <string>

#include "SocketOps.h"
#include "EventLoop.h"
#include "InetAddr.h"
#include "Acceptor.h"
#include "Buffer.h"
#include "Channel.h"
#include "TcpConnection.h"
#include "EPoller.h"
#include "TcpServer.h"
#include "Thread.h"

using std::placeholders::_1;

using std::cin;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using namespace hahs;

void serverMessageDiscard(std::shared_ptr<TcpConnection> conn, Buffer *rbuf, int len)
{
	//cout << "user onMessageRead: read from fd["<< conn->fd() << ",], len = " << len << endl;
	//从rbuf读完所有数据
	std::string message(rbuf->getAllAsString());
	cout << "[DISCARD]get msg_size[" << message.size() << "], msg[***********" << message << "**********]" << endl;

	//discard recv data
}


void serverMessageEcho(std::shared_ptr<TcpConnection> conn, Buffer *rbuf, int len)
{
	//cout << "user onMessageRead: read from fd["<< conn->fd() << ",], len = " << len << endl;
	//从rbuf读完所有数据
	std::string message(rbuf->getAllAsString());
	cout << "[ECHO]get msg_size[" << message.size() << "], msg[***********" << message << "**********]" << endl;
	
	//send to client
	conn->send(message);
}

void clientRecvEcho(std::shared_ptr<TcpConnection> conn, Buffer *buf, int len)
{
	std::string message(buf->getAllAsString());
	cout << "ECHO MSG[**********" << message << "***********]" << endl;
}

void clientDiscardThread(EventLoop *loop)
{
	InetAddr discard_server_addr("192.168.12.32", 9001);
	int discardFD = sockets::create(AF_INET, SOCK_STREAM);
		
	if(sockets::connectNonblocking(discardFD, discard_server_addr.getSockAddr()) != 0)
	{
		cout << "connect echo_server_addr error: " << errno << endl;
		sockets::close(discardFD);
		return;
	}
	
	std::shared_ptr<TcpConnection> connDiscard(new TcpConnection(discardFD, loop));
	
	const string message = "this is discard data";
	while(1) {
		connDiscard->send(message);
		cout << "client send[**********" <<  message <<  "************]" << endl;
		sleep(1);
	}
}

void clientEchoThread(EventLoop *loop)
{
	InetAddr echo_server_addr("192.168.12.32", 9002);
	int echoFD = sockets::create(AF_INET, SOCK_STREAM);
		
	if(sockets::connectNonblocking(echoFD, echo_server_addr.getSockAddr()) != 0)
	{
		cout << "connect echo_server_addr error: " << errno << endl;
		sockets::close(echoFD);
		return;
	}
	
	std::shared_ptr<TcpConnection> connEcho(new TcpConnection(echoFD, loop));

	connEcho->setReadCallBack(clientRecvEcho);
	
	const string message = "this is echo data";
	while(1) {
		connEcho->send(message);
		cout << "client send[**********" <<  message <<  "************]" << endl;
		sleep(3);
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2) {
		cout << "./test -c"<<endl;
		cout << "./test -s" << endl;
		return 0;
	}
	if(strcmp(argv[1], "-s") == 0)
	{
		cout << "start server (in port 9001/9002)"<<endl;
		EventLoop loop;
		InetAddr discard_addr(9001);
		TcpServer server_discard(&discard_addr, &loop);
		//server.setConnectCallBack(onConnection);
		server_discard.setMessageCallBack(serverMessageDiscard);

		InetAddr echo_addr(9002);
		TcpServer server_echo(&echo_addr, &loop);
		//server.setConnectCallBack(onConnection);
		server_echo.setMessageCallBack(serverMessageEcho);
		
		loop.loop();
	}else if(strcmp(argv[1], "-c") == 0)
	{
		EventLoop loop;		
		Thread thread_echo(std::bind(clientEchoThread, &loop), "thread_echo");
		Thread thread_discard(std::bind(clientDiscardThread, &loop), "thread_discard");

		thread_discard.start();
		thread_echo.start();
		loop.loop();
		thread_discard.join();
		thread_echo.join();
	}
	
}




