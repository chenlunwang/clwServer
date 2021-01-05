#pragma once
#include "LogStream.h"
#include <functional>
#include <memory>

namespace my_muduo
{
       enum LogLevel
       {
              INFO,   //提示信息
              WARN,   //警告
              ERROR,  //错误
              NUM_LOG //枚举值为3,代表前面有三种日志等级
       };

       //简单的日志，日志的内容，搭配AsyncLogging使用
       class Logger
       {
       public:
              explicit Logger(LogLevel level, const char *file, int line);
              ~Logger();
              LogStream &stream() { return impl_.stream_; }; //获取日志流

              typedef std::function<void(const char *, int)> outputFunc;
              typedef std::function<void()> flushFunc;
              static void setOutputFunc(const outputFunc &output_) { output = output_; }
              static void setFlushFunc(const flushFunc &flush_) { flush = flush_; }

       private:
              struct Impl
              {
                     Impl(LogLevel level, const char *file, int line);
                     void formatTime(); //获取日志消息的时间
                     LogStream stream_; //日志流
                     LogLevel level_;   //日志等级
                     std::string file_; //日志产生的文件
                     int line_;         //日志产生的行数
              };
              struct Impl impl_;

              static outputFunc output;
              static flushFunc flush;
       };

#define LOG_INFO my_muduo::Logger(my_muduo::LogLevel::INFO, __FILE__, __LINE__).stream()
#define LOG_WARN my_muduo::Logger(my_muduo::LogLevel::WARN, __FILE__, __LINE__).stream()
#define LOG_ERROR my_muduo::Logger(my_muduo::LogLevel::ERROR, __FILE__, __LINE__).stream()
} // namespace my_muduo