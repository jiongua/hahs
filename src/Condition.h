/******************************************************************************
 * Filename : Condition.h
 * Copyright: 
 * Created  : 2018-1-8 by gongzenghao
 * Description: 
 ******************************************************************************/
#pragma once

#include "Mutex.h"

class Condition:noncopyable
{
	public:
		//note：MutexLock is noncopyable, can't passed by value
		explicit Condition(MutexLock &mtx):mutex_(mtx)
		{
			MCHECK(pthread_cond_init(&cont_, NULL));
		}
		~Condition()
		{
			MCHECK(pthread_cond_destroy(&cont_));
		}
		void notifyAll()
		{
			//外部加锁解锁
			MCHECK(pthread_cond_broadcast(&cont_));
		}
		void notify()
		{
			//外部加锁解锁
			MCHECK(pthread_cond_signal(&cont_));
		}
		
		void wait()
		{
			//外部加锁解锁
			MCHECK(pthread_cond_wait(&cont_, mutex_.getPthreadMutex()));
		}
		//todo
		void waitTime(int ms){}
	private:
		MutexLock &mutex_;//Condition内部用锁与外部一致
		pthread_cond_t cont_;
		
		//noncopyable, Condition可通过继承noncopyable类代替
		//Condition(const Condition &rhs) = delete;
		//Condition& operator=(const Condition &rhs) = delete;
};

