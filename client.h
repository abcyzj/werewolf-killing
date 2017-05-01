#ifndef CLIENT_H
#define CLIENT_H

#include"message.h"
#include<thread>

namespace Werewolf{
  class Client{
  private:
    bool _available;//指示当前Client是否可用
    Socket _sock;//和该客户端通信的Socket
  public:
    Client();//默认构造函数
    Client(Client&) = delete;//不允许拷贝构造
    Client& operator = (const Client&) = delete;//不允许拷贝赋值
    Client(Client&&);//移动构造函数
    Client& operator = (Client&&);//移动赋值
    Client(Socket&&);//给出Socket的构造函数
    void set_sock(Socket&&);//Socket也可不在构造时给出
    bool available();//返回当前Client是否可用
  };
}

#endif
