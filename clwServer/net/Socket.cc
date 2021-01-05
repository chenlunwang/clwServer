#include "Socket.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "../base/Logging.h"

using namespace my_muduo;

Socket::Socket(int sockfd)
{
       sockfd_ = sockfd;
}

Socket::Socket() //: sockfd_(sockfd)
{
       LOG_INFO << "Socket::Socket create listenfd";
       sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0); //建立一个套接字fd
       if (sockfd_ < 0)
       {
              LOG_ERROR << "Socket::socket error";
       }
}

Socket::~Socket()
{
       close();
}

void Socket::close()
{
       int ret = ::close(sockfd_);
       if (ret < 0)
       {
              LOG_ERROR << "Socket::close error";
       }
}

//当IP为主机上任意IP时
void Socket::bind(uint16_t port)
{
       struct sockaddr_in addr;
       memset(&addr, 0, sizeof addr);
       addr.sin_family = AF_INET;
       addr.sin_port = htons(port);
       addr.sin_addr.s_addr = htonl(INADDR_ANY); //主机上的任意IP地址

       int ret = ::bind(sockfd_, (sockaddr *)&addr, sizeof addr);
       if (ret < 0)
       {
              LOG_ERROR << "Socket::bind failed";
       }
}

//指定IP地址
void Socket::bind(const char *ip, uint16_t port)
{
       struct sockaddr_in addr;
       memset(&addr, 0, sizeof addr);
       addr.sin_family = AF_INET;
       addr.sin_port = htons(port);
       inet_pton(AF_INET, ip, &addr.sin_addr.s_addr); //将点分十进制转换为网络二进制
}

void Socket::listen(int maxConn)
{
       int ret = ::listen(sockfd_, maxConn);
       if (ret < 0)
       {
              LOG_ERROR << "Socket::listen error";
       }
}

int Socket::accept(struct sockaddr_in *peeraddr)
{
       socklen_t addrlen = sizeof(*peeraddr);
       int connfd = ::accept4(sockfd_, (struct sockaddr *)peeraddr, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC); //设置为非阻塞模式，若为accept需要用fcntl方式设置
       // if (connfd < 0)
       // {
       //        LOG_ERROR << "Socket::accept error" << errno;
       // }
       return connfd;
}

void Socket::connect(const char *ip, uint16_t port)
{
       struct sockaddr_in addr;
       memset(&addr, 0, sizeof addr);
       addr.sin_family = AF_INET;
       addr.sin_port = htons(port);
       inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);

       int ret = ::connect(sockfd_, (struct sockaddr *)&addr, sizeof addr);
       if (ret < 0)
       {
              LOG_ERROR << "Socket::connect failed";
              return;
       }
}

void Socket::setTcpNoDelay(bool on)
{
       int opt = on ? 1 : 0;
       ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::setReuseAddr(bool on)
{
       int opt = on ? 1 : 0;
       ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
       int opt = on ? 1 : 0;
       int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));

       if (ret < 0 && on)
       {
              LOG_ERROR << "SO_REUSEPORT failed";
       }
#else
       if (on)
       {
              LOG_ERROR << "SO_REUSEPORT error";
       }
#endif
}

void Socket::setKeepAlive(bool on)
{
       int opt = on ? 1 : 0;
       ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));
}
