/******************************************************************************
 * Filename : example_sudoku.cpp
 * Copyright: 
 * Created  : 2018-2 -8 by gongzenghao
 * Description: 
 单线程soduku_server版本
 ******************************************************************************/
#include <unistd.h>
#include <cstring>

#include <iostream>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <fstream> 
#include <algorithm>

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

class sodokuServer
{
public:
	sodokuServer(InetAddr *addr, EventLoop *loop)
	:server_(addr, loop)
	{
		//server_.setConnectCallBack(std::bind(&EchoServer::onConnect, this, _1));
		//server_.setDisConnectCallBack(std::bind(&EchoServer::onDisConnect, this));
		server_.setMessageCallBack(std::bind(&sodokuServer::onMessage, this,_1, _2, _3));
	}
	
	void onMessage(connPtr conn, Buffer *rbuf, int len)
	{
	//要处理的问题:
	//1 如何处理多个请求？当缓冲区有多个请求消息数据确保要全部读取
	//2 非法请求的处理
	// i) 当请求类型不符
	//ii) 当请求数据字段长度非法(有结尾符但数据长度不符)
	//3) 数据不完整(无结尾符号)
	//4)回应的格式
		while(rbuf->readableBytes() >= kCell + 2)
		{
			const char *crlf = rbuf->findCRLF();
			if(crlf)
			{
			//找到，*crlf is '\r'

				string req(rbuf->peek(), crlf);//不含crlf
				if(!rbuf->setGetIndexOffset(req.size() + 2))
				{
					assert(0);
				}
				//find id
				string id;
				auto colon = std::find(req.begin(), req.end(), ':');
				if(colon != req.end())
				{
					id.assign(req.begin(), colon);//不含colon
					req.erase(req.begin(), colon+1);//含colon
					//req此时只包含有效的数据字段
				}
				//判断长度是否合法
				if(req.size() == kCell)
				{
				//如果solveSoduku耗时过长，将影响I/O主线程响应其他事件
				//应将其封装为函数扔到线程池中计算,使事件回调(onMessage)快速返回，将
				//控制权返还给loop
					string result = solveSoduku(req);
					if(id.empty())
					{
						conn->send(result + "\r\n");
					}
					else
					{						
						conn->send(id + ":" +result + "\r\n");
					}
				}
				else
				{
				//长度不合法
					cerr << "bad msg length" << endl;
					conn->send("error length\r\n");
					//断开连接？
				}
			}
			else
			{
				cout << "not find crlf" << endl;
				break;
			}
		}
		cout << "readableBytes == " << rbuf->readableBytes() << endl;
		
	}
	
	void start()
	{
		server_.start();	
	}
	
private:
	string solveSoduku(const string &puzzle)
	{
		return puzzle;
	}
	TcpServer server_;
	static const int kCell = 81;
};



class sudokuClient
{
public:
	sudokuClient(InetAddr *addr, EventLoop *loop)
	:client_(addr, loop)
	{
		client_.setConnectCallBack(std::bind(&sudokuClient::onConnect, this, _1));
		//client_.setDisConnectCallBack(std::bind(&EchoClient::onDisConnect, this));
		
		client_.setMessageCallBack(std::bind(&sudokuClient::onMessage, this,_1, _2, _3));
		//client_.setWriteCompleteCallBack(std::bind(&EchoClient::onWriteComplete, this,_1));
	}
	//服务器tcp连接成功
	void onConnect(connPtr conn)
	{
		cout << "client connection ok" << endl;

		//set highWater
		//conn->setHighWaterCallBack(std::bind(&EchoClient::onHighWater, this, _1), 5*1024);

		std::string request1 = "a:100000010400000000020000000000050407008000300001090000300400200050100000000806005\r\n";
		std::string request2 = "b:000000010400000000020000000000050407008000300001090000300400200050100000000806005\r\n";
		std::string request3 = "c:100000010400000000020000000000050407008000300001090000300400200050100000000806005\r\n";
		std::string request4 = "400000000020000000000050407008000300001090000300400200050100000000806005\r\n";


		if(conn->isConnected())
		{
			conn->send(request1 + request4 + request3 + request2);
			cout << "send:" <<endl;
			cout << request1 + request2 + request3 + request4 << endl;
		}	
	}

	void onMessage(connPtr conn, Buffer *rbuf, int len)
	{
		cout << "recv: " << endl;
		while(rbuf->readableBytes() > 0)
		{
			const char* crlf = rbuf->findCRLF();
			if(crlf)
			{
				//收到一个回应
				string res(rbuf->peek(), crlf);
				rbuf->setGetIndexOffset(res.size() + 2);
				cout << "res = " << res << endl;
			}
		}
		cout << "readableBytes == " << rbuf->readableBytes() << endl;
		
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
		cout << "start server (in port 9981)"<<endl;
		EventLoop loop;
		InetAddr addr(9981);
		sodokuServer server(&addr, &loop);
		server.start();
		loop.loop();
		
	}else if(strcmp(argv[1], "-c") == 0)
	{

		//for test
		/*string s1 = {"0123456"};
		const char arr[3] = "34";
		//[0,6)
		auto it = std::search(&s1[0], &s1[7], arr, arr+2);
		cout << "*it = " << *it << endl;
		string s2(s1.begin(), s1.end());
		cout << s2 << endl;
		return 0;
		*/
		EventLoop loop;
		InetAddr remote("192.168.12.32", 9981);
		sudokuClient client(&remote, &loop);
		client.start();
		loop.loop();
	}
	
}


