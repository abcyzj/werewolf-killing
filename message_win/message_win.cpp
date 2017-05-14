#include "message_win.h"
#include <string>
#include <stdlib.h>
#include <stdexcept>
#include <winsock2.h>
#include <cmath>
#include <iostream>
using namespace Werewolf;

void Socket::check_addr(const std::string& s){
	sockaddr_in tep;
	memset(&tep,0,sizeof(tep));
	if(memcmp(&tep,&_addr,sizeof(tep))==0)
		throw std::runtime_error(s.c_str());
}
Socket::Socket(SOCKET socket,int af,int type,int protocol,std::string addr,int port):_socket(socket),_af(af),_type(type),_protocol(protocol){
	memset(&_addr,0,sizeof(_addr));
	_addr.sin_family = af;
	_addr.sin_addr.s_addr = ::inet_addr(addr.c_str());
	_addr.sin_port = ::htons(port);
}
Socket::Socket():_socket(-1){}

Socket::Socket(int af,int type,int protocol):_af(af),_type(type),_protocol(protocol){
	_socket=::socket(af,type,protocol);
	memset(&_addr,0,sizeof(_addr));
}

Socket::Socket(int af,int type,int protocol,const std::string &addr,int port):_af(af),_type(type),_protocol(protocol){
	_socket=::socket(af,type,protocol);
	memset(&_addr,0,sizeof(_addr));
	_addr.sin_family=af;
	_addr.sin_addr.s_addr=inet_addr(addr.c_str());
	_addr.sin_port = ::htons(port);
}
//
Socket::Socket(Socket &&s):_af(s._af),_type(s._type),_protocol(s._protocol){
	_socket=s._socket;
	s._socket=-1;
	memcpy(&_addr,&s._addr,sizeof(s._addr));
}

Socket& Socket::operator=(Socket&&s){
	_socket=s._socket;
	_af=s._af;
	_type=s._type;
	_protocol=s._protocol;
	s._socket=-1;
	memcpy(&_addr,&s._addr,sizeof(s._addr));
	return *this;
}
Socket::~Socket(){
	if(_socket!=-1)
		::closesocket(_socket);
};
/*
  void Socket::assign_SOCKET(SOCKET newsocket){
	_socket=newsocket;
  }

  SOCKET Socket::getsocket(){return _socket;}
*/
void Socket::assign_addr(const std::string& addr,int port){
	memset(&_addr,0,sizeof(_addr));
	_addr.sin_family=_af;
	_addr.sin_addr.s_addr=inet_addr(addr.c_str());
	_addr.sin_port=htons(port);
}

int Socket::bind(){
	check_addr("no address when bind");
	int res=::bind(_socket,(SOCKADDR*)&_addr,sizeof(_addr));
	if(res >= 0)
		is_bind=true;
	return res;
}
int Socket::connect(double delay){
	check_addr("no address when connect");
  if(delay == 0)
    return ::connect(_socket, (sockaddr*) &_addr, sizeof(_addr));

  fd_set fds;
  int sec = std::floor(delay);
  int usec = (delay - std::floor(delay)) * 1e6;
  timeval timeout = {sec, usec};
  // FD_ZERO(&fds);
  // FD_SET(_socket, &fds);
  // unsigned long ul = 1;
  // ::ioctl(_socket, FIONBIO, &ul);
  // bool ok = false;
  // if(::connect(_socket, (struct sockaddr*) &_addr, sizeof(_addr)) == -1){
  //   if(::select(_socket + 1, NULL, &fds, NULL, &timeout) > 0){
  //     int error = -1, len;
  //     ::getsockopt(_socket, SOL_SOCKET, SO_ERROR, &error, (socklen_t *) &len);
  //     if(error == 0)
  //       ok = true;
  //   }
  // }
  // else
  //   ok = true;
  // ul = 0;
  // ::ioctl(_socket, FIONBIO, &ul);
  // if(ok)
  //   return 1;
  // else
  //   return -1;
  unsigned long ul = 1;
  int error = -1, len;
  len = sizeof(int);
  ioctlsocket(_socket, FIONBIO, &ul); //璁剧疆涓洪潪闃诲妯″紡  
  bool ret = false;  
  if( ::connect(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) ==   
      -1)  
    {  
      FD_ZERO(&fds);  
      FD_SET(_socket, &fds);  
      if(::select(_socket+1, NULL, &fds, NULL, &timeout) > 0)  
        {  
          ::getsockopt(_socket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);  
          if(error == 0) ret = true;  
          else ret = false;  
        } else ret = false;  
    }  
  else ret = true;  
  ul = 0;  
  ::ioctlsocket(_socket, FIONBIO, &ul);
  if(ret)
    return 1;
  else
    return -1;
}
int Socket::listen(){
	check_addr("no address when listen");
	return ::listen(_socket,20);
}
Socket Socket::accept(double delay) const
{
	sockaddr_in new_addr;
	int len = sizeof(new_addr);
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
	std::string Umsg = GBK_To_UTF8(msg);
  if(delay == 0.0)
    return :: send(_socket, Umsg.c_str(), Umsg.length() + 1, 0);

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
      return ::send(_socket, Umsg.c_str(), Umsg.length() + 1, 0);
    else
      return -1;
  }
}
/*
  int Socket::send(const std::string &msg,double delay) const{
	if(delay>1e-9){
  int dt=delay*1000;
  ::setsockopt(_socket,SOL_SOCKET,SO_SNDTIMEO,(char*)&dt, sizeof(int));
	}
	return ::send(_socket,msg.c_str(),msg.size()+1,0);
  }

*/
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





/*
  int Socket::recv(char* buf,int len,double delay)const {
	if(delay>1e-9){
  int dt=delay*1000;
  ::setsockopt(_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&dt, sizeof(int));
	}
	return ::recv(_socket,buf,len,0);//delay remained to be done
  }
*/
std::string Socket::recv(double delay) const{
  char buf[200] = {0};
  if(delay == 0.0){
    int res = ::recv(_socket, buf, 199, 0);
    if(res < 0)
      return std::string();
    else if(res == 0)
      return std::string("__SHUTDOWN");
    else
      return UTF8_To_GBK(std::string(buf));
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
        return UTF8_To_GBK(std::string(buf));
    }
    else
      return std::string();
  }
}


/*
  std::string Socket::recv(double delay)const {//delay remained to be done
	char buf[200]={0};
	if(delay>1e-9){
  int dt=delay*1000;
  ::setsockopt(_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&dt,sizeof(int));
	}
	int res=::recv(_socket,buf,199,0);
	if(res<0)
  return "";
	else if(res==0)
  return "__SHUTDOWN";
	else
  return std::string(buf);
  }*/



int Socket::broadcast(const std::string &msg){
	static bool is_first_send=true;
	if(is_first_send){
		if(_type!=SOCK_DGRAM)
			throw std::runtime_error("type not consistent when broadcast");
		bool opt=true;
		::setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char FAR *>(&opt), sizeof(opt));
		is_first_send=false;
	}
	return send_to(msg,inet_ntoa(_addr.sin_addr),ntohs(_addr.sin_port));
}
int Socket::recv_broadcast(char* buf,int len,double delay){
	static bool is_first_recv=true;
	if(is_first_recv){
		if(_type!=SOCK_DGRAM)
			throw std::runtime_error("type not consistent when recv broadcast");
		if(!is_bind){
			_addr.sin_addr.s_addr=htonl(INADDR_ANY);
			bind();
			is_bind=true;
		}
		is_first_recv=false;
	}
	return recv_from(buf,len,"0.0.0.0",ntohs(_addr.sin_port),delay);//0.0.0.0地址是无效的，因为在recvfrom时会把server的地址放入_addr中
}
std::string Socket::recv_broadcast(double delay){
	static bool is_first_recv=true;
	if(is_first_recv){
		if(_type!=SOCK_DGRAM)
			throw std::runtime_error("type not consistent when recv broadcast");
		if(!is_bind){
			_addr.sin_addr.s_addr=htonl(INADDR_ANY);
			while(bind() < 0);
      is_bind=true;
		}
		is_first_recv=false;
	}
	return recv_from("0.0.0.0",ntohs(_addr.sin_port),delay);//0.0.0.0地址是无效的，因为在recvfrom时会把server的地址放入_addr中
}
int Socket :: recv_from(char* buf, int len, const std::string& addr, int port, double delay)
{
  _addr.sin_family = _af;
  _addr.sin_addr.s_addr = ::inet_addr(addr.c_str());
  _addr.sin_port = ::htons(port);
  int so = sizeof(SOCKADDR);

  if(delay == 0.0)
    return ::recvfrom(_socket, buf, len, 0, (sockaddr *) &_addr, &so);

  fd_set fds;
  int sec = std::floor(delay);
  int usec = (delay - std::floor(delay)) * 1e6;
  timeval timeout = {sec, usec};
  
  //  timeval *timeoutptr = &timeout;
  //  if(delay == 0.0)
  //  	timeoutptr = NULL;
  //  	
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

/*
  int Socket::recv_from(char*buf,int len,const std::string&addr,int port,double delay){
	_addr.sin_family=_af;
	_addr.sin_addr.s_addr=inet_addr(addr.c_str());
	_addr.sin_port=htons(port);

	if(delay>1e-9){
  int dt=delay*1000;
  ::setsockopt(_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&dt,sizeof(int));
	}
	int nsize=sizeof(_addr);
	return ::recvfrom(_socket,buf,len,0,(SOCKADDR*)&_addr,&nsize);
  }*/
std::string Socket :: recv_from(const std::string& addr, int port, double delay)
{
  _addr.sin_family = _af;
  _addr.sin_addr.s_addr = ::inet_addr(addr.c_str());
  _addr.sin_port = ::htons(port);
  char buf[200];
  int so = sizeof(_addr);

  if(delay == 0.0){
    int res = ::recvfrom(_socket, buf, 199, 0, (sockaddr *)&_addr, &so);
    if(res < 0)
      return std::string();
    else
      return UTF8_To_GBK(std::string(buf));
  }

  fd_set fds;
  int sec = std::floor(delay);
  int usec = (delay - std::floor(delay)) * 1e6;
  timeval timeout = {sec, usec};
  
  //  timeval *timeoutptr = &timeout;
  //  if(delay == 0.0)
  //  	timeoutptr = NULL;
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
        return UTF8_To_GBK(std::string(buf));
    }

    else
      return std::string();
  }
}


/*
  std::string Socket::recv_from(const std::string& addr,int port,double delay)
  {
	_addr.sin_family=_af;
	_addr.sin_addr.s_addr=inet_addr(addr.c_str());
	_addr.sin_port=htons(port);
	char buf[200]={0};
	if(delay>1e-9){
  int dt=delay*1000;
  ::setsockopt(_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&dt,sizeof(int));
	}
	int nsize=sizeof(_addr);
	int res=::recvfrom(_socket,buf,199,0,(SOCKADDR*)&_addr,&nsize);
	if(res<0)
  return "";
	else
  return std::string(buf);
  }
*/
int Socket::send_to(const std::string& msg,const std::string &addr,int port){
	std::string Umsg = GBK_To_UTF8(msg);
	_addr.sin_family=_af;
	_addr.sin_addr.s_addr=inet_addr(addr.c_str());
	_addr.sin_port=htons(port);
	return ::sendto(_socket,Umsg.c_str(), Umsg.size()+1,0,(SOCKADDR*)&_addr,sizeof(_addr));
}

void Socket::close(){
	if(_socket>=0)
		::closesocket(_socket);
	_socket=-1;
}

bool Socket::valid()const{
	return _socket!=-1;
}

void Socket::shutdown(Socket::Howto how){
	::shutdown(_socket,(int)how);
}
/*
  std::string Socket::get_host_addr()const{
	::ifaddrs *ifsptr = NULL;
	::getifaddrs(&ifsptr);

  while(ifsptr != NULL){
  if(ifsptr->ifa_adr->sa_family == AF_INET){
  in_addr *temptr = &((sockaddr_in *)ifsptr->ifa_addr)->sin_addr;
  std::string host_addr(inet_ntoa(*temptr));
  if(host_addr != "127.0.0.1")
  return host_addr;
  }
  ifsptr = ifsptr->ifa_next;
  }
  return std::string();
  }
*/
std::string Socket::get_host_addr(){
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	::WSAStartup(sockVersion, &wsaData);
	char lv_name[50];
	gethostname(lv_name, 50);
	hostent * lv_pHostent;
	lv_pHostent = (hostent *)malloc(sizeof(hostent));
	if( NULL == (lv_pHostent = gethostbyname(lv_name)))
    {
      printf("get Hosrname Fail \n");
      return 0;
    }
	SOCKADDR_IN lv_sa;
	lv_sa.sin_family = AF_INET;
	lv_sa.sin_port = htons(6000);
	memcpy(&lv_sa.sin_addr.S_un.S_addr, lv_pHostent->h_addr_list[0], lv_pHostent->h_length);
	return inet_ntoa(lv_sa.sin_addr);
}

std::string Socket::GBK_To_UTF8(const std::string & str)  
{  
  int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);  
  
  wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴  
  ZeroMemory(pwBuf, nwLen * 2 + 2);  
  
  ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);  
  
  int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);  
  
  char * pBuf = new char[nLen + 1];  
  ZeroMemory(pBuf, nLen + 1);  
  
  ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);  
  
  std::string retStr(pBuf);  
  
  delete []pwBuf;  
  delete []pBuf;  
  
  pwBuf = NULL;  
  pBuf = NULL;  
  
  return retStr;  
}

std::string Socket::UTF8_To_GBK(const std::string & str)  
{  
  int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);  
  
  wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴  
  memset(pwBuf, 0, nwLen * 2 + 2);  
  
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);  
  
  int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);  
  
  char * pBuf = new char[nLen + 1];  
  memset(pBuf, 0, nLen + 1);  
  
  WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);  
  
  std::string retStr = pBuf;  
  
  delete []pBuf;  
  delete []pwBuf;  
  
  pBuf = NULL;  
  pwBuf = NULL;  
  
  return retStr;  
}
