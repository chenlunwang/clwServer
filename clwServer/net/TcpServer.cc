#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "TcpConnection.h"
#include "../base/Logging.h"
#include <functional>

using namespace my_muduo;

TcpServer::TcpServer(EventLoop *loop, int numThread, uint16_t port) : loop_(loop),
                                                                      acceptor_(new Acceptor(loop, port)),
                                                                      threadPool_(new EventLoopThreadPool(loop, numThread))
{
       acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::TcpServer(EventLoop *loop, int numThread, uint16_t port, const char *ip) : loop_(loop),
                                                                                      acceptor_(new Acceptor(loop, port, ip)),
                                                                                      threadPool_(new EventLoopThreadPool(loop, numThread))
{
       acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
       LOG_INFO << "TcpServer::~TcpServer success";
       for (auto &item : ConnectionMap)
       {
              TcpConnectionPtr conn(item.second);
              item.second.reset();
              conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
       }
}

void TcpServer::start()
{
       acceptor_->listen();  //监听新连接
       threadPool_->start(); //创建线程池
       loop_->loop();        //运行事件循环,loop_为主线程的loop
}

//处理新连接
void TcpServer::newConnection(int connfd, const sockaddr_in &peerAddr)
{
       LOG_INFO << "TcpServer::newConnection [" << connfd << "] begin ";
       EventLoop *ioLoop = threadPool_->getNextLoop(); //给新连接分配EventLoop,若numThread=0，ioloop就是主线程的loop
       TcpConnectionPtr connPtr = std::make_shared<TcpConnection>(ioLoop, connfd, peerAddr);
       ConnectionMap[connfd] = connPtr;

       connPtr->setMessageCallback(this->messageCallback_);
       //connPtr->setConnectionCallback(this->connectionCallback_);
       connPtr->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

       //唤醒ioloop线程
       ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, connPtr));
       LOG_INFO << "TcpServer::newConnection [" << connfd << "] end";
}

void TcpServer::removeConnection(const TcpConnectionPtr &connPtr)
{
       LOG_INFO << "TcpServer::removeConnection  [" << connPtr->fd() << "]";
       this->ConnectionMap.erase(connPtr->fd());
       EventLoop *ioLoop = connPtr->getLoop();
       ioLoop->runInLoop(std::bind(&TcpConnection::connectDestroyed, connPtr));
}