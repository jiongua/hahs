/******************************************************************************
 * Filename : ThreadPool_test.h
 * Copyright: 
 * Created  : 2018-2-10 by gongzenghao
 * Description: 
 ******************************************************************************/

#include <stdio.h>
#include <unistd.h>  // usleep
#include <string>
#include <iostream>

#include "ThreadPool.h"
#include "CurrentThread.h"
#include "CountDownLatch.h"

using std::cout;
using std::endl;
//任务1
void print()
{
  printf("tid=%d\n", hahs::CurrentThread::tid());
}

//任务2
void printString(const std::string& str)
{
  cout << str << endl;
  usleep(100*1000);
}

void test(int maxSize)
{
  //cout << "Test ThreadPool with max queue size = " << maxSize;
  ThreadPool pool(5, "MainThreadPool");
 // pool.setMaxQueueSize(maxSize);

  pool.start();

  cout << "Adding";
  //将任务print放入任务队列，并通知新建的5个线程
  pool.queueInPool(print);
  pool.queueInPool(print);

  //创建100个printString任务
  //可能会导致任务队列满
  for (int i = 0; i < 100; ++i)
  {
    char buf[32];
    snprintf(buf, sizeof buf, "task %d", i);
    pool.queueInPool(std::bind(printString, std::string(buf)));
  }
  cout << "Done";

  CountDownLatch latch(1);
  pool.queueInPool(std::bind(&CountDownLatch::countDown, &latch));
  cout << "wait........" << endl;
  latch.wait();
  
  cout << "end" << endl;
  //pool.stop();
}

int main()
{
  test(0);
  cout << "return"<<endl;
  //test(1);		//taskQueue numbers must be <= 1
  //test(5);		//taskQueue numbers must be <= 5
  //test(10);		//taskQueue numbers must be <= 10
  //test(50);		//taskQueue numbers must be <= 50
}

