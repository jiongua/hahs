/******************************************************************************
 * Filename : Thread_test0.cpp
 * Copyright: 
 * Created  : 2018-1-8 by gongzenghao
 * Description:  测试Thread类, 多线程轮流执行
 ******************************************************************************/
#include "Thread.h"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>

//#include <sys/syscall.h>
//#define gettid() syscall(SYS_gettid)

using namespace std;
using namespace hahs;

MutexLock g_mutex;
Condition g_cond(g_mutex);
int g_value = 0;
const int loopCnt = 10;

//user define, will be callback in new thread
void user_thread_A(void)
{
	//cout << "<<<<<start user_thread_A>>>>>>>" << endl;
	for(int i = 0; i < loopCnt; i++)
	{
		MutexLockGuard lock(g_mutex);
		//already lock
		while(g_value  % 3 != 0) 
		{
			g_cond.wait();				
		}
		//already lock
		cout << "A";
		g_value++;
		g_cond.notifyAll();
	}
	//unlock

	//cout << "<<<<<end user_thread_A>>>>>>>" << endl;
}

void user_thread_B(void)
{
	//cout << "<<<<<start user_thread_B>>>>>>>" << endl;
	for(int i = 0; i < loopCnt; i++)
	{
		MutexLockGuard lock(g_mutex);	
		//already lock
		while(g_value  % 3 != 1) 
		{
			g_cond.wait();				
		}
		//already lock
		cout << "B";
		g_value++;
		g_cond.notifyAll();
	}

	//cout << "<<<<<end user_thread_B>>>>>>>" << endl;
}

void user_thread_C(void)
{
	//cout << "<<<<<start user_thread_C>>>>>>>" << endl;
	for(int i = 0; i < loopCnt; i++)
	{
		MutexLockGuard lock(g_mutex);	
		//already lock
		while(g_value % 3 != 2) 
		{
			g_cond.wait();				
		}
		
		//already lock
		cout << "C";
		g_value++;
		//sleep(1);
		g_cond.notifyAll();
	}
	//cout << "<<<<<end user_thread_C>>>>>>>" << endl;
}

int main()
{
	//cout << "start main thread" << endl; 

	Thread thread(user_thread_A, "user_thread_A");
	thread.start();
	
	Thread thread2(user_thread_B, "user_thread_B");
	thread2.start();

	Thread thread3(user_thread_C, "user_thread_C");
	thread3.start();
	
	//cout << "main thread do sth" << endl; 

	//main thread wait util new thread exit
	thread.join();
	thread2.join();
	thread3.join();
	cout << endl;

	
}
