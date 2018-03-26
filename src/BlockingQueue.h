/******************************************************************************
 * Filename : BlockingQueue.h
 * Copyright: 
 * Created  : 2018-2 - 9 by gongzenghao
 * Description: 
无界阻塞队列
 ******************************************************************************/
#pragma once
#include <deque>
#include <functional>

#include "noncopyable.h"
#include "Condition.h"

//for debug
#include <iostream>
using std::cout;
using std::endl;
#define __FUNC__  ((const char*) (__func__))

template <typename T>
class BlockingQueue
{
public:
	BlockingQueue()
	:notEmpty_(mutex_)
	{
	}

	//从队列取任务/数据
	//当没有任务时，或未获取锁时是阻塞的
	T take()
	{
		MutexLockGuard lock(mutex_);
		while(queue_.empty())
		{
			notEmpty_.wait();
		}
		//fixme: here is copy
		T t = queue_.front();
		queue_.pop_front();
		//fixme: here is copy
		return t;
	}
	
	void put(const T& t)
	{
		MutexLockGuard lock(mutex_);
		queue_.push_back(t);
		notEmpty_.notifyAll();
	}

	void put(T&& t)
	{
		MutexLockGuard lock(mutex_);
		queue_.push_back(std::move(t));
		notEmpty_.notifyAll();
	}

	
private:
//线程安全
	mutable MutexLock mutex_;
	Condition notEmpty_;
	//尾插头取
	std::deque<T> queue_;
};


//todo: use templete
//todo:线程安全
/*class TaskQueue:noncopyable
{
public:
	typedef std::function<void(void)> task;
	TaskQueue() = default;

	void push(const task &cb)
	{
		//cout << "call push(const task &cb)" << endl;
		MutexLockGuard lock(mutex_);
		tasks_.push_back(cb);
	}

	void push(task &&cb)
	{
		//cout << "call push(task &&cb)" << endl;
		MutexLockGuard lock(mutex_);
		tasks_.push_back(std::move(cb));
	}
	
	task pop()
	{
		//错误做法:如果tasks_为空将导致未定义行为
		
		task cb = tasks_.front();
		tasks_.pop_front();
		return cb;
		
	}

	int size() const
	{
		MutexLockGuard lock(mutex_);
		return tasks_.size();
	} 
private:
	mutable MutexLock mutex_;
	std::deque<task> tasks_;
};
*/


