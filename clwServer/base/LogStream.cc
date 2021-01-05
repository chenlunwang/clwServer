#include "LogStream.h"
#include <algorithm>

using namespace my_muduo;

const char digits[] = "9876543210123456789";
const char *zero = digits + 9;

template <typename T>
size_t convert(char buf[], T value) //把数字value转换为字符串，存放到buf中
{
       T i = value;
       char *p = buf;

       do
       {
              int lsd = static_cast<int>(i % 10);
              i /= 10;
              *p++ = zero[lsd];
       } while (i != 0);

       if (value < 0)
              *p++ = '-';
       *p = '\0';
       std::reverse(buf, p); //颠倒buf和p的顺序
       return p - buf;
}

template <typename T>
void LogStream::formatInteger(T v)
{
       //buffer_的空闲空间大于kMaxNumericSize
       if (buffer_.avail() >= kMaxNumericSize)
       {
              size_t len = convert(buffer_.current(), v);
              buffer_.add(len); //缓冲区空闲空间的首地址增加len
       }
}

LogStream &LogStream::operator<<(bool v)
{
       buffer_.append(v ? "1" : "0", 1);
       return *this;
}

LogStream &LogStream::operator<<(short v)
{
       *this << static_cast<int>(v);
       return *this;
}

LogStream &LogStream::operator<<(unsigned short v)
{
       *this << static_cast<unsigned int>(v);
       return *this;
}

LogStream &LogStream::operator<<(int v)
{
       formatInteger(v);
       return *this;
}

LogStream &LogStream::operator<<(unsigned int v)
{
       formatInteger(v);
       return *this;
}

LogStream &LogStream::operator<<(long v)
{
       formatInteger(v);
       return *this;
}

LogStream &LogStream::operator<<(unsigned long v)
{
       formatInteger(v);
       return *this;
}

LogStream &LogStream::operator<<(long long v)
{
       formatInteger(v);
       return *this;
}

LogStream &LogStream::operator<<(unsigned long long v)
{
       formatInteger(v);
       return *this;
}

LogStream &LogStream::operator<<(float v)
{
       *this << static_cast<double>(v);
       return *this;
}

LogStream &LogStream::operator<<(double v)
{
       if (buffer_.avail() >= kMaxNumericSize)
       {
              int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v); //%.12g表示数据长度最多为12位
              buffer_.add(len);
       }
       return *this;
}

LogStream &LogStream::operator<<(long double v)
{
       if (buffer_.avail() >= kMaxNumericSize)
       {
              int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v); //%.12g表示数据长度最多为12位
              buffer_.add(len);
       }
       return *this;
}

LogStream &LogStream::operator<<(char v)
{
       buffer_.append(&v, 1);
       return *this;
}

LogStream &LogStream::operator<<(const char *str)
{
       if (str)
              buffer_.append(str, strlen(str));
       else
              buffer_.append("(null)", 6);
       return *this;
}

LogStream &LogStream::operator<<(const unsigned char *str)
{
       return operator<<(reinterpret_cast<const char *>(str));
}

LogStream &LogStream::operator<<(const std::string &v)
{
       buffer_.append(v.c_str(), v.size());
       return *this;
}
