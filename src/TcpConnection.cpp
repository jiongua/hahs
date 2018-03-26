/******************************************************************************
 * Filename : TcpConnection.cpp
 * Copyright: 
 * Created  : 2018-1-19 by gongzenghao
 * Description: 
 ******************************************************************************/
#include "TcpConnection.h"
//#include "Channel.h"
#include "SocketOps.h"
#include "EventLoop.h"

//debug
#include <iostream>
#include <string.h>	//for strerror
using std::cerr;
using std::cout;
using std::endl;
#define __FUNC__  ((const char*) (__func__))

//created by TcpServer
TcpConnection::TcpConnection(int fd, EventLoop *loop)
:connSock_(fd),
 loop_(loop),
 channel_(new Channel(connSock_.fd(), loop_)),
 state_(connected)
{
	//TcpConnection负责channel_生命期
	channel_->setReadCallBack(std::bind(&TcpConnection::handleRead, this));
	channel_->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));

	channel_->enableRead();
}

TcpConnection::~TcpConnection()
{
	cout << "destruct tcpConnection" << endl;
	//if(connSock_.fd() > 0){
	//	sockets::close(connSock_.fd());
	//}
}

//called in Channel::handleEvent()
//run in loop()
void TcpConnection::handleRead()
{
	int n = inputBuffer_.readFD(connSock_.fd());
	//cout<< __FUNC__ << ": recv len = " << n << endl;

	if(n == 0)
	{
		//peer close
		//《UNP》：收到(call recv)FIN(放在TCP内核接收队列的最后)接收端执行"被动关闭"，read返回0
		//FIN在队列最后，上层收到FIN表示TCP内核接收队列已无数据，已被上层应用读取
		cout<< __FUNC__ << ": peer close" << endl;
		handleClose();
	}
	else if(n < 0)
	{
		//read error
		cerr << __FUNC__ << ": readFD error: " << strerror(errno) << endl;
	}
	else
	{
		//read ok
		//onMessageRead_不能阻塞
		//cout <<  __FUNC__  << ": call  onMessageRead_" << endl;
		//fixme: check if onMessageRead_ valid
		if(onMessageRead_) 
		{
			//cout << "call onMessageRead_" << endl;
			onMessageRead_(shared_from_this(), &inputBuffer_, n);
		}
	}
}

//可以被多线程同时访问同一个连接
void TcpConnection::send(const char * data, int len)
{
	if(state_ == connected){
		if(loop_->isInLoopThread())
		{
		//当前是I/O线程,在当前线程发送
			sendInloop(data, len);
		}
		else
		{
			//放到I/O线程中执行
			//使用shared_from_this(),防止回调前TcpConnection生命期终止
			loop_->queueInloop(std::bind(&TcpConnection::sendInloop, shared_from_this(), data, len));
		}
	}
}

//fixme1: 线程不安全(Buffer被多线程访问和修改，引起数据混乱)
//TODO1: 多个线程同时调用send，将操作传递给I/O线程去处理

//fixme2:发送端发送速度远高于接收端，将撑爆发送缓冲区outputBuffer_
//TODO2: 新增接口，预先设置高水位标志

//fixme3:send里直接回调了onWriteComplete_，onWriteComplete_函数体可能继续调用了send
//TODO3: 把onWriteComplete_转移到eventLoop

//TODO4:使用C++11 move避免拷贝
//question:什么时候直接写入socket，什么时候写入buffer
//answer:
//1)如果当前outputBuffer有数据待发送，则直接写入Buffer不可直接写入socket
//2)否则，调用直接写入socket,并判断返回值：
////i) -1且EWOULDBLOCK: 当前socket写缓存区已满，此时写入buffer并监听可写
////ii) 非负，实际写入socket写缓存字节数，(如果有)剩余的则写入outputbuffer并监听可写
void TcpConnection::sendInloop(const char * data, int len)
{
	if(outputBuffer_.readableBytes() > 0)
	{
		if(onHighWater_ && highWaterMark_)
		{
			if(len + outputBuffer_.readableBytes() > highWaterMark_)
			{
				//当前已是I/O线程
				//此时不直接回调onHighWater_,而是让下一次loop中调用
				loop_->queueInloop(std::bind(onHighWater_, 
											shared_from_this()));
				//return;
			}
			else
			{
				outputBuffer_.put(data, len);
				if(!channel_->isWriting())
				{
					channel_->enableWrite();
				}
			}
		}
		else
		{
			outputBuffer_.put(data, len);
			if(!channel_->isWriting())
			{
				channel_->enableWrite();
			}
		}
	}
	else
	{
		ssize_t sendedLen = sockets::sendNonblocking(connSock_.fd(), data, len);
		if(sendedLen < 0)
		{
			if(errno == EWOULDBLOCK)
			{
				//socket写缓存区已满,写入outputBuffer_
				outputBuffer_.put(data, len);
				channel_->enableWrite();
			}
			else
			{
				//other error
				//TODO
				cout << "other send error: " << strerror(errno) << endl;
			}
		}
		else if(0 < len - sendedLen)
		{
			//没有发完，剩余数据写入outBuffer
			if(onHighWater_ && highWaterMark_)
			{
				if(len + outputBuffer_.readableBytes() > highWaterMark_)
				{
					loop_->queueInloop(std::bind(onHighWater_, 
												shared_from_this()));
				}
				else
				{
					outputBuffer_.put(data, len);
					if(!channel_->isWriting())
					{
						channel_->enableWrite();
					}
				}
			}
			else
			{
				outputBuffer_.put(data + sendedLen, len - sendedLen);
				if(!channel_->isWriting())
				{
					channel_->enableWrite();
				}
			}
		}
		else if(0 == len - sendedLen)
		{
		//全部发出
			//BUG:不能在send里直接调用onWriteComplete_
			//如果在onWriteComplete_函数内部
			//调用了TcpConnection::send会引起递归调用
			//引用计数一直增长造成程序崩溃!
			if(onWriteComplete_) {
				//cout << __FUNC__<< ": call onWriteComplete_" << endl;
				loop_->queueInloop(std::bind(onWriteComplete_, 
											shared_from_this()));
			}
			else
			{
				//cout << __FUNC__<< ": no onWriteComplete_" << endl;
			}
		}
	}
}

void TcpConnection::send(const std::string &data)
{
	send(data.data(), data.size());
}

//user call like this
//Buffer buf;
//buf.putAsString();
//send(*buf);
//question:如何使用Buffer*避免0拷贝？如果
void TcpConnection::send(Buffer *data)
{
	
}

//会被多线程调用？是的
//todo: 
void TcpConnection::activeClose()
{
	if(loop_->isInLoopThread())
	{
		activeCloseInloop();
	}
	else
	{
		loop_->queueInloop(std::bind(&TcpConnection::activeCloseInloop, shared_from_this()));
	}
}

void TcpConnection::activeCloseInloop()
{
	//主动关闭连接，必须先发送outBuffer里的数据然后在close/shutdown
	if(outputBuffer_.readableBytes() > 0)
	{
		state_ = disconnecting;
		//等写缓存可写时在handleWrite里发送
		if(!channel_->isWriting())
		{
			channel_->enableWrite();
		}
	}
	else
	{
		state_ = disconnect;
		channel_->disableAll();
		//delete Connptr from tcpServer.
	}
}

//socket缓存可写,调用send
//当outputBuffer的数据全部写入socket写缓存后，关闭写监听防止loop-busy
//当outpuBuffer还有待发数据时，不关闭写监听等待下一次事件触发

//fixme:是否和send同时调用引起线程不安全？
//安全的.handleWrite实在I/O线程中调用，send实现了sendInLoop后是线程安全的
void TcpConnection::handleWrite()
{
	//for test
	//int nLeftBegin = outputBuffer_.readableBytes();
	
	ssize_t sendedLen = sockets::sendNonblocking(connSock_.fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
	assert(sendedLen >= 0);

	//update Buffer::getIndex_
	//fixme:如何有效且安全的更新getIndex_
	//线程安全问题？
	Buffer::pos curGetIndex = outputBuffer_.getIndex();
	//if(curGetIndex + sendedLen > outputBuffer_.putIndex_())
	outputBuffer_.setGetIndex(curGetIndex + sendedLen);
	
	assert(outputBuffer_.readableBytes() >= 0);
	//如果outputBuffer_发送完毕，则关闭监听可写
	//并回调onWriteComplete_
	if(0 == outputBuffer_.readableBytes())
	{	
		channel_->disableWrite();
		if(onWriteComplete_) {
			cout << __FUNC__<< ": call onWriteComplete_" << endl;
			//onWriteComplete_(shared_from_this());
			loop_->queueInloop(std::bind(onWriteComplete_, 
										shared_from_this()));
		}	
		
		//如果调用了主动关闭，buffer缓存发送完成后，关闭连接
		if(state_ == disconnecting)
		{
			//todo
		}
	}
	else
	{
		//还有待发数据
		//do nothing， 继续监听可写事件
		//cout << "begin == " <<nLeftBegin <<", nowleft = " << outputBuffer_.readableBytes() << ",sendThisTime = " << sendedLen << endl;
	}
}

//对端关闭连接:
//1 注销connfd上的所有事件
//2 需要销毁TcpConnection
void TcpConnection::handleClose()
{
	//sockets::close(connSock_.fd());
	/*if(channel_->isReading())
	{
		channel_->disableRead();
	}
	
	if(channel_->isWriting())
	{
		channel_->disableWrite();
	}*/
	
	//modify 20180129
	//处理tcpConnection管理的资源
	state_ = disconnect;
	channel_->disableAll();
	//因为tcpServer创建了TcpConnection，需要通知TcpServer做处理
	//多线程问题，TcpConnection在非主线程上创建
	if(onDisconnect_) {
		onDisconnect_(this);
	}
}


void TcpConnection::handleError()
{
//todo
}


