#ifndef BROAD_H
#define BROAD_H
#include"message.h"
#include<thread>
#include<iostream>
#include<string>

namespace Werewolf{
  class Broadcast{
  private:
    Socket _sock;
    std::string _msg;
    std::thread _th;//执行广播的线程
    bool _over;//广播结束的标记
    void do_broadcast();
  public:
    Broadcast(int port);//构造时指定端口号
    ~Broadcast();
    void start(const std::string &msg);//指定消息，新增一个子线程开始广播
    void end();//停止广播
  };
}
#endif
