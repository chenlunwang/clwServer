#pragma once
#include "../base/noncopyable.h"
#include "TcpConnection.h"
#include <unistd.h>
#include <memory>
#include <map>

namespace my_muduo
{
       class Acceptor;
       class EventLoop;
       class EventLoopThreadPool;

       //处理新建TCP连接
       class TcpServer : public noncopyable
       {
       public:
              TcpServer(EventLoop *loop, int numThread, uint16_t port);
              TcpServer(EventLoop *loop, int numThread, uint16_t port, const char *ip);
              ~TcpServer();

              void start();

              void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
              //void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

       private:
              void newConnection(int sockfd, const struct sockaddr_in &peerAddr);
              void removeConnection(const TcpConnectionPtr &connPtr);

              std::map<int, TcpConnectionPtr> ConnectionMap;

              EventLoop *loop_;
              std::unique_ptr<Acceptor> acceptor_;
              std::unique_ptr<EventLoopThreadPool> threadPool_;

              MessageCallback messageCallback_;
              //ConnectionCallback connectionCallback_;
       };
} // namespace my_muduo