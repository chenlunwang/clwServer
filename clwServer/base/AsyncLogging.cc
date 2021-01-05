#include "AsyncLogging.h"
#include "LogFile.h"
using namespace my_muduo;

AsyncLogging::AsyncLogging(const std::string &basename, int flushInteval) : flushInteval_(flushInteval),
                                                                            running_(false),
                                                                            basename_(basename),
                                                                            thread_(std::bind(&AsyncLogging::threadFunc, this)),
                                                                            latch_(1),
                                                                            mutex_(),
                                                                            cond_(mutex_),
                                                                            currentBuffer_(new Buffer),
                                                                            nextBuffer_(new Buffer),
                                                                            buffers_()
{
       currentBuffer_->bzero();
       nextBuffer_->bzero();
       buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging()
{
       running_ = false;
       cond_.signal();
       thread_.join();
}

//启动异步日志
void AsyncLogging::start()
{
       running_ = true;
       thread_.start(); //创建异步日志线程
       latch_.wait();   //开启后端线程，并在主线程阻塞条件等待，等到后端线程进入线程处理函数以后解除阻塞,
                        //将下一个缓冲区数据写入磁盘文件中。
}

//前端实现，往当前缓冲区中添加日志消息
void AsyncLogging::append(const char *logline, int len)
{
       MutexLockGuard lock(mutex_);

       if (currentBuffer_->avail() > len)
              currentBuffer_->append(logline, len);
       else
       {
              buffers_.push_back(std::move(currentBuffer_)); //当前缓冲区已满，将其移入buffers_
              if (nextBuffer_)
                     currentBuffer_ = std::move(nextBuffer_); //如果有备用缓存，就取下拿出来
              else
                     currentBuffer_.reset(new Buffer); //如果没有，就创建一个新的Buffer
              currentBuffer_->append(logline, len);    //向currentBuffer_中写入日志消息
              cond_.signal();                          //前端写满了至少一个buffer,唤醒后端线程，准备向磁盘文件写入数据
       }
}

//后端实现，将日志消息写入磁盘文件
void AsyncLogging::threadFunc()
{
       latch_.countDown(); //将计数器减1

       LogFile output(basename_); //LogFile对象,每一个日志线程所写的文件不同
       //双缓冲技术。第二块缓冲区，替换第一块缓冲区的内容，并写入日志文件。
       BufferPtr newBuffer1(new Buffer);
       BufferPtr newBuffer2(new Buffer);
       BufferVector buffersToWrite; //

       newBuffer1->bzero();
       newBuffer2->bzero();
       buffersToWrite.reserve(16);

       while (running_)
       {
              //加锁，为了完成等待日志消息写入缓冲，并将缓冲从成员变量中置换出来
              {
                     MutexLockGuard lock(mutex_);
                     if (buffers_.empty())
                            cond_.timedwait(flushInteval_); //线程阻塞时间间隔为3秒

                     buffers_.push_back(std::move(currentBuffer_));
                     currentBuffer_ = std::move(newBuffer1);
                     buffersToWrite.swap(buffers_); //buffers_中的内容全部移入bufferToWrite中，buffers_变为空，解锁后其它线程可继续向buffers_中添加日志

                     if (!nextBuffer_) //currentBuffer_使用完了之后就会用nextBuffer_
                     {
                            nextBuffer_ = std::move(newBuffer2);
                     }
              }

              //如果日志内容过多，丢弃多余的日志buffer,防止日志库引起的程序故障，是自我保护
              if (buffersToWrite.size() > 25)
              {
                     char buf[256];
                     snprintf(buf, sizeof buf, "Droped Log messages at  %zd large buffers\n", buffersToWrite.size() - 2); //将参数格式化成字符串，并复制到buf中
                     //char buf[] = "log message is too much,droped some of them!\n";
                     fputs(buf, stderr);
                     output.append(buf, static_cast<int>(strlen(buf)));
                     buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
              }

              //将buffersToWrite写入磁盘文件中
              for (const auto &buffer : buffersToWrite)
              {
                     output.append(buffer->data(), buffer->length());
              }
              if (buffersToWrite.size() > 2)
              {
                     buffersToWrite.resize(2);
              }

              //重新填充newBuffer1和newBuffer2，用于下一次替换前端的当前缓冲和预备缓冲
              if (!newBuffer1)
              {
                     newBuffer1 = std::move(buffersToWrite.back()); //back()最后一个元素的引用
                     buffersToWrite.pop_back();                     //删除容器中的最后一个元素
                     newBuffer1->reset();
              }
              if (!newBuffer2)
              {
                     newBuffer2 = std::move(buffersToWrite.back());
                     buffersToWrite.pop_back();
                     newBuffer2->reset();
              }
              buffersToWrite.clear();
              output.flush(); //将缓冲区的数据写回磁盘文件中
       }
       output.flush();
}