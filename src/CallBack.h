/******************************************************************************
 * Filename : CallBack.h
 * Copyright: 
 * Created  : 2018-2-11 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once
#include <functional>
#include <memory>

//don't include !
//#include "TcpConnection.h"
//#include "Buffer.h"

class Buffer;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> connPtr;
typedef std::function<void(connPtr, Buffer*, int)> messageCallBack;
typedef std::function<void(connPtr)> connectCallBack;
typedef std::function<void()> disConnectCallBack;
typedef std::function<void(TcpConnection *)> tcpDisconnectCallBack;

typedef std::function<void(const connPtr&)> completeCallBack;
typedef std::function<void(connPtr&)> highWaterCallBack;


