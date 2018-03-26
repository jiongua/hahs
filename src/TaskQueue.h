/******************************************************************************
 * Filename : TaskQueue.h
 * Copyright: 
 * Created  : 2018-2 - 5 by gongzenghao
 * Description: 
 无界任务队列，主要转移函数对象的执行,或者线程池
 ******************************************************************************/
#pragma once
#include <deque>
#include <functional>

#include "noncopyable.h"
#include "Mutex.h"

//for debug
#include <iostream>
using std::cout;
using std::endl;

#define __FUNC__  ((const char*) (__func__))

//todo: use templete
//todo:线程安全
class TaskQueue:noncopyable
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

