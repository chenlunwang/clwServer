#include <functional>
#include <memory>

namespace my_muduo
{
       class TcpConnection;
       typedef std::shared_ptr<TcpConnection> TcpConnectionPtr; //TcpConnection的对象生命期是模糊的
       typedef std::function<void(const TcpConnectionPtr &, const char *, size_t, int)> MessageCallback;
       typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
       typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
} // namespace my_muduo