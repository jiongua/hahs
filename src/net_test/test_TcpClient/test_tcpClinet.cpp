/******************************************************************************
 * Filename : test_tcpClient.cpp
 * Copyright: 
 * Created  : 2018-2 -2 by gongzenghao
 * Description: test tcpClient/tcpServer
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

using std::cin;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using namespace hahs;

typedef std::shared_ptr<TcpConnection> connPtr;
std::ofstream outfile("./outfile.log", std::ios::out);
std::ifstream infile("./infile.log");


void serverOnMessage(connPtr conn, Buffer *rbuf, int len)
{
	//cout << "user onMessageRead: read from fd["<< conn->fd() << ",], len = " << len << endl;
	//从rbuf读完所有数据
	//std::string message(rbuf->getAllAsString());
	//cout << "[ECHO]get msg_size[" << message.size() << "], msg[***********" << message << "**********]" << endl;
	
	//send to client
	std::string message = rbuf->getAllAsString();
	//cout << "[ECHO]get msg_size[" << message.size() << "], msg[***********" << message << "**********]" << endl;

	if(conn->isConnected()) {
		conn->send(message);
		//cout << "[ECHO]send msg_size[" << message.size() << "], msg[***********" << message << "**********]" << endl;
	}
}

void serverOnConnect(connPtr conn)
{
	cout << "server connection ok !!" << endl;
}

void serverOndisConnect()
{
	cout << "server connection diconnect!!" << endl;
}

void clientEchoThread(connPtr conn)
{	
	//infile.open("infile");
	std::string line;
	while(getline(infile, line))
	{
		if(conn->isConnected()) {
			conn->send(line);
			cout << "send: " << line << endl;
			//sleep(1);
		}
		else
		{
			cerr<<"connection diconnect"<<endl;
			break;
		}
	}
	cout << "send finish" << endl;
}

void clientOnConnect(connPtr conn)
{
	cout << "client connection ok !!" << endl;
	Thread echo_thread(std::bind(clientEchoThread, conn));
	echo_thread.start();
}

void  clientOndisConnect()
{
	cout << " client connection diconnect!!" << endl;
}

void clientOnMessage(connPtr conn, Buffer *rbuf, int len)
{
	//outfile.open("outfile", std::ios::out);
	std::string message = rbuf->getAllAsString();
	//cout << "[ECHO]get msg_size[" << message.size() << "], msg[***********" << message << "**********]" << endl;
	outfile << message << endl;
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
		cout << "start server (in port 9002)"<<endl;
		EventLoop loop;
		InetAddr echo_addr(9002);
		TcpServer server_echo(&echo_addr, &loop);
		server_echo.setConnectCallBack(serverOnConnect);
		server_echo.setDisConnectCallBack(serverOndisConnect);
		server_echo.setMessageCallBack(serverOnMessage);
		server_echo.start();
		
		loop.loop();
		
	}else if(strcmp(argv[1], "-c") == 0)
	{
		EventLoop loop;		
		InetAddr server_addr("192.168.12.32", 9002);
		TcpClient client_echo(&server_addr, &loop);
		client_echo.setConnectCallBack(clientOnConnect);
		client_echo.setDisConnectCallBack(clientOndisConnect);
		client_echo.setMessageCallBack(clientOnMessage);

		client_echo.start();
		loop.loop();
	}
	
}





