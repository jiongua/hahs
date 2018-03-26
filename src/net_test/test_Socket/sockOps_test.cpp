/******************************************************************************
 * Filename : sockOps_test.cpp
 * Copyright: 
 * Created  : 2018-1-3 by gongzenghao
 * Description: test SocketOps/InetAddr
 ******************************************************************************/

#include "SocketOps.h"
#include "InetAddr.h"
#include <iostream>
#include <string.h>
#include <string>
#include <assert.h>
#include <errno.h>
#include <vector>

using namespace std;
const int MAX_LISTEN = 16;

typedef std::vector<char> Buffer;

int createTcpServerSimple(const InetAddr &localAddr)
{
	int listenfd = sockets::create(localAddr.family(), SOCK_STREAM);
	sockets::bindOrDie(listenfd, localAddr.getSockAddr());
	sockets::listenOrDie(listenfd, MAX_LISTEN);	
	return listenfd;
}

int createUdpServerSimple(const InetAddr &localAddr)
{
	int localfd = sockets::create(localAddr.family(), SOCK_DGRAM);
	sockets::bindOrDie(localfd, localAddr.getSockAddr());
	return localfd;
}

int userTcpOnMessage(Buffer *buf, int len, ofstream &outfile)
{
	return 0;
}

int userUdpOnMessage(void *buf, int len)
{
	//read from buffer
	//write to file
	return 0;
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
		char recv_buf[1024];
		
		int serverfd = sockets::create(AF_INET, SOCK_STREAM);
		//int serverfd = sockets::createNonblockingOrDie(AF_INET, SOCK_STREAM);
		assert(serverfd >= 0);
		/*struct sockaddr_in localAddr;
		struct sockaddr_in peerAddr;		
		bzero(&localAddr, sizeof(struct sockaddr_in));
		bzero(&peerAddr, sizeof(struct sockaddr_in));
		//must init localAddr to bind
		localAddr.sin_family = AF_INET;
		localAddr.sin_port = htons(atoi(argv[2]));
		//localAddr.sin_addr.s_addr =  htonl(0); 
		//localAddr.sin_addr.s_addr = inet_addr("192.168.12.32"); 
		*/
		InetAddr localAddr(port);
		//InetAddr peerAddr;
		struct sockaddr_in peerSockAddr;
		
		sockets::bindOrDie(serverfd, localAddr.getSockAddr());
		sockets::listenOrDie(serverfd, 256);
		
		int connfd = sockets::accept(serverfd, &peerSockAddr, 0);
		if(connfd < 0)
		{
			sockets::close(connfd);
			return 0;
		}
		
		int readLen = 0;

		//echo
		while(1)
		{
			readLen = sockets::read(connfd, recv_buf, 1024);
			cout << "recvlen =  " << readLen << endl;
			if(readLen < 0)
			{
				cerr << "recv error " << strerror(errno) << endl;
				break;
			}
			else if(readLen == 0)
			{
				sockets::close(connfd);
			}
			else
			{
				//echo
				cout << "recv: " << recv_buf << endl;
				int writeLen = sockets::write(connfd, recv_buf, readLen);
				cout << "writeLen = " << writeLen << endl;
			}
		}
	}else if(strcmp(argv[1], "-c") == 0)
	{
		cout << "input server ip:" <<endl;
		string ip;
		cin >> ip;
		cout << "input server port:" <<endl;
		int port;
		cin >> port;
		cout << "start client"<<endl;
		const string sendStr = "helloWorld";
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

		string word;
		char recv_buf[32];
		int sendLen = 0;
		int readLen = 0;
		while(cin >> word)
		{
			sendLen = sockets::write(clientfd, &word, word.size());
			cout << "send len =" << sendLen << "msg: " << word << endl;
			readLen = sockets::read(clientfd, recv_buf, 32);
			cout << "readLen  =" << readLen << "msg: " << recv_buf << endl;
		}
		cout << "cin end" << endl;
		sockets::close(clientfd);
	}
	
}
