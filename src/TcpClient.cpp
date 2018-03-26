/******************************************************************************
 * Filename : TcpClient.cpp
 * Copyright: 
 * Created  : 2018-2 -2 by gongzenghao
 * Description: 
 ******************************************************************************/
#include "TcpClient.h"
#include "TcpConnection.h"

#include <iostream>
//在Connetor中回调，处理新建立的tcp连接
void TcpClient::onConnectEstablish(int connfd)
{

	connPtr newConn(new TcpConnection(connfd, loop_));
	connsMap_.insert(make_pair(connfd, newConn));
	//fixme:
	//这里为每一个新建的连接创建了相同的回调函数
	newConn->setReadCallBack(std::move(onMessage_));
	newConn->setDisconnectCallBack(std::bind(&TcpClient::removeConnect, this, _1));
	newConn->setWriteCompleteCallBack(std::move(onWriteComplete_));

	if(onConnect_) 
	{	
		std::cout << "call userOnConnect_" << std::endl;
		onConnect_(newConn); 
	}

}

void TcpClient::removeConnect(TcpConnection *conn)
{
	//remove connPtr from connsMap
	int connFD =  conn->fd();
	if(connsMap_.find(connFD) != connsMap_.end())
	{
		connsMap_.erase(connFD);
	}
	else
	{
		assert(0);
	}
	
	if(onDisConnect_) onDisConnect_();
}




