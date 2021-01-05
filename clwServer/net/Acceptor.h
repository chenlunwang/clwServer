#pragma once
#include "../base/noncopyable.h"
#include "Socket.h"
#include "Channel.h"
#include <functional>
#include <memory>

namespace my_muduo
{
       class EventLoop;

       //用于accept新连接
       class Acceptor : noncopyable
       {
       public:
              typedef std::function<void(int sockfd, const struct sockaddr_in &peeraddr)> NewConnectionCallback;

              Acceptor(EventLoop *loop, uint16_t port);
              Acceptor(EventLoop *loop, uint16_t port, const char *ip);
              ~Acceptor();

              Channel *get_pointer(const std::unique_ptr<Channel> &ptr) { return ptr.get(); } //将智能指针转换为普通指针

              void setNewConnectionCallback(const NewConnectionCallback &cb) { NewConnectionCallback_ = cb; }
              void listen();

       private:
              void handleRead();

              EventLoop *loop_;
              std::unique_ptr<Socket> acceptSocket_;
              std::unique_ptr<Channel> acceptChannel_;
              NewConnectionCallback NewConnectionCallback_;
       };
} // namespace my_muduo