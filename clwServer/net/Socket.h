#pragma once
#include "../base/noncopyable.h"
#include <unistd.h>
#include <netinet/in.h>

namespace my_muduo
{
       class Socket : noncopyable
       {
       public:
              explicit Socket(int sockfd);
              explicit Socket();

              ~Socket();

              int fd() const { return sockfd_; }
              //用于服务器
              void bind(uint16_t port); //当IP为主机上任意IP时
              void bind(const char *ip, uint16_t port);
              void listen(int maxConn);
              int accept(struct sockaddr_in *peeraddr);

              //用于客户端
              void connect(const char *ip, uint16_t port);

              //关闭fd
              void close();

              void setTcpNoDelay(bool);
              void setReuseAddr(bool);
              void setReusePort(bool);
              void setKeepAlive(bool);

       private:
              int sockfd_;
       };
} // namespace my_muduo