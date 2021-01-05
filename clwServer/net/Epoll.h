#pragma once
#include <vector>
#include <map>
#include <sys/epoll.h>
#include "EventLoop.h"
#include "../base/noncopyable.h"

namespace my_muduo
{
       class Channel;
       typedef std::vector<Channel *> ChannelList;

       //供所属的EventLoop在IO线程中调用，不用加锁
       class Epoll : noncopyable
       {
       public:
              Epoll();
              ~Epoll();

              //epoll处理文件描述符
              void epoll_add(Channel *channel);                          //添加新的文件描述符到红黑树中
              void epoll_mod(Channel *channel);                          //修改已经添加的事件
              void epoll_del(Channel *channel);                          //移除不再监听的事件
              void epoll_wait(int timeout, ChannelList &ActiveChannels); //阻塞等待事件的发生

       private:
              int epollfd_;                                 //文件描述符,红黑树的根节点
              std::vector<struct epoll_event> ActiveEvents; //存放活跃的事件
              std::map<int, Channel *> ChannelMap;          //存放epoll监听的channel事件
       };
} // namespace my_muduo