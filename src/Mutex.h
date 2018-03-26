/******************************************************************************
 * Filename : Mutex.h
 * Copyright: 
 * Created  : 2018-1-2 by gongzenghao
 * Description: 封装pthread_mutex*
 ******************************************************************************/
#pragma once

#include <assert.h>
#include <pthread.h>
#include "noncopyable.h"

#ifdef CHECK_PTHREAD_RETURN_VALUE

#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail (int errnum,
                                  const char *file,
                                  unsigned int line,
                                  const char *function)
    __THROW __attribute__ ((__noreturn__));
__END_DECLS
#endif

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       if (__builtin_expect(errnum != 0, 0))    \
                         __assert_perror_fail (errnum, __FILE__, __LINE__, __func__);})

#else  // CHECK_PTHREAD_RETURN_VALUE

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       assert(errnum == 0); (void) errnum;})

#endif // CHECK_PTHREAD_RETURN_VALUE

class MutexLock:noncopyable
{
	public:
	MutexLock()
	{
		//todo set pthread_mutexatrr
		MCHECK(pthread_mutex_init(&mutex_,NULL));
	}
	~MutexLock(){
		pthread_mutex_destroy(&mutex_);
	}
	//only used for MutexLockGuard use
	void lock()
	{
		MCHECK(pthread_mutex_lock(&mutex_));
	}
	void unlock()
	{
		MCHECK(pthread_mutex_unlock(&mutex_));
	}
	
	//for condition
	pthread_mutex_t *getPthreadMutex() /* non-const ！*/
	{
		return &mutex_;
	}
	
	//todo assign holder
	private:
		pthread_mutex_t mutex_;
		//noncopyable
		//MutexLock(const MutexLock &rhs) = delete;
		//MutexLock& operator=(const MutexLock &rhs) = delete;
};

//manage Mutex
class MutexLockGuard:noncopyable
{
	public:
	//note：MutexLock is noncopyable, can't passed by value
		explicit MutexLockGuard(MutexLock &mutex)
			:mtx_(mutex)
		{
			mtx_.lock();
		}
		~MutexLockGuard(){
			mtx_.unlock();
		}
	private:
		MutexLock &mtx_;//mtx_ must be reference because it's noncopyable
		//noncopyable
		//MutexLockGuard(const MutexLockGuard &rhs) = delete;
		//MutexLockGuard& operator=(const MutexLockGuard &rhs) = delete;
};