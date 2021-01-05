#include <map>
#include <stdio.h>

namespace my_muduo
{
       enum Method //请求报文的方法
       {
              kInvalid,
              kGet,   //请求读取由URL标志的信息
              kPost,  //给服务器添加信息
              kHead,  //请求读取由URL标志的信息的首部
              kPut,   //在指明的URL下存储一个文档
              kDelete //删除URL所标志的资源
       };

       enum Version //HTTP协议的版本
       {
              HttpUnknown,
              Http10, //HTTP/1.0协议
              Http11  //HTTP/1.1协议
       };

       enum ResquestParseState //请求报文解析状态
       {
              Http_ResLine, //请求行
              Http_Headers, //等待解析headers
              Http_Body,    //等待解析请求body
              Http_Done     //解析完成
       };

       //HTTP请求报文
       class HttpRequest
       {
       public:
              HttpRequest() : method_(kInvalid),
                              version_(HttpUnknown),
                              ParseState_(Http_ResLine)
              {
              }

              void setVersion(std::string &str)
              {
                     if (str == "HTTP/1.1")
                            version_ = Http11;
                     else if (str == "HTTP/1.0")
                            version_ = Http10;
                     else
                            version_ = HttpUnknown;
              }

              void setMethod(std::string &str)
              {
                     if (str == "GET")
                            method_ = kGet;
                     else if (str == "POST")
                            method_ = kPost;
                     else if (str == "HEAD")
                            method_ = kHead;
                     else if (str == "PUT")
                            method_ = kPut;
                     else if (str == "DELETE")
                            method_ = kDelete;
                     else
                            method_ = kInvalid;
              }

              const std::string &path() const { return path_; }
              void setParseState(ResquestParseState ParseState) { ParseState_ = ParseState; }
              ResquestParseState getParseState() { return ParseState_; }

       private:
              Method method_;    //方法
              Version version_;  //版本
              std::string path_; //URL
              ResquestParseState ParseState_;
              std::map<std::string, std::string> RequestHeaders_;
       };

       enum HttpStatusCode //状态码
       {
              kUnknown,             //不知道
              k200Ok = 200,         //表示成功
              k400BadRequest = 400, //错误的请求
              k404NotFound = 404    //找不到
       };

       //HTTP响应报文
       class HttpResponse
       {
       public:
              HttpResponse() : StatusCode_(kUnknown)
              {
              }

              void setStatusCode(HttpStatusCode code) { StatusCode_ = code; } //设置状态码
              void setStatusMessage(const std::string &message) { statusMessage_ = message; }
              void setContentType(const std::string &contentType) { addHeader("Content-Type", contentType); }
              void addHeader(const std::string &key, const std::string &value) { ResponseHeaders_[key] = value; }
              void setBody(const std::string body) { body_ = body; }

       private:
              HttpStatusCode StatusCode_;
              std::string statusMessage_;
              std::string body_;
              std::map<std::string, std::string> ResponseHeaders_;
       };

} // namespace my_muduo