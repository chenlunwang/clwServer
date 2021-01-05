#include <stdio.h>
#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "base/Thread.h"
#include "base/Logging.h"

using namespace my_muduo;

class Server
{
public:
       Server(EventLoop *loop, int numThread, uint16_t port) : loop_(loop),
                                                               server_(loop, numThread, port)
       {
              server_.setMessageCallback(std::bind(&Server::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
       }

       Server(EventLoop *loop, int numThread, uint16_t port, const char *ip) : loop_(loop),
                                                                               server_(loop, numThread, port, ip)
       {
              server_.setMessageCallback(std::bind(&Server::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
       }

       void start() { server_.start(); }

private:
       void onMessage(const TcpConnectionPtr &connPtr, const char *buf, size_t len, int sockfd)
       {
              char temp_buf[65536];
              for (size_t j = 0; j < len; j++)
              {
                     temp_buf[j] = toupper(buf[j]);
              }
              sleep(2);
              write(sockfd, temp_buf, len);
              write(STDOUT_FILENO, temp_buf, len); //从buf中读取n个字节显示到屏幕上
       }

       EventLoop *loop_;
       TcpServer server_;
};

int main()
{
       LOG_INFO << "pid=" << getpid() << ",tid=" << CurrentThread::getpid();

       EventLoop loop;
       int numThreads = 2;   //线程数量
       uint16_t port = 6666; //端口号
       Server server(&loop, numThreads, port);

       server.start();
       //loop.loop();
       return 0;
}