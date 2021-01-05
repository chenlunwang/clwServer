#pragma once
#include "noncopyable.h"
#include <pthread.h>
#include "Logging.h"

namespace my_muduo
{
       class MutexLock : noncopyable
       {
       public:
              explicit MutexLock() : mutex(PTHREAD_MUTEX_INITIALIZER)
              {
                     pthread_mutex_init(&mutex, NULL);
              }

              ~MutexLock()
              {
                     pthread_mutex_destroy(&mutex);
              }
              //上锁
              void lock()
              {
                     pthread_mutex_lock(&mutex);
              }

              //解锁
              void unlock()
              {
                     pthread_mutex_unlock(&mutex);
              }

              //获取互斥锁
              pthread_mutex_t &GetMutex()
              {
                     return this->mutex;
              }

       private:
              pthread_mutex_t mutex; //互斥锁
       };

       class MutexLockGuard : noncopyable
       {
       public:
              explicit MutexLockGuard(MutexLock &mutex_) : mutex(mutex_)
              {
                     mutex.lock();
              }

              ~MutexLockGuard()
              {
                     mutex.unlock();
              }

       private:
              MutexLock &mutex;
       };

} // namespace my_muduo
