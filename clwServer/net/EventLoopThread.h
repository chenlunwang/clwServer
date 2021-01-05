#include "../base/noncopyable.h"
#include "../base/condition.h"
#include "../base/Mutex.h"
#include "../base/Thread.h"

namespace my_muduo
{
       class EventLoop;

       //one loop one thread创建、销毁事件循环
       class EventLoopThread : public noncopyable
       {
       public:
              EventLoopThread();
              ~EventLoopThread();

              EventLoop *startLoop();

       private:
              void ThreadFunc();

              EventLoop *loop_;
              Thread thread_;
              MutexLock mutex_;
              Condition cond_;
       };
} // namespace my_muduo