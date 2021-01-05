#pragma once
#include <string>
#include <string.h>
#include "noncopyable.h"

namespace my_muduo
{
       const int kSmallBuffer = 4000;
       const int kLargeBuffer = 4000 * 1000;

       template <int SIZE> //指定缓冲区大小
       //FixedBuffer自定义缓冲区类
       class FixedBuffer : noncopyable
       {
       public:
              FixedBuffer() : cur(buf) {}
              ~FixedBuffer() {}

              //追加缓冲区内容
              void append(const char *buf_, size_t len)
              {
                     if (avail() > static_cast<int>(len)) //implicit_cast隐式转换类型
                     {
                            memcpy(cur, buf_, len); //把buf_指向的内存区域中的len长度的字节拷贝到cur指向的内存区域中
                            cur += len;             //缓冲区空闲区域首地址更新
                     }
              }

              const char *data() const { return buf; }                   //获取缓冲区数据,返回缓冲区首地址
              int length() const { return static_cast<int>(cur - buf); } //当前缓冲区的数据长度，空闲区域首地址减去缓冲区首地址

              char *current() { return cur; }                                         //缓冲区当前空闲区域的首地址
              int avail() const { return static_cast<int>(buf + sizeof(buf) - cur); } //缓冲区空闲区域的容量
              void add(size_t len) { cur += len; }                                    //将缓冲区中len字节更新为已存储空间

              //清空缓冲区
              void reset() { cur = buf; }
              void bzero() { memset(buf, 0, sizeof buf); }

       private:
              char buf[SIZE];
              char *cur; //指向缓冲区空闲内存的首地址
       };

       //LogStream自定义流，将数据以字符串格式写到流，不使用自带的iostream,从而提高效率
       class LogStream : noncopyable
       {
              typedef LogStream self;

       public:
              typedef FixedBuffer<kSmallBuffer> Buffer;

              self &operator<<(bool v);
              self &operator<<(short);
              self &operator<<(unsigned short);
              self &operator<<(int);
              self &operator<<(unsigned int);
              self &operator<<(long);
              self &operator<<(unsigned long);
              self &operator<<(long long);
              self &operator<<(unsigned long long);
              self &operator<<(float v);
              self &operator<<(double);
              self &operator<<(long double);
              self &operator<<(char v);
              self &operator<<(const char *str);
              self &operator<<(const unsigned char *str);
              self &operator<<(const std::string &v);

              void append(const char *data, int len) { buffer_.append(data, len); }
              const Buffer &getBuffer() const { return buffer_; }
              void resetBuffer() { buffer_.reset(); }

       private:
              Buffer buffer_;
              static const int kMaxNumericSize = 32; //数据的最大位数，也是缓冲区为一个数据预留的最小空间

              template <typename T>
              void formatInteger(T); //将T类型数字转换为字符串
       };
} // namespace my_muduo