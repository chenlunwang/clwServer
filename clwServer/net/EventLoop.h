#pragma once
#include "../base/noncopyable.h"
#include "../base/Mutex.h"
#include "../base/CurrentThread.h"
#include <vector>
#include <memory>

namespace my_muduo
{
       class Channel;
       class Epoll;

       //EventLoop的主要功能是运行事件循环,必须由IO线程创建
       class EventLoop : noncopyable
       {
       public:
              typedef std::function<void()> Functor;

              EventLoop();
              ~EventLoop();

              void loop();
              void quit();
              void wakeup();

              void runInLoop(const Functor &cb);                                           //若用户在当前IO线程调用这个函数，回调会同步进行;若用户在其它线程调用，cb会被加入队列，等待IO线程被唤醒来调用
              void queueInLoop(const Functor &cb);                                         //将cb放入队列，必要时唤醒IO线程
              bool isInLoopThread() const { return threadId_ == CurrentThread::getpid(); } //判断EventLoop本对象是否在当前线程中

              void addChannel(Channel *channel);    //添加
              void modChannel(Channel *channel);    //修改
              void removeChannel(Channel *channel); //删除

       private:
              void doPendingFunctors();
              void handleRead();

              typedef std::vector<Channel *> ChannelList;
              ChannelList ActiveChannels_; //激活的事件集合

              const pid_t threadId_; //EventLoop所在线程ID
              bool looping_;
              bool quit_;
              bool isPendingFunctor_;
              int wakeupFd_;

              std::unique_ptr<Epoll> Epoller_;
              mutable MutexLock mutex_;
              std::vector<Functor> pendingFunctors;    //会暴露给其他线程，需要加锁
              std::unique_ptr<Channel> wakeupChannel_; //处理wakeupFd_的可读事件
       };

} // namespace my_muduo