/******************************************************************************
 * Filename : EventLoop_test1.cpp
 * Copyright: 
 * Created  : 2018-1-17 by gongzenghao
 * Description: test EventLoop/Channel/EPoller
 ******************************************************************************/
#include "SocketOps.h"
#include "EventLoop.h"
#include "Channel.h"
#include "EPoller.h"
#include "Thread.h"
#include "InetAddr.h"

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
const int MAX_LISTEN = 16;

void onMessageRead(int fd)
{
	cout << "call onMessageRead....."<<endl;
	char recv_buf[64];
	int recvLen = sockets::read(fd, recv_buf, 64);
	cout << "recv len === " << recvLen << endl;
}

void onConnection(int fd)
{
	cout  << "call onConnection" << endl;
	
	struct sockaddr_in peerAddr;
	int connfd = sockets::accept(fd, &peerAddr, 1);
	if(connfd < 0)
	{
		sockets::close(connfd);
		return;
	}
	
	cout << "accpet ok "<< endl;
	
	Channel* msgEvent = new Channel(connfd, &g_loop);
	std::function<void()> factor = std::bind(&onMessageRead, connfd);
	msgEvent->setReadCallBack(factor);
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
		//char recv_buf[1024];

		InetAddr localAddr(port);
		int listenfd = sockets::createNonblockingOrDie(localAddr.family(), SOCK_STREAM);
		sockets::bindOrDie(listenfd, localAddr.getSockAddr());
		sockets::listenOrDie(listenfd, MAX_LISTEN); 

		Channel listenEvent(listenfd, &g_loop);
		std::function<void()> factor = std::bind(&onConnection, listenfd);
		listenEvent.setReadCallBack(factor);
		listenEvent.enableRead();

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

		/*struct sockaddr_in sereverAddr;
		bzero(&sereverAddr, sizeof(struct sockaddr_in));
		sereverAddr.sin_family = AF_INET;
		sereverAddr.sin_addr.s_addr = inet_addr(argv[2]);
		sereverAddr.sin_port = htons(atoi(argv[3])); 
		*/
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


