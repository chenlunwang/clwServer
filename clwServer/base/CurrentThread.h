#pragma once
#include "Thread.h"

namespace my_muduo
{
       namespace CurrentThread
       {
              extern __thread pid_t ThreadPid;
              pid_t getpid();
       } // namespace CurrentThread
} // namespace my_muduo
