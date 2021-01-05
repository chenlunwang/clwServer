#include "HttpServer.h"

using namespace my_muduo;

void defaultHttpCallback(const HttpRequest &, HttpResponse *resp)
{
}

HttpServer::HttpServer(EventLoop *loop, int numThread, uint16_t port)
    : server_(loop, numThread, port),
      HttpCallback_(defaultHttpCallback)
{
       server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

HttpServer::HttpServer(EventLoop *loop, int numThread, uint16_t port, const char *ip)
    : server_(loop, numThread, port),
      HttpCallback_(defaultHttpCallback)
{
       server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void HttpServer::start()
{
       server_.start();
}

void onMessage(const TcpConnectionPtr &connPtr, const char *buf, size_t len, int sockfd)
{
       HttpRequest resquest;
       HttpResponse response;
}

void ParseResquest(const char *str, HttpRequest *request)
{
       if (request->getParseState() == Http_ResLine)
       {
              int pos = findCRLF(str);
       }
}

int findCRLF(const char *str)
{
       int pos = 0;
       do
       {
              if (str[pos] == '\r' && str[pos + 1] == '\n') //找到'\r\n'
                     return pos;
              pos++;
       } while (str[pos + 1] == '\0');

       return -1; //没有找到'\r\n'
}
