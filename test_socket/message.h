#ifndef MESSAGE_H
#define MESSAGE_H
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string>
#include<netdb.h>
#include<ifaddrs.h>

namespace Werewolf{
  class Socket{
  private:
    sockaddr_in _addr;
    int _socket;//windows下类型应该是SOCKET,-1表示不可用
    int _af;
    int _type;
    int _protocol;
    bool is_bind = false;
    char _buffer[100];//缓冲区
    void check_addr(std :: string);
    Socket(int socket, int af, int type, int protocol, std::string addr, int port);//私有构造函数，在accept中使用
  public:
    enum class Howto{RD = 0, WR = 1, BOTH = 2};
    Socket();//_socket为-1,不可用
    Socket(int af, int type, int protocol);//普通构造函数
    Socket(int af, int type, int protocol, std::string addr, int port);//构造时指定ip地址和端口号
    Socket(const Socket&) = delete;
    Socket& operator = (const Socket&) = delete;
    Socket(Socket &&);
    Socket& operator = (Socket &&);
    ~Socket();
    void assign_addr(std::string addr, int port);//如果构造时没给出，可用这个函数指定地址和端口
    int bind();//将套接字和指定的地址及端口绑定,成功返回1，失败返回-1
    int connect();//将套接字和指定的地址及端口连接,返回值同上
    int listen();//监听，返回值同上
    Socket accept(double delay) const;//接受连接，返回一个新的Socket类，内部存有客户端的地址信息,有延时功能，未成功accept则返回一个不可用的Socket
    int send(std::string msg, double delay = 0.0) const;//功能和返回值与原send函数相同
    int recv(char* buf, int len, double delay = 0.0) const;//同上
    std::string recv(double delay = 0.0) const;//将获得的信息返回为一个std::string类,不成功时返回空的string,shutdown时返回"__SHUTDOWN"
    int broadcast(std::string msg);//向局域网内广播消息，不要先bind
    int recv_broadcast(char *buf, int len, double delay = 0.0);
    std::string recv_broadcast(double delay = 0.0);//接受广播消息，不要先bind
    int recv_from(char *buf, int len,  std::string addr, int port, double delay = 0.0);//功能和返回值同原函数,有延时功能
    std::string recv_from(std::string addr, int port, double delay = 0.0);
    int send_to(std::string msg, std::string addr, int port);//功能和返回值同原函数
    void close();
    std::string get_host_addr() const;//返回第一个不是127.0.0.1的本机ip
    bool valid() const;//返回当前对象是否可用
    void shutdown(Howto);//同原函数
  };
}


#endif
