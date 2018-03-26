/******************************************************************************
 * Filename : accptor_unittest.cpp
 * Copyright: 
 * Created  : 2018-1-19 by gongzenghao
 * Description: test Accptor with EventLoop/Channel
 ******************************************************************************/
#include "SocketOps.h"
#include "EventLoop.h"
#include "Channel.h"
#include "EPoller.h"
#include "InetAddr.h"
#include "Acceptor.h"
#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <functional>

using std::cin;
using std::cerr;
using std::cout;
using std::endl;

EventLoop g_loop;

void onMessageRead(Channel* channel)
{
	cout << "call onMessageRead....."<<endl;
	char recv_buf[64];
	int fd = channel->fd();
	int recvLen = sockets::read(fd, recv_buf, 64);
	cout << "recv len === " << recvLen << endl;
	if(recvLen == 0)
	{
		cout << "client exit" << endl;
		channel->disableAll();
		sockets::close(fd);
	}
}

void onConnection(int connfd)
{
	cout  << "user call onConnection" << endl;
	if(connfd < 0)
	{
		cerr  << "connfd error" << endl;
		return;
	}
	
	Channel* msgEvent = new Channel(connfd, &g_loop);
	msgEvent->setReadCallBack(std::bind(&onMessageRead, msgEvent));
	msgEvent->enableRead();
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
		cout << "input server_local port:" <<endl;
		int port;
		cin >> port;
		
		cout << "start server"<<endl;
		InetAddr localAddr(port);
		Acceptor acceptor(&localAddr, &g_loop);
		acceptor.setConnectionCallBack(onConnection);

		g_loop.loop();
	}else if(strcmp(argv[1], "-c") == 0)
	{
		cout << "input server ip:" <<endl;
		std::string ip;
		cin >> ip;
		cout << "input server port:" <<endl;
		int port;
		cin >> port;
		cout << "start client"<<endl;
		const std::string sendStr = "helloWorld";
		//char recv_buf[1024];
		
		int clientfd = sockets::create(AF_INET, SOCK_STREAM);
		//int clientfd = sockets::createNonblockingOrDie(AF_INET, SOCK_STREAM);
		assert(clientfd >= 0);

		InetAddr sereverAddr(ip, port);
		if(sockets::connect(clientfd, sereverAddr.getSockAddr()) != 0)
		{
			cout << "connect error: " << errno << endl;
			sockets::close(clientfd);
			return 0;
		}
		cout << "connect ok" << endl;

		std::string word;
		//char recv_buf[32];
		int sendLen = 0;
		//int readLen = 0;
		while(cin >> word)
		{
			sendLen = sockets::write(clientfd, &word, word.size());
			cout << "send len =" << sendLen << "msg: " << word << endl;
			//readLen = sockets::read(clientfd, recv_buf, 32);
			//cout << "readLen  =" << readLen << "msg: " << recv_buf << endl;
		}
		cout << "cin end" << endl;
		
		sockets::close(clientfd);
	}
	
}



