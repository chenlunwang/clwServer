#pragma once
#include "../base/noncopyable.h"
#include <functional>
#include <memory>
#include <vector>

namespace my_muduo
{
       class EventLoop;
       class EventLoopThread;

       class EventLoopThreadPool : public noncopyable
       {
       public:
              EventLoopThreadPool(EventLoop *baseLoop, int numThread);
              ~EventLoopThreadPool();

              void start();
              EventLoop *getNextLoop();
              //bool started() { return started_; }

       private:
              EventLoop *baseLoop_;
              int numThread_;
              int next_;
              std::vector<std::unique_ptr<EventLoopThread>> threads_;
              std::vector<EventLoop *> loops_;
       };
} // namespace my_muduo