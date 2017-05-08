#ifndef FOR_CLIENT_H
#define FOR_CLIENT_H

#include"message.h"
#include<string>
#include<chrono>
#include<thread>
#include<vector>
#include<mutex>

namespace Werewolf{
  class Connect;
  class Messenger;
  class InputManager;
  class ClientExecutor;

  class Connect{//给出端口，该类可以接受广播，打印已找到的服务器信息，并连接。也可通过ip地址连接
  private:
    std::thread _recv_th;//接受广播的线程
    std::thread _show_th;//show线程
    std::vector<std::string> _info;//存储当前探测到的服务器信息,格式暂定为"ROOMNAME:***;IP:***;"
    std::mutex _info_mtx;//_info的互斥量
    int _por;//存储端口
    void do_recv();//线程函数
    bool _recv_over;
    void do_show(std::chrono::milliseconds);//线程函数
    bool _show_over;
    Socket _recv_sock;//接受广播的Socket
    std::string get_IP(std::string);//从信息中提取出IP地址来
  public:
    Connect();//默认构造函数
    Connect(int port);//指定端口的构造函数
    ~Connect();
    void set_port(int port);//设置端口
    Socket connect(int k);//连接第k个客户端，返回一个Socket
    Socket connect_via_IP(const std::string &IP);//通过IP直接连接
    void recv();//开始recv
    void end_recv();//结束recv线程
    void show(std::chrono::milliseconds);//指定show的时间间隔并开始show
    void end_show();//结束show线程
  };

  class InputManager{//管理用户输入
  private:
    Messenger *_parent_messenger;
    std::thread _th;//读取输入的线程
    bool _hold_on = 0;
    bool _th_finished = true;;
    void do_input();//线程函数
  public:
    InputManager(Messenger *parent);//告知父亲指针
    ~InputManager();
    void turn_on();//打开输入
    void turn_off();//关闭输入
    void hold_on();//打开并保持输入
    void end_thread();//关闭进程
  };

  class ClientExecutor{//命令执行器
  private:
    Messenger *_parent_messenger;//存储父亲指针
    std::string _order;//命令队列
    void execute(std::string);
    InputManager inputmanager;
  public:
    ClientExecutor(Messenger *parent);//给出父亲指针
    ~ClientExecutor();
    void add_order(std::string);//向命令队列中添加信息
  };

  class Messenger{//管理和服务器之间的通信
  private:
    Socket _sock;//通信所用Socket
    //std::thread _th;//通信线程(主线程即可)
    bool _thread_over;//线程结束标记
    std::string _info;//存储暂未发送的信息
    std::mutex _info_mtx;//_info的互斥量
    void do_commu();//线程函数
    ClientExecutor executor;
  public:
    Messenger(Socket&&);//构造函数，给定通信的Socket
    ~Messenger();
    void start_thread();//开始通信进程
    void end_thread();//结束通信进程
    void send(std::string);//向服务器发信息
  };
}
#endif









