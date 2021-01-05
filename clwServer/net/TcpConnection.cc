#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "../base/Logging.h"
#include "arpa/inet.h"
#include <sys/epoll.h>
#include <fcntl.h>

using namespace my_muduo;

TcpConnection::TcpConnection(EventLoop *loop, int connfd, const sockaddr_in &peerAddr)
    : loop_(loop),
      connfd_(connfd),
      peerAddr_(peerAddr),
      channel_(new Channel(loop, connfd))
{
       channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
       //channel_->setWriteCallback(std::bind(&TcpConnection ::handleWrite, this));
       channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
       channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));

       inet_ntop(AF_INET, &peerAddr_.sin_addr.s_addr, peerIp_, sizeof peerIp_); //获取客户端的ip地址
       peerPort_ = ntohs(peerAddr_.sin_port);                                   //获取客户端port
       LOG_INFO << "new Connection: [" << peerIp_ << ":" << peerPort_ << "] at [" << connfd_ << "]";

       //socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
       LOG_INFO << "Connection close: [" << peerIp_ << ":" << peerPort_ << "] at [" << connfd_ << "]";
       ::close(connfd_);
}

//新连接建立时调用
void TcpConnection::connectEstablished()
{
       channel_->setEvents(EPOLLIN);
       loop_->addChannel(get_pointer(channel_)); //将新连接添加到红黑树中
       LOG_INFO << "TcpConnection::connectEstablished finished";
}

//连接断开时调用
void TcpConnection::connectDestroyed()
{
       loop_->removeChannel(get_pointer(channel_)); //将连接从树中去除
       LOG_INFO << "TcpConnection::connectDestroyed finished";
}

void TcpConnection::handleRead()
{
       LOG_INFO << "TcpConnection::handleRead begin";
       char buf[65536];
       while (1)
       {
              ssize_t n = ::read(channel_->fd(), buf, sizeof buf); //此处后续需进行改进
              if (n > 0)
              {
                     MessageCallback_(TcpConnection::shared_from_this(), buf, n, channel_->fd()); //TcpConnection::shared_from_this()产生一个新shared_ptr对象
              }
              else if (n == 0)
              {
                     handleClose();
              }
              else
              {
                     if (errno == EAGAIN)
                     {
                            LOG_INFO << "read all";
                            break;
                     }
                     else if (errno == EINTR)
                            continue;
                     handleError();
              }
       }
}

void TcpConnection::handleWrite()
{
       LOG_INFO << "TcpConnection::handleWrite begin";
}

void TcpConnection::handleClose()
{
       LOG_INFO << "TcpConnection::handleClose begin";
       CloseCallback_(shared_from_this()); //回调TcpServer::removeConnection，进而回调connectDestroyed函数，将已关闭的连接移除
}

void TcpConnection::handleError()
{
       LOG_INFO << "TcpConnection::handleError begin";
       int err;
       socklen_t optlen = sizeof(err);
       if (::getsockopt(connfd_, SOL_SOCKET, SO_ERROR, &err, &optlen) < 0)
       {
              LOG_ERROR << "getsockopt error at fd: [" << connfd_ << "]";
       }
       else
       {
              LOG_ERROR << "TcpConnection::handleError [" << peerIp_ << ":" << peerPort_ << "]-SO_ERROR=" << err << " " << strerror(err);
       }
}