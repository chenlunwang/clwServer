#pragma once
#include "noncopyable.h"
#include "Mutex.h"
#include "condition.h"

//倒计时计数器类，当计数值大于0时休眠，等于0时唤醒所有线程
namespace my_muduo
{
       class CountDownLatch : noncopyable
       {
       public:
              explicit CountDownLatch(int count);
              ~CountDownLatch();

              void wait();          //主线程调用wait进入阻塞状态，等待计数器变为0
              void countDown();     //子线程调用countDown将计数器减1,计数器为0时唤醒所有线程
              int getCount() const; //获取计数器的值

       private:
              mutable MutexLock mutex_; //mutable可突破const的限制，永远处于可变的状态
              Condition cond_;
              int count_;
       };
} // namespace my_muduo