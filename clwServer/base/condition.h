#pragma once
#include <stdio.h>
#include <errno.h>
#include "noncopyable.h"
#include "Mutex.h"

namespace my_muduo
{
       //互斥锁的条件变量类，提供信号机制
       class Condition : noncopyable
       {
       public:
              explicit Condition(MutexLock &mutex_) : mutex(mutex_), cond(PTHREAD_COND_INITIALIZER)
              {
                     ::pthread_cond_init(&cond, NULL);
              }

              ~Condition()
              {
                     ::pthread_cond_destroy(&cond);
              }

              //阻塞等待某个即将终止的线程发送信号到关联的条件变量
              void wait()
              {
                     int ret = ::pthread_cond_wait(&cond, &(mutex.GetMutex()));
                     if (ret != 0)
                            perror("pthread_cond_wait error");
              }

              //唤醒等在相应条件变量上的单个线程
              void signal()
              {
                     int ret = ::pthread_cond_signal(&cond);
                     if (ret != 0)
                            perror("pthread_cond_signal error");
              }

              //唤醒等在相应条件变量的所有线程
              void broadcast()
              {
                     int ret = ::pthread_cond_broadcast(&cond);
                     if (ret)
                            perror("pthread_cond_broadcast error");
              }

              //线程设置一个阻塞时间的限制
              bool timedwait(int seconds) //如果seconds秒内还没有事件发生，就返回true
              {
                     struct timespec time;
                     clock_gettime(CLOCK_REALTIME, &time);                                          //相当于gettimeofday
                     time.tv_sec += static_cast<time_t>(seconds);                                   //计算超时时刻
                     return ETIMEDOUT == ::pthread_cond_timedwait(&cond, &mutex.GetMutex(), &time); //若返回ETIMEDOUT，表示条件变量超时还没被唤醒，唤醒后重新尝试获取锁
              }

       private:
              MutexLock &mutex;
              pthread_cond_t cond;
       };
} // namespace my_muduo