#include "LogFile.h"
//#include "Mutex.h"
#include <bits/types.h>

#define kCheckTimeRoll 1024
#define kRollPerSeconds 60 * 60 * 24 //一天多少秒

using namespace my_muduo;

LogFile::LogFile(const std::string &basename, int flushInterval) : basename_(basename),
                                                                   flushInterval_(flushInterval),
                                                                   startTime_(0),
                                                                   lastFlush_(0),
                                                                   mutex_(new MutexLock())
{
       rollFile();
}

void LogFile::append(const char *logline, int len)
{
       MutexLockGuard lock(*mutex_);
       append_unlocked(logline, len);
}

void LogFile::append_unlocked(const char *content, size_t length)
{
       file_->append(content, length); //将数据添加到缓存中

       //判断是否需要刷新或回滚日志
       time_t now = ::time(NULL);
       time_t nowDay = now / kRollPerSeconds * kRollPerSeconds;
       if (nowDay != startTime_)
              rollFile();
       else if ((now - lastFlush_) > flushInterval_)
       {
              lastFlush_ = now;
              file_->flush();
       }
}

//刷新日志文件
void LogFile::flush()
{
       MutexLockGuard lock(*mutex_);
       file_->flush();
}

//回滚日志文件，即：
//如果已经打开一个日志文件，那么就关闭这个文件，并重新创建一个文件
//如果没有打开日志文件，就直接创建一个新的日志文件
void LogFile::rollFile()
{
       std::string logName = setLogName();
       file_.reset(new File(logName));
       startTime_ = startTime_ / kRollPerSeconds * kRollPerSeconds; //将时间调整到当天零点
}

//设置日志文件的名字，日志名称格式为：basename_+年月日-小时分秒+.log
std::string LogFile::setLogName()
{
       std::string filename(basename_); //进程名字

       char timebuf[32];
       struct tm tm_; //时间结构体
       startTime_ = time(NULL);
       gmtime_r(&startTime_, &tm_);
       strftime(timebuf, sizeof timebuf, "%Y%m%d-%H%M%S_", &tm_); //格式化输出字符串

       filename += timebuf; //日志文件创建时间
       filename += ".log";  //日志后缀名
       return filename;
}