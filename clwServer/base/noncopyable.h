#pragma once

namespace my_muduo
{
       class noncopyable
       {
       protected:
              noncopyable() = default;
              ~noncopyable() = default;

       private:
              noncopyable(const noncopyable &) = delete;                 //不能调用拷贝构造函数
              const noncopyable operator=(const noncopyable &) = delete; //不能调用赋值重载函数
       };
} // namespace my_muduo
