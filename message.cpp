#include"message.h"
#include<iostream>
#include <stdexcept>//报错，抛出runtim_error
#include<cmath>

using namespace Werewolf;

void Socket :: check_addr(std::string s)
{
	sockaddr_in tep;
	::bzero(&tep, sizeof(tep));
	if(memcmp(&tep, &_addr, sizeof(tep)) == 0)
	{
		throw std::runtime_error(s.c_str());
	}
}
Socket::Socket(int af, int type, int protocol): _af(af), _type(type), _protocol(protocol){
  _socket = ::socket(af, type, protocol);
  ::bzero(&_addr, sizeof(_addr));
}

Socket::Socket(int af, int type, int protocol, std::string addr, int port): _af(af), _type(type), _protocol(protocol){
  _socket = ::socket(af, type, protocol);
  ::bzero(&_addr, sizeof(_addr));
  _addr.sin_family = af;
  _addr.sin_addr.s_addr = inet_addr(addr.c_str());
  _addr.sin_port = htons(port);
}

Socket::Socket(Socket &&s): _af(s._af), _type(s._type), _protocol(s._protocol){
  _socket = s._socket;
  s._socket = -1;//-1代表该socket无须被析构
  memcpy(&_addr, &s._addr, sizeof(s._addr));
}

Socket& Socket::operator = (Socket &&s){
  _socket = s._socket;
  _af = s._af;
  _type = s._type;
  _protocol = s._protocol;
  s._socket = -1;
  memcpy(&_addr, &s._addr, sizeof(s._addr));
  return *this;
}

Socket::~Socket(){
  if(_socket != -1)
    ::close(_socket);
}

void Socket::assign_addr(std::string addr, int port){
  _addr.sin_family = _af;
  _addr.sin_addr.s_addr = inet_addr(addr.c_str());
  _addr.sin_port = htons(port);
}

int Socket :: bind()
{
	check_addr("no address when bind");
	int res = ::bind(_socket, (sockaddr*) &_addr, sizeof(_addr));
  if(res >= 0)
    is_bind = true;

  return res;
}

int Socket :: connect()
{
	check_addr("no address when connect");
	return ::connect(_socket, (sockaddr*) &_addr, sizeof(_addr));
}

int Socket :: listen()
{
	check_addr("no address when listen");
	return ::listen(_socket, 20);
}

Socket Socket :: accept()
{
  sockaddr_in new_addr;
  socklen_t len = sizeof(new_addr);
	int new_sock = ::accept(_socket, (sockaddr*) &new_addr, &len);
  if(new_sock < 0)
    throw std::runtime_error("accept wrong");//
  return Socket(_af, _type, _protocol, ::inet_ntoa(new_addr.sin_addr), new_addr.sin_port);
}

int Socket :: send(std::string msg, double delay)
{
  if(delay == 0.0)
    return :: send(_socket, msg.c_str(), msg.length() + 1, 0);

  fd_set fds;
  int sec = std::floor(delay);
  int usec = (delay - std::floor(delay)) * 1e6;
  timeval timeout = {sec, usec};
  FD_ZERO(&fds);
  FD_SET(_socket, &fds);
  switch(::select(_socket + 1, NULL, &fds, NULL, &timeout)){
  case -1:
    throw std::runtime_error("select wrong");
    break;
  case 0:
    return -1;//-1表示发送超时
  default:
    if(FD_ISSET(_socket, &fds))
      return ::send(_socket, msg.c_str(), msg.length() + 1, 0);
    else
      return -1;
  }
}

int Socket :: recv(char * buf, int len, double delay)
{
  if(delay == 0.0)
    return ::recv(_socket, buf, len, 0);

  fd_set fds;
  int sec = std::floor(delay);
  int usec = (delay - std::floor(delay)) * 1e6;
  timeval timeout = {sec, usec};
  FD_ZERO(&fds);
  FD_SET(_socket, &fds);
  switch(::select(_socket + 1, &fds, NULL, NULL, &timeout)){
  case -1:
    throw std::runtime_error("select wrong");
    break;
  case 0:
    return -1;//-1表示发送超时
  default:
    if(FD_ISSET(_socket, &fds))
      return ::send(_socket, buf, len, 0);
    else
      return -1;
  }
}

std::string Socket::recv(double delay){
  char buf[200] = {0};
  if(delay == 0.0){
    int res = ::recv(_socket, buf, 199, 0);
    if(res < 0)
      return std::string();
    else
      return std::string(buf);
  }

  fd_set fds;
  int sec = std::floor(delay);
  int usec = (delay - std::floor(delay)) * 1e6;
  timeval timeout = {sec, usec};
  FD_ZERO(&fds);
  FD_SET(_socket, &fds);
  switch(::select(_socket + 1, &fds, NULL, NULL, &timeout)){
  case -1:
    throw std::runtime_error("select wrong");
    break;
  case 0:
    return std::string();//表示发送超时
  default:
    if(FD_ISSET(_socket, &fds)){//可以接收时
      int res = ::recv(_socket, buf, 199, 0);
      if(res < 0)
        return std::string();
      else
        return std::string(buf);
    }
    else
      return std::string();
  }
}

int Socket::broadcast(std::string msg){
  static bool is_first = true;
  if(is_first){//仅在第一次调用时检查
    if(_type != SOCK_DGRAM)
      throw std::runtime_error("type not consistent when broadcast");//检查是否为udp类型的socket
    int n = 1;
    setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, &n, sizeof(n));
    is_first = false;
  }
  return send_to(msg,inet_ntoa(_addr.sin_addr) ,_addr.sin_port);
}

int Socket::recv_broadcast(char *buf, int len){
  static bool is_first = true;
  if(is_first){
    if(_type != SOCK_DGRAM)
      throw std::runtime_error("type not consistent when recv broadcast");
    _addr.sin_addr.s_addr = INADDR_ANY;
    if(!is_bind)
      bind();
  }

  return recv(buf, len);
}

std::string Socket::recv_broadcast(){
  static bool is_first = true;
  if(is_first){
    if(_type != SOCK_DGRAM)
      throw std::runtime_error("type not consistent when recv broadcast");
    _addr.sin_addr.s_addr = INADDR_ANY;
    if(!is_bind)
      bind();
  }

  return recv();
}


void Socket::close(){
  if(_socket >= 0)
    ::close(_socket);
  _socket = -1;
}

int Socket :: recv_from(char* buf, int len, std::string addr, int port)
{
  _addr.sin_family = _af;
  _addr.sin_addr.s_addr = ::inet_addr(addr.c_str());
  _addr.sin_port = port;
  socklen_t so = sizeof(_addr);
  return ::recvfrom(_socket, buf, len, 0, (sockaddr*)&_addr, &so);
}

std::string Socket :: recv_from(std::string addr, int port)
{
  _addr.sin_family = _af;
  _addr.sin_addr.s_addr = ::inet_addr(addr.c_str());
  _addr.sin_port = port;
  char buf[200];
  socklen_t so = sizeof(_addr);
  int res = ::recvfrom(_socket, buf, 199, 0, (sockaddr*)&_addr, &so);
  if (res < 0)
    return std :: string();
  return std::string(buf);
}

int Socket :: send_to (std::string msg, std::string addr, int port)
{
  _addr.sin_family = _af;
  _addr.sin_addr.s_addr = inet_addr(addr.c_str());
  _addr.sin_port = port;
  return ::sendto(_socket, msg.c_str(), msg.length() + 1, 0, (sockaddr*) &_addr, sizeof(_addr));
}
