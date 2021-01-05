#include "EventLoopThread.h"
#include "EventLoop.h"
#include "../base/Logging.h"

using namespace my_muduo;

EventLoopThread::EventLoopThread() : loop_(NULL),
                                     thread_(std::bind(&EventLoopThread::ThreadFunc, this)),
                                     mutex_(),
                                     cond_(mutex_)
{
}

EventLoopThread::~EventLoopThread()
{
       if (loop_ != NULL)
       {
              loop_->quit(); //退出循环，保证EventLoop生命期与线程主函数作用域相同
              thread_.join();
       }
}

//返回指向开始运行的EventLoop的指针loop_
EventLoop *EventLoopThread::startLoop()
{
       thread_.start(); //创建子线程,会自动调用threadFunc

       {
              MutexLockGuard lock(mutex_);
              while (loop_ == NULL)
              {
                     cond_.wait(); //等待子线程的创建与运行
              }
       }
       return loop_;
}

void EventLoopThread::ThreadFunc()
{
       EventLoop loop;
       {
              MutexLockGuard lock(mutex_);
              loop_ = &loop;
              cond_.signal(); //唤醒startLoop()
       }
       loop_->loop(); //运行子线程事件循环
       loop_ = NULL;
}
