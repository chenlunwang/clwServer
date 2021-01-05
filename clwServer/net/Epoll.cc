#include "Epoll.h"
#include <sys/epoll.h>
#include <unistd.h>
#include "Channel.h"
#include "../base/Logging.h"

using namespace my_muduo;

int MAXEVENTS = 1024; //最大可存放活跃的事件数量

Epoll::Epoll()
    : epollfd_(::epoll_create1(EPOLL_CLOEXEC)), //创建文件描述符
      ActiveEvents(MAXEVENTS)
// ActiveChannels(MAXEVENTS * 2)
{
       if (epollfd_ < 0)
              LOG_ERROR << "Epoll::epoll_create1 error";
       // ::abort(); //异常终止一个进程
}

Epoll::~Epoll()
{
       ::close(epollfd_);
}

//添加新的文件描述符到红黑树中  updateChannel
void Epoll::epoll_add(Channel *channel)
{
       int fd = channel->fd();
       struct epoll_event ev;
       memset(&ev, 0, sizeof(ev));
       ev.data.fd = fd;
       ev.events = channel->events() | EPOLLET; //边沿触发
       ChannelMap[fd] = channel;

       int listenfd = ::epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev); //添加文件描述符fd到监听队列中
       if (listenfd < 0)
       {
              LOG_ERROR << "Epoll::epoll_add error" << errno;
              return;
       }
       //nChannel++;
}

//修改已经添加的事件  updateChannel
void Epoll::epoll_mod(Channel *channel)
{
       int fd = channel->fd();
       struct epoll_event ev;
       memset(&ev, 0, sizeof(ev));
       ev.data.fd = fd;
       ev.events = channel->events() | EPOLLET; //边沿触发
       ChannelMap[fd] = channel;

       int listenfd = ::epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev); //修改文件描述符
       if (listenfd < 0)
       {
              LOG_ERROR << "Epoll::epoll_mod error" << errno;
              return;
       }
}

//移除不再监听的事件  removeChannel
void Epoll::epoll_del(Channel *channel)
{
       int fd = channel->fd();
       int listenfd = ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, 0); //移除文件描述符
       if (listenfd < 0)
       {
              LOG_ERROR << "Epoll::epoll_del error" << errno;
       }
       //ChannelMap[fd] = NULL; //将fd对应的事件指针置空
       ChannelMap.erase(fd);
}

//阻塞等待事件的发生
void Epoll::epoll_wait(int timeout, ChannelList &ActiveChannels)
{
       int numEvents = ::epoll_wait(epollfd_, &*ActiveEvents.begin(), MAXEVENTS, timeout);
       if (numEvents < 0)
       {
              LOG_ERROR << "Epoll::epoll_wait error" << errno;
       }
       else if (numEvents == 0) //等待超时
       {
              LOG_WARN << "Epoll::epoll_wait nothing happen";
       }
       else
       {
              ActiveChannels.clear(); //清空channel数组
              for (int i = 0; i < numEvents; i++)
              {
                     int fd = ActiveEvents[i].data.fd;
                     auto temp_channel = ChannelMap[fd];
                     temp_channel->setRevents(ActiveEvents[i].events);
                     //temp_channel->setEvents(0);
                     ActiveChannels.push_back(temp_channel); //将活跃事件添加到channel数组中
              }
              if (numEvents == MAXEVENTS)
              {
                     MAXEVENTS *= 2;
                     ActiveEvents.resize(MAXEVENTS);
              }
              ActiveEvents.clear(); //清空活跃事件数组
       }
}