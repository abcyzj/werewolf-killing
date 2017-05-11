#ifndef CLIENT_H
#define CLIENT_H

#ifdef WIN32
#include"message_win.h"
#else
#include"message.h"
#endif

#include<thread>
#include<string>
#include<mutex>
#include"character.h"
#include<queue>

namespace Werewolf{
  class Client{//使用之前应lock，之后unlock
  private:
    bool _available;//指示当前Client是否可用
    std::string _nickname = "UNKOWN";//客户端的昵称
    Socket _sock;//和该客户端通信的Socket
    //std::thread _th;//监听线程
    std::mutex mtx;//该Client类的互斥量
    //bool _commu_over;//线程结束标记
    //void do_commu();//通信线程函数
    //std::string _order;//希望发送的命令
    //std::mutex _order_mtx;//_order的互斥量
    Character* _charac;//该客户端对应的角色类
  public:
    Client();//默认构造函数
    Client(Client&) = delete;//不允许拷贝构造
    Client& operator = (const Client&) = delete;//不允许拷贝赋值
    Client(Client&&);//移动构造函数
    Client& operator = (Client&&);//移动赋值
    Client(Socket&&);//给出Socket的构造函数
    ~Client();
    void set_sock(Socket&&);//Socket也可不在构造时给出
    bool available();//返回当前Client是否可用
    //void lock();//获取使用权限
    //bool try_lock();//非阻塞的lock，成功返回true
    //void unlock();//解锁
    //void start_thread();//开始通信进程
    //void end_thread();//关闭通信进程(放在private里是否更好？)
    //void send(const std::string&, double = 0.0);
    void print(std::string);//命令客户端打印信息
    void turn_on_input();//命令客户端打开输入读取,只读取一次
    void hold_on_input();//命令客户端打开输入读取，直到调用turn_off_input为止
    void turn_off_input();//命令客户端关闭输入读取
    void shut_down();//命令客户端断开连接
    std::string recv(double delay = 0);//从客户端接受信息
    void set_charac(Character*);//绑定角色类
    Character* selfCharacter();
    void changename(std::string);
    std::string getnickname();
  };
}

#endif
