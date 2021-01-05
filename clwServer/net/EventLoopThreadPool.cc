#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"
#include "../base/Logging.h"

using namespace my_muduo;

EventLoopThreadPool ::EventLoopThreadPool(EventLoop *baseLoop, int numThread)
    : baseLoop_(baseLoop),
      numThread_(numThread),
      next_(0)
{
}

EventLoopThreadPool ::~EventLoopThreadPool()
{
}

void EventLoopThreadPool ::start()
{
       //started_ = true;
       LOG_INFO << "threadPool create begin";
       for (int i = 0; i < numThread_; i++)
       {
              // std::unique_ptr<EventLoopThread> tempThread(new EventLoopThread());
              // threads_.push_back(tempThread);
              //  loops_.push_back(tempThread->startLoop()); //添加开始运行的事件循环
              auto t = new EventLoopThread();
              threads_.push_back(std::unique_ptr<EventLoopThread>(t));
              loops_.push_back(t->startLoop());
       }
       LOG_INFO << "threadPool create finished";
}

//获取下一个运行的事件循环  轮询
EventLoop *EventLoopThreadPool ::getNextLoop()
{
       EventLoop *loop = baseLoop_;
       if (!loops_.empty())
       {
              loop = loops_[next_];
              ++next_;
              if (static_cast<size_t>(next_) >= loops_.size()) //size()的返回值为size_t类型
              {
                     next_ = 0;
              }
       }
       return loop;
}