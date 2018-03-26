/******************************************************************************
 * Filename : CountDownLatch.h
 * Copyright: 
 * Created  : 2018-1-8 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once
#include "Condition.h"

class CountDownLatch : noncopyable
{
	public:
		explicit CountDownLatch(int thread_count)
		: mutex_(), cond_(mutex_),count_(thread_count)
		{
			//cout << "creat CountDownLatch"<<endl;
		}
		void wait()
		{
			MutexLockGuard lock(mutex_);
			while(count_)
			{
				cond_.wait();
			}
		}
		void countDown()
		{
			MutexLockGuard lock(mutex_);
			count_--;
			if(!count_)
			{
				cond_.notifyAll();
			}
		}
		int getCount() const
		{
			MutexLockGuard lock(mutex_);
			return count_;
		}
	private:
		
		//mutex_初始化顺序必须先于cond_
		mutable MutexLock mutex_;
		Condition cond_;
		int count_;
		//noncopyable
		//CountDownLatch(const CountDownLatch &rhs) = delete;
		//CountDownLatch& operator=(const CountDownLatch &rhs) = delete;
};