#ifndef ACCEPT_H
#define ACCEPT_H

#include"message.h"
#include"client.h"
#include<thread>
#include<vector>
#include<chrono>

namespace Werewolf{
  class Accept{//该类被设计用于管理一个负责accept的线程，并把连接生成的Client类放入一个vector中
  private:
    std::thread _th;//所管理的线程
    std::thread _show_th;//show对应的线程
    Socket _sock;//用于接受的socket
    std::vector<Client> *_vec;//所管理的vector
    bool _over;//指示应当结束线程
    bool _show_over;//同上
    void do_accept();//线程函数
    void do_show(std::chrono::milliseconds);//同上
  public:
    Accept();//默认构造函数
    Accept(int port);//给出端口
    Accept(int port, std::vector<Client>*);//给出一个想让Accept管理的vector的指针
    void set_vector(std::vector<Client>*);//也可以不在构造时给出该指针
    void show(std::chrono::milliseconds);//多一个show线程，指定show的时间间隔
    void end_show();//结束show线程
    void start();//开始
    void end();//结束
  };
}
#endif
