#pragma once
#include "../base/noncopyable.h"
#include "Channel.h"
#include "TcpCallbacks.h"
#include <netinet/in.h>
#include <memory>
#include <functional>

namespace my_muduo
{
       class EventLoop;
       class Socket;

       //处理已连接描述符的相关操作，如关闭连接、读、写、error等
       class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> //调用T::shared_from_this会产生一个新的shared_ptr<T>对象
       {
       public:
              TcpConnection(EventLoop *loop, int sockfd, const sockaddr_in &peerAddr); //sockfd为已连接描述符
              ~TcpConnection();

              EventLoop *getLoop() { return loop_; }
              const char *peerIp() { return peerIp_; }  //获取已连接客户端的ip地址
              uint16_t peerPort() { return peerPort_; } //获取已连接客户端的端口号port
              int fd() { return connfd_; }
              Channel *get_pointer(const std::unique_ptr<Channel> &ptr) { return ptr.get(); } //将智能指针转换为普通指针

              void setMessageCallback(const MessageCallback &cb) { MessageCallback_ = cb; }
              void setCloseCallback(const CloseCallback &cb) { CloseCallback_ = cb; }
              //void setConnectionCallback(const ConnectionCallback &cb) { ConnectionCallback_ = cb; }

              void connectEstablished(); //新连接建立时调用
              void connectDestroyed();   //连接断开时调用

       private:
              void handleRead();
              void handleWrite();
              void handleClose();
              void handleError();

              EventLoop *loop_;
              std::unique_ptr<Channel> channel_; //获取socket_上的IO事件

              int connfd_;
              const sockaddr_in peerAddr_;
              char *peerIp_;
              uint16_t peerPort_;

              //MessageCallback在TcpServer中定义
              MessageCallback MessageCallback_;
              CloseCallback CloseCallback_;
              //ConnectionCallback ConnectionCallback_;
       };
} // namespace my_muduo