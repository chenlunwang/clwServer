#pragma once
#include <functional>
#include <pthread.h>
#include "noncopyable.h"
//#include "CountDownLatch.h"

namespace my_muduo
{
	class Thread : noncopyable
	{
	public:
		typedef std::function<void()> ThreadFuncCallback; //线程回调函数

		//显式构造函数，不能隐式转换
		explicit Thread(const ThreadFuncCallback &func);
		~Thread();

		int start(); //启动线程
		int join();  //阻塞等待指定的线程终止
		void runInThread();

		//获取私有成员变量
		bool started() const { return started_; }
		pid_t tid() const { return tid_; }

	private:
		ThreadFuncCallback ThreadFuncCallback_; //线程中具体执行的函数

		bool started_;	 //线程是否启动
		bool joined_;		 //线程是否终止
		pthread_t pthreadId_; //线程ID
		pid_t tid_;		 //线程真实pid,系统内唯一

		static void *startThread(void *thread);
	};
} // namespace my_muduo
