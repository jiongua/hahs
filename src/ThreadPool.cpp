#include "ThreadPool.h"
#include "CurrentThread.h"


ThreadPool::ThreadPool(int maxThreads, const std::string& name)
:maxThreadNum_(maxThreads),
 name_(name),
 running_(false),
 notEmpty_(mutex_) 
{
	//创建n个线程对象
	for(int i = 0; i < maxThreadNum_; i++){
		//std::unique_ptr<Thread> ThreadPtr(new Thread(std::bind(&ThreadPool::threadFunc, this)));
		//threadQueue_.push_back(std::move(ThreadPtr));

		//use c++11 
		threadQueue_.emplace_back(new Thread(std::bind(&ThreadPool::threadFunc, this)));
	}
}
//线程安全
void ThreadPool::queueInPool(Task &&task)
{
	MutexLockGuard lock(mutex_);
	taskQueue_.push_back(std::move(task));
	notEmpty_.notifyAll();	
}

//启动所有线程
void ThreadPool::start()
{
	if(running_ == false){
		running_ = true;
		for(auto &threadPtr: threadQueue_){
			threadPtr->start();
			cout << "start " << threadPtr->tid() << endl;
		}
	}
}

void ThreadPool::stop()
{
	{
		MutexLockGuard lock(mutex_);
		running_ = false;
		notEmpty_.notifyAll();
	}
	
	//ThreadPool对象所在线程应等待子线程终止
	for(auto &threadPtr: threadQueue_){
		//cout << "main wait  " << threadPtr->tid() << endl;
		threadPtr->join();
	}
}
//called by threadFunc()
ThreadPool::Task ThreadPool::take()
{
	MutexLockGuard lock(mutex_);
	//把runninig_加入条件判断，是为了pool析构时能跳出阻塞
	while(running_ && taskQueue_.empty())
	{
		notEmpty_.wait();
	}
	Task cb;
	if(!taskQueue_.empty())
	{
		cb = taskQueue_.front();
		taskQueue_.pop_front();
	}
	return cb;
}

//线程函数
//各线程执行相同的操作
//todo:使各个线程执行不同的操作
void ThreadPool::threadFunc()
{

	//注意：
	//线程阻塞在take，持有cond
	//如果threadPool析构，将析构BlockingQueue->析构Condition，
	//此时pthread_cond_destroy将返回错误(EBUSY)

	//EBUSY:The implementation has detected an attempt to destroy the object referenced 
	//by cond while it is referenced (for example, while being used in a 
	//pthread_cond_wait() or pthread_cond_timedwait()) by another thread.
	while(running_){
		Task cb(take());//移动构造
		if(cb)
		{
			cb();
		}
	}
}


