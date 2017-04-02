#ifndef MESSAGE_H
#define MESSAGE_H
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

namespace Werewolf{
  class Socket{
  private:
    sockaddr_in _addr;
    int _socket;//windows下类型应该是SOCKET
    char _buffer[100];//缓冲区
  public:
    Socket(int af, int type, int protocol);//普通构造函数
    Socket(int af, int type, int protocol, string addr, int port);//构造时指定ip地址和端口号
    ~Socket();
    void assign_addr(string addr, int port);//如果构造时没给出，可用这个函数指定地址和端口
    int bind();//将套接字和指定的地址及端口绑定,成功返回1，失败返回-1
    int connect();//将套接字和指定的地址及端口连接,返回值同上
    int listen();//监听，返回值同上
    Socket accept();//接受连接，返回一个新的Socket类，内部存有客户端的地址信息
    int send(string msg);//功能和返回值与原send函数相同
    int recv(char* buf, int delay);//同上
    string recv(int delay);//将获得的信息返回为一个string类
    int broadcast(string msg);//向局域网内广播消息
    int recv_broadcast(char * buf);
    string recv_broadcast();//接受广播消息
    int recv_from(string addr, int port);//功能和返回值同原函数
    int send_to(string addr, int port);//功能和返回值同原函数
    void close();
  }
}


#endif
