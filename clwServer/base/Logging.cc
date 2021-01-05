#include "Logging.h"
#include <sys/time.h>

using namespace my_muduo;

__thread time_t t_lastSecond;
__thread char t_time[32];

const char *LogLevelName[my_muduo::NUM_LOG] =
    {
        "INFO ",
        "WARN",
        "ERROR",
};

void defaultOutput(const char *msg, int len)
{
       size_t n = ::fwrite(msg, 1, len, stdout); //向指定的文件中写入数据,stdout是标准输出
       (void)n;
}

void defaultFlush()
{
       ::fflush(stdout); //强迫将缓冲区数据写入指定文件中
}

Logger::outputFunc Logger::output = defaultOutput;
Logger::flushFunc Logger::flush = defaultFlush;

Logger::Logger(LogLevel level, const char *file, int line) : impl_(level, file, line)
{
}

Logger::~Logger()
{
       impl_.stream_ << "\n";
       const FixedBuffer<kSmallBuffer> &buf(impl_.stream_.getBuffer());
       output(buf.data(), buf.length());
}

Logger::Impl::Impl(LogLevel level, const char *file, int line) : stream_(),
                                                                 level_(level),
                                                                 file_(file),
                                                                 line_(line)
{
       formatTime();
       stream_ << LogLevelName[level_] << file_ << ":" << line_ << " ";
}

void Logger::Impl::formatTime() //将当前时间转换为规定格式，输出到流中
{
       struct timeval tv;
       time_t time;
       ::gettimeofday(&tv, NULL); //gettimeofday获取1970.1.1零点零分到当前所偏移的精确时间
       time = tv.tv_sec;          //秒数
       if (t_lastSecond != time)
       {
              t_lastSecond = time;
              struct tm *p_time = ::localtime(&time); //将偏移时间转换为本地时间
              ::strftime(t_time, 26, "%Y-%m-%d %H:%M:%S", p_time);
       }
       stream_ << t_time;
}