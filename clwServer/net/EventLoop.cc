#include "EventLoop.h"
#include "Channel.h"
#include "../base/Logging.h"
#include "Epoll.h"
#include <unistd.h>
#include <sys/eventfd.h>

using namespace my_muduo;

int createEventfd()
{
       int efd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
       if (efd <= 0)
       {
              LOG_ERROR << "Failed in eventfd";
              ::abort();
       }
       return efd;
}

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      wakeupFd_(createEventfd()),
      isPendingFunctor_(false),
      Epoller_(new Epoll),
      threadId_(CurrentThread::getpid()),
      wakeupChannel_(new Channel(this, wakeupFd_))
{
       LOG_INFO << "EventLoop created  [" << this << "] in thread [" << threadId_ << "]";
       wakeupChannel_->setEvents(EPOLLIN);
       wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
       addChannel(wakeupChannel_.get());
}

EventLoop::~EventLoop()
{
       ::close(wakeupFd_);
}

//事件循环
void EventLoop::loop()
{
       looping_ = true;
       quit_ = false;
       LOG_INFO << "EventLoop in thread[" << CurrentThread::getpid() << "] start looping";

       while (!quit_)
       {
              ActiveChannels_.clear();
              Epoller_->epoll_wait(-1, ActiveChannels_); //获取活跃Channel列表
              for (ChannelList::iterator it = ActiveChannels_.begin(); it != ActiveChannels_.end(); it++)
              {
                     (*it)->handleEvent(); //调用事件处理函数
              }
              doPendingFunctors();
       }
       LOG_INFO << "EventLoop " << this << "stop looping";
       looping_ = false;
}

void EventLoop::quit()
{
       quit_ = true; //终止事件循环标志位，下一次while(!quit)时起效
       if (!isInLoopThread())
       {
              wakeup(); //唤醒wakeupFd_所在的IO线程
       }
}

//在其IO线程执行某个用户任务回调
void EventLoop::runInLoop(const Functor &cb)
{
       if (isInLoopThread())
       {
              cb();
       }
       else
       {
              queueInLoop(std::move(cb));
       }
}

//将cb放入队列，必要时唤醒IO线程
void EventLoop::queueInLoop(const Functor &cb)
{
       {
              MutexLockGuard lock(mutex_);
              pendingFunctors.push_back(std::move(cb));
       }

       if (!isInLoopThread() || isPendingFunctor_)
       {
              wakeup();
       }
}

//唤醒IO线程，对wakeupFd_写入数据
void EventLoop::wakeup()
{
       uint64_t one = 1;
       size_t n = ::write(wakeupFd_, &one, sizeof(one));
       if (n != sizeof(one))
       {
              LOG_ERROR << "EventLoop::wakeup write" << n << "bytes instead of" << sizeof(one);
       }
       LOG_INFO << "EventLoop::wakeup finished";
}

//唤醒IO线程，读取wakeupFd_的数据
void EventLoop::handleRead()
{
       uint64_t one = 1;
       size_t n = ::read(wakeupFd_, &one, sizeof(one));
       if (n != sizeof(one))
       {
              LOG_ERROR << "EventLoop::handleRead reads" << n << "bytes instead of" << sizeof(one);
       }
}

//依次调用队列中的回调函数Functor
void EventLoop::doPendingFunctors()
{
       std::vector<Functor> tempFunctors;
       isPendingFunctor_ = true;

       {
              MutexLockGuard lock(mutex_);
              tempFunctors.swap(pendingFunctors); //交换内存的原因,一是减小临界区的长度，二是避免死锁(Functor再次调用queueInLoop)
       }

       for (size_t i = 0; i < tempFunctors.size(); i++)
       {
              tempFunctors[i]();
       }
       isPendingFunctor_ = false;
}

//添加 updateChannel
void EventLoop::addChannel(Channel *channel)
{
       Epoller_->epoll_add(channel);
}

//修改 updateChannel
void EventLoop::modChannel(Channel *channel)
{
       Epoller_->epoll_mod(channel);
}

//删除  removeChannel
void EventLoop::removeChannel(Channel *channel)
{
       Epoller_->epoll_del(channel);
}
