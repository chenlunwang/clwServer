#include "TcpServer.h"
#include <functional>
#include "HttpRe.h"
#include "../base/noncopyable.h"

namespace my_muduo
{
       class HttpRequest;
       class HttpResponse;

       typedef std::function<void(const HttpRequest &, HttpResponse *)> HttpCallback;

       class HttpServer : noncopyable
       {
       public:
              HttpServer(EventLoop *loop, int numThread, uint16_t port);
              HttpServer(EventLoop *loop, int numThread, uint16_t port, const char *ip);

              void start();

              void setHttpCallback(const HttpCallback &cb) { HttpCallback_ = cb; }

       private:
              void onMessage(const TcpConnectionPtr &connPtr, const char *buf, size_t len, int sockfd);

              TcpServer server_;
              HttpCallback HttpCallback_;
       };

} // namespace my_muduo