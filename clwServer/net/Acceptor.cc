#include "Acceptor.h"
#include "EventLoop.h"
#include "../base/Logging.h"
#include "arpa/inet.h"
#include <sys/epoll.h>

using namespace my_muduo;

Acceptor::Acceptor(EventLoop *loop, uint16_t port) : loop_(loop),
                                                     acceptSocket_(new Socket),
                                                     acceptChannel_(new Channel(loop, acceptSocket_->fd()))
{
       acceptSocket_->setReuseAddr(true);
       acceptSocket_->setReusePort(true);
       acceptSocket_->bind(port); //此时ip为INADDR_ANY

       acceptChannel_->setEvents(EPOLLIN);
       acceptChannel_->setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::Acceptor(EventLoop *loop, uint16_t port, const char *ip) : loop_(loop),
                                                                     acceptSocket_(new Socket),
                                                                     acceptChannel_(new Channel(loop, acceptSocket_->fd()))
{
       acceptSocket_->setReuseAddr(true);
       acceptSocket_->setReusePort(true);
       acceptSocket_->bind(ip, port);

       acceptChannel_->setEvents(EPOLLIN);
       acceptChannel_->setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
       loop_->removeChannel(get_pointer(acceptChannel_)); //移除监听新连接的文件描述符listenfd
}

void Acceptor::listen()
{
       loop_->addChannel(get_pointer(acceptChannel_)); //将listenfd添加到红黑树中
       acceptSocket_->listen(1024);
       LOG_INFO << "Acceptor::listen finished";
}

void Acceptor::handleRead()
{
       char str;
       struct sockaddr_in clieAddr;

       //epoll为边沿模式下，采用非阻塞accept，在while循环内接受所有连接请求
       while (1)
       {
              int connfd = acceptSocket_->accept(&clieAddr); //传出客户端地址clieAddr,accept在Socket.cc中已定义为非阻塞模式
              LOG_INFO << "Acceptor::handleRead received from [" << inet_ntop(AF_INET, &clieAddr.sin_addr.s_addr, &str, sizeof(str))
                       << "] at port [" << ntohs(clieAddr.sin_port) << "]";

              if (connfd < 0)
              {
                     if (errno == EAGAIN)
                     {
                            LOG_INFO << "all connect requestions accepted";
                            break;
                     }
                     else if (errno == EINTR)
                            continue;
                     else
                     {
                            LOG_ERROR << "accept error";
                     }
              }

              if (NewConnectionCallback_)
              {
                     NewConnectionCallback_(connfd, clieAddr); //回调TcpConnection中的newConnection函数
              }
              else
              {
                     int ret = ::close(connfd);
                     if (ret < 0)
                     {
                            LOG_ERROR << "Acceptor::close error";
                     }
              }
       }
}