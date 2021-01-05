#pragma once
#include <iostream>
#include "LogFile.h"
#include "LogStream.h"
#include "Thread.h"
#include "CountDownLatch.h"
#include "condition.h"
#include <vector>
#include "Mutex.h"
#include "Logging.h"
#include "noncopyable.h"

namespace my_muduo
{
       //异步日志类
       class AsyncLogging : noncopyable
       {
       public:
              AsyncLogging(const std::string &logBasename, int flushInteval = 3); //日志库每隔3秒执行一次两个缓冲区的交换写入操作，保证及时将日志消息写入磁盘文件
              ~AsyncLogging();

              void start();
              void stop();
              void append(const char *logline, int len);

       private:
              typedef my_muduo::FixedBuffer<kLargeBuffer> Buffer;
              typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
              typedef std::unique_ptr<Buffer> BufferPtr;

              const int flushInteval_;     //刷新间隔，每次刷新将双缓冲中的数据写入File类中的fwrite中的缓冲区
              bool running_;               //循环判断的标志位
              const std::string basename_; //日志的基本名称

              my_muduo::Thread thread_;
              my_muduo::CountDownLatch latch_;
              my_muduo::MutexLock mutex_;
              my_muduo::Condition cond_;

              //第一块缓冲区
              BufferPtr currentBuffer_; //当前缓冲区
              BufferPtr nextBuffer_;    //预备缓冲区
              BufferVector buffers_;    //待写入文件的已18填满的缓冲

              void threadFunc();
       };
} // namespace my_muduo
