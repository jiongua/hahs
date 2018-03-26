/******************************************************************************
 * Filename : ThreadPool.h
 * Copyright: 
 * Created  : 2018-2-10 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once
#include "noncopyable.h"
#include "Thread.h"
#include "BlockingQueue.h"
#include "CountDownLatch.h"
#include <functional>
#include <vector>
#include <memory>
using namespace hahs;

//创建工作线程池
class ThreadPool:noncopyable
{
public:
	typedef std::function<void()> Task;
	explicit ThreadPool(int maxThreads, const std::string& name = std::string());

	//当ThreadPool析构前，应该终止所有线程执行.
	~ThreadPool()
	{
		cout << "~ThreadPool"<<endl;
		if(running_ == true)
		{
			stop();
		}
	}
	//启动所有线程
	void start();
	//等待所有线程结束
	void queueInPool(Task &&);

	void threadFunc();
private:
	int maxThreadNum_;
	const std::string name_;
	std::vector<std::unique_ptr<Thread>> threadQueue_;
	
	MutexLock mutex_;	
	bool running_;
	Condition notEmpty_;	//任务非空条件
	std::deque<Task> taskQueue_;	//尾进头取任务队列
	//不宜使用BlockingQueue
	//因为当ThreadPool析构时，需要通知所有线程.
	//而这些线程可能阻塞在等待任务，主线程不能使用BlockingQueue内部的条件变量通知
	//BlockingQueue<Task> taskQueue_;

	
	void stop();
	//从pool取走一个任务,由线程函数调用
	Task take();
};
