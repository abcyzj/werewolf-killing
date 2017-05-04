#include"message.h"
#include<iostream>
#include <stdexcept>//报错，抛出runtime_error
#include<cmath>
#include<sys/ioctl.h>

using namespace Werewolf;

void Socket :: check_addr(const std::string& s)
{
	sockaddr_in tep;
	::bzero(&tep, sizeof(tep));
	if(memcmp(&tep, &_addr, sizeof(tep)) == 0)
	{
		throw std::runtime_error(s.c_str());
	}
}

Socket::Socket(int socket, int af, int type, int protocol, std::string addr, int port): _socket(socket), _af(af), _type(type), _protocol(protocol){
  ::bzero(&_addr, sizeof(_addr));
  _addr.sin_family = af;
  _addr.sin_addr.s_addr = inet_addr(addr.c_str());
  _addr.sin_port = htons(port);
}

Socket::Socket():_socket(-1){}

Socket::Socket(int af, int type, int protocol): _af(af), _type(type), _protocol(protocol){
  _socket = ::socket(af, type, protocol);
  ::bzero(&_addr, sizeof(_addr));
}

Socket::Socket(int af, int type, int protocol, const std::string& addr, int port): _af(af), _type(type), _protocol(protocol){
  _socket = ::socket(af, type, protocol);
  ::bzero(&_addr, sizeof(_addr));
  _addr.sin_family = af;
  _addr.sin_addr.s_addr = inet_addr(addr.c_str());
  _addr.sin_port = htons(port);
}

Socket::Socket(Socket &&s): _af(s._af), _type(s._type), _protocol(s._protocol), is_bind(s.is_bind){
  _socket = s._socket;
  s._socket = -1;//-1代表该socket无须被析构
  memcpy(&_addr, &s._addr, sizeof(s._addr));
}

Socket& Socket::operator = (Socket &&s){
  if(this == &s)
    return *this;
  close();
  _socket = s._socket;
  _af = s._af;
  _type = s._type;
  _protocol = s._protocol;
  is_bind = s.is_bind;
  s._socket = -1;
  memcpy(&_addr, &s._addr, sizeof(s._addr));
  return *this;
}

Socket::~Socket(){
  if(_socket != -1)
    ::close(_socket);
}

void Socket::assign_addr(const std::string& addr, int port){
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

int Socket :: connect(double delay)
{
	check_addr("no address when connect");
  if(delay == 0)
    return ::connect(_socket, (sockaddr*) &_addr, sizeof(_addr));

  fd_set fds;
  int sec = std::floor(delay);
  int usec = (delay - std::floor(delay)) * 1e6;
  timeval timeout = {sec, usec};
  FD_ZERO(&fds);
  FD_SET(_socket, &fds);
  unsigned long ul = 1;
  ::ioctl(_socket, FIONBIO, &ul);
  bool ok = false;
  if(::connect(_socket, (struct sockaddr*) &_addr, sizeof(_addr)) == -1){
    if(::select(_socket + 1, NULL, &fds, NULL, &timeout) > 0){
      int error = -1, len;
      ::getsockopt(_socket, SOL_SOCKET, SO_ERROR, &error, (socklen_t *) &len);
      if(error == 0)
        ok = true;
    }
  }
  else
    ok = true;
  ul = 0;
  ::ioctl(_socket, FIONBIO, &ul);
  if(ok)
    return 1;
  else
    return -1;
}

int Socket :: listen()
{
	check_addr("no address when listen");
	return ::listen(_socket, 20);
}

Socket Socket::accept(double delay) const
{
  sockaddr_in new_addr;
  socklen_t len = sizeof(new_addr);
  if(delay == 0.0){//不延时
    int new_sock = ::accept(_socket, (sockaddr*) &new_addr, &len);
    if(new_sock < 0)
      throw std::runtime_error("accept wrong");
    return Socket(_af, _type, _protocol, ::inet_ntoa(new_addr.sin_addr), new_addr.sin_port);
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
    return Socket();
  default:
    if(FD_ISSET(_socket, &fds)){
      int new_socket = ::accept(_socket, (sockaddr *) &new_addr, &len);
      if(new_socket < 0)
        throw std::runtime_error("accept wrong");
      return Socket(new_socket,_af, _type, _protocol, ::inet_ntoa(new_addr.sin_addr), new_addr.sin_port);
    }
    else
      return Socket();
  }
}

int Socket::send(const std::string &msg, double delay) const
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

int Socket :: recv(char * buf, int len, double delay) const
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

std::string Socket::recv(double delay) const{
  char buf[200] = {0};
  if(delay == 0.0){
    int res = ::recv(_socket, buf, 199, 0);
    if(res < 0)
      return std::string();
    else if(res == 0)
      return std::string("__SHUTDOWN");
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
      else if(res == 0)
        return std::string("__SHUTDOWN");
      else
        return std::string(buf);
    }
    else
      return std::string();
  }
}

int Socket::broadcast(const std::string& msg){
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

int Socket::recv_broadcast(char *buf, int len, double delay){
  static bool is_first = true;
  if(is_first){
    if(_type != SOCK_DGRAM)
      throw std::runtime_error("type not consistent when recv broadcast");
    if(!is_bind){
      _addr.sin_addr.s_addr = INADDR_ANY;
      bind();
      is_bind = true;
    }
    is_first = false;
  }

  return recv_from(buf, len, "0.0.0.0", ntohs(_addr.sin_port), delay);
}

std::string Socket::recv_broadcast(double delay){
  static bool is_first = true;
  if(is_first){
    if(_type != SOCK_DGRAM)
      throw std::runtime_error("type not consistent when recv broadcast");
    if(!is_bind){
      _addr.sin_addr.s_addr = INADDR_ANY;
      bind();
      is_bind = true;
    }
    is_first = false;
  }

  return recv_from("0.0.0.0", ntohs(_addr.sin_port), delay);
}


void Socket::close(){
  if(_socket >= 0)
    ::close(_socket);
  _socket = -1;
}

int Socket :: recv_from(char* buf, int len, const std::string& addr, int port, double delay)
{
  _addr.sin_family = _af;
  _addr.sin_addr.s_addr = ::inet_addr(addr.c_str());
  _addr.sin_port = htons(port);
  socklen_t so = sizeof(_addr);

  if(delay == 0.0)
    return ::recvfrom(_socket, buf, len, 0, (sockaddr *) &_addr, &so);

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
      return ::recvfrom(_socket, buf, len, 0, (sockaddr *)&_addr, &so);
    else
      return -1;
  }
}

std::string Socket :: recv_from(const std::string& addr, int port, double delay)
{
  _addr.sin_family = _af;
  _addr.sin_addr.s_addr = ::inet_addr(addr.c_str());
  _addr.sin_port = htons(port);
  char buf[200];
  socklen_t so = sizeof(_addr);

  if(delay == 0.0){
    int res = ::recvfrom(_socket, buf, 199, 0, (sockaddr*)&_addr, &so);
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
    return std::string();//-1表示发送超时
  default:
    if(FD_ISSET(_socket, &fds)){
      int res = ::recvfrom(_socket, buf, 199, 0, (sockaddr*)&_addr, &so);
      if(res < 0)
        return std::string();
      else
        return std::string(buf);
    }

    else
      return std::string();
  }
}

int Socket :: send_to (const std::string& msg, const std::string& addr, int port)
{
  _addr.sin_family = _af;
  _addr.sin_addr.s_addr = inet_addr(addr.c_str());
  _addr.sin_port = port;
  return ::sendto(_socket, msg.c_str(), msg.length() + 1, 0, (sockaddr*) &_addr, sizeof(_addr));
}

std::string Socket::get_host_addr(){
  ::ifaddrs *ifsptr = NULL;
  ::getifaddrs(&ifsptr);

  while(ifsptr != NULL){
    if(ifsptr->ifa_addr->sa_family == AF_INET){
      in_addr *temptr = &((sockaddr_in *)ifsptr->ifa_addr)->sin_addr;
      std::string host_addr(inet_ntoa(*temptr));
      if(host_addr != "127.0.0.1")
        return host_addr;
    }
    ifsptr = ifsptr->ifa_next;
  }

  return std::string();
}

bool Socket::valid() const{
  return _socket != -1;
}

void Socket::shutdown(Werewolf::Socket::Howto how){
  ::shutdown(_socket, (int)how);
}
