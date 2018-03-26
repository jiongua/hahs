/******************************************************************************
 * Filename : Channel.h
 * Copyright: 
 * Created  : 2018-1-10 by gongzenghao
 * Description: 
 ******************************************************************************/

//封装fd，fd上的事件类型，fd事件回调函数
//Channel的拥有者：
//Channel的生命期:
//Channel最终注册到EventLoop
#pragma once
#include <functional>
#include "noncopyable.h"

class EventLoop;

class Channel : noncopyable
{
public:
	typedef std::function<void()> HandleCallBack;
	Channel() = default;
	Channel(int fd, EventLoop *loop)
	:fd_(fd),
	 loop_(loop),
	 uevents_(kempty),
	 aevents_(kempty)
	{}

	~Channel();
	//note:
	//利用function&bind,替代虚函数实现"多态".
	void handleEvent();
	
	void setReadCallBack(HandleCallBack &&cb) {userHandleRead_ = cb;}
	void setWriteCallBack(HandleCallBack &&cb) {userHandleWrite_ = cb;}

	void enableRead() {	uevents_ |= kread; update();}
	void enableWrite() { uevents_ |= kwrite; update();}

	void disableRead() {uevents_ &= ~kread; update();}
	void disableWrite() {uevents_ &= ~kwrite; update();}
	void disableAll() {uevents_ = kempty; update();}

	void setActiveEvent(int event) {aevents_ |= event;}
	int fd() const {return fd_;}
	int uevents() const {return uevents_;}
	int aevents() const {return aevents_;}

	//判断用户是否注册了事件
	bool isEmpty() const {return uevents_ == kempty;}
	bool isReading() const {return uevents_ & kread;}
	bool isWriting() const {return uevents_ & kwrite;}

	//阻止拷贝
private:
	static const int kempty;
	static const int kread;
	static const int kwrite;
	
	void update();
	int fd_;
	EventLoop *loop_;
	int uevents_;	//user register
	int aevents_;	//active event
	//fixme
	HandleCallBack userHandleRead_;
	HandleCallBack userHandleWrite_;

};
