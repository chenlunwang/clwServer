#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>

#include "CurrentThread.h"
#include "Logging.h"

namespace my_muduo
{
	namespace CurrentThread
	{
		//__thread关键字修饰的全局变量
		__thread pid_t ThreadPid = 0;
	} // namespace CurrentThread

	pid_t CurrentThread::getpid()
	{
		if (ThreadPid == 0)
		{
			ThreadPid = ::syscall(SYS_gettid); //获取当前线程的真实tid,系统内唯一
		}
		return ThreadPid;
	}

	Thread::Thread(const ThreadFuncCallback &func)
	    : pthreadId_(0),
	      tid_(0),
	      started_(false),
	      joined_(false)
	{
		ThreadFuncCallback_ = std::move(func);
	}

	Thread::~Thread()
	{
		if (started_ && !joined_)
			pthread_detach(pthreadId_); //把指定线程转变为脱离状态
	}

	//子线程执行内容
	void *Thread::startThread(void *thread)
	{
		Thread *tempThread = static_cast<Thread *>(thread); //static_cast将隐式转换变为显式转换
		tempThread->runInThread();
		return NULL;
	}

	//启动线程
	int Thread::start()
	{
		started_ = true;
		int ret = ::pthread_create(&pthreadId_, NULL, startThread, this); //this表示子线程执行函数输入参数，这里是类Thread；pthread_create会返回线程ID
		if (ret)								   //子线程创建成功时返回0,失败时返回一个大于0的数
		{
			started_ = false;
			//perror("pthread_create error\n");
			LOG_ERROR << "Thread::pthread_create failed:";
		}
		return ret;
	}

	//阻塞等待指定的线程终止
	int Thread::join()
	{
		joined_ = true;
		int ret = ::pthread_join(pthreadId_, NULL);
		if (ret)
		{
			//perror("pthread_join error\n");
			LOG_ERROR << "Thread::pthread_join failed";
		}
		return ret;
	}

	//线程执行内容
	void Thread::runInThread()
	{
		tid_ = CurrentThread::getpid(); //获取当前线程pid,系统内唯一
		if (ThreadFuncCallback_)
			ThreadFuncCallback_(); //调用回调函数
	}
} // namespace my_muduo