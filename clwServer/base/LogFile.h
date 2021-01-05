#pragma once
#include <string>
#include <memory>
#include "Mutex.h"
#include "noncopyable.h"
#include <iostream>

//File类对磁盘上的日志文件进行操作
namespace my_muduo
{
       class File
       {
       public:
              File(const std::string &filename) : fp(::fopen(filename.c_str(), "ae"))
              {
                     if (fp == NULL)
                     {
                            std::cout << "fopen fail\n";
                            return;
                     }
                     ::setbuffer(fp, buffer, sizeof(buffer)); //设置fp缓冲区大小
              }

              ~File()
              {
                     ::fclose(fp);
              }

              void append(const char *content, size_t length)
              {
                     size_t n = unlockWrite(content, length);
                     size_t remain = length - n;
                     while (remain > 0)
                     {
                            size_t x = unlockWrite(content + n, remain);
                            if (x == 0)
                            {
                                   int err = ferror(fp);
                                   if (err)
                                          fprintf(stderr, "AppendFile::append() failed!\n");
                                   break;
                            }
                            n += x;
                            remain = length - n;
                     }
              }

              void flush() { ::fflush(fp); } //将缓冲区的数据写回fp指向的文件中

       private:
              FILE *fp;
              char buffer[64 * 1024];

              size_t unlockWrite(const char *content, size_t length)
              {
                     return ::fwrite_unlocked(content, 1, length, fp); //fwrite_unlocked是fwrite的不安全啊版本
              }
       };

       //LogFile是对File的进一步封装，主要实现以下功能：
       //创建统一格式的日志
       //如果隔一段时间，会自动刷新用户缓冲区
       //如果日志创建时间和当前时间不在一天时，会回滚日志
       class LogFile : noncopyable
       {
       public:
              explicit LogFile(const std::string &basename, int flushInterval = 3);
              ~LogFile(){};

              void append(const char *logline, int len);
              void flush();
              void rollFile(); //回滚日志，即每天新建一个日志文件

       private:
              const std::string basename_; //日志的基本名字
              const int flushInterval_;    //日志刷新时间
              time_t startTime_;           //日志开始创建的时间
              time_t lastFlush_;           //上一次刷新的时间

              std::unique_ptr<File> file_;
              std::unique_ptr<MutexLock> mutex_;

              std::string setLogName();
              void append_unlocked(const char *content, size_t length);
       };
} // namespace my_muduo