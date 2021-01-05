#include "Channel.h"
#include <sys/epoll.h>
#include "../base/Logging.h"

using namespace my_muduo;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0)
{
}

void Channel::handleEvent()
{
       if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
       {
              LOG_WARN << "Channer::handleEvent EPOLLHUP" << 1 << " ";
              if (closecallback_)
                     closecallback_();
       }
       if (revents_ & EPOLLERR)
       {
              if (errorcallback_)
                     errorcallback_();
              return;
       }
       if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
       {
              if (readcallback_)
                     readcallback_();
       }
       if (revents_ & EPOLLOUT)
       {
              if (writecallback_)
                     writecallback_();
       }
}