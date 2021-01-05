#pragma once
#include "CountDownLatch.h"

using namespace my_muduo;

CountDownLatch::CountDownLatch(int count)
    : mutex_(),
      cond_(mutex_),
      count_(count)
{
}

CountDownLatch::~CountDownLatch() {}

//主线程调用wait进入阻塞状态，等待计数器变为0
void CountDownLatch::wait()
{
       MutexLockGuard lock(mutex_);
       while (count_ > 0) //count>0时就休眠
       {
              cond_.wait();
       }
}

//子线程调用countDown将计数器减1,计数器为0时唤醒所有线程
void CountDownLatch::countDown()
{
       MutexLockGuard lock(mutex_);
       --count_;
       if (count_ == 0)          //count=0时
              cond_.broadcast(); //唤醒所有线程
}

//获取计数器的值
int CountDownLatch::getCount() const
{
       MutexLockGuard lock(mutex_);
       return count_;
}