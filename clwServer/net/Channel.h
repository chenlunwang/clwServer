#pragma once
#include "../base/noncopyable.h"
#include <functional>
//using namespace std;

namespace my_muduo
{
       class EventLoop;

       //channel为事件分发通道，并注册对应的回调函数
       class Channel : noncopyable
       {
       public:
              typedef std::function<void()> EventCallback;
              Channel(EventLoop *loop, int fd);

              //事件处理函数，Channel的核心
              void handleEvent();

              //事件回调函数
              void setReadCallback(const EventCallback &cb) { readcallback_ = cb; }
              void setWriteCallback(const EventCallback &cb) { writecallback_ = cb; }
              void setErrorCallback(const EventCallback &cb) { errorcallback_ = cb; }
              void setCloseCallback(const EventCallback &cb) { closecallback_ = cb; }

              int fd() const { return fd_; }                       //获取文件描述符
              int events() { return events_; }                     //获取关心的事件
              void setEvents(int events) { events_ = events; }     //设置关心的事件
              int revents() { return revents_; }                   //获取活跃的事件
              void setRevents(int revents) { revents_ = revents; } //设置活跃的事件

              // void add() { loop_->addChannel(this); }
              // void mod() { loop_->modChannel(this); }
              // void remove() { loop_->removeChannel(this); }

       private:
              EventLoop *loop_;
              const int fd_;
              int events_;  //用户关心的IO事件
              int revents_; //活跃的事件,由EventLoop/Epoller设置

              EventCallback readcallback_;
              EventCallback writecallback_;
              EventCallback errorcallback_;
              EventCallback closecallback_;
       };
} // namespace my_muduo