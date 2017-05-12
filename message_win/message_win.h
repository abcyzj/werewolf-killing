#ifndef MESSAGE_WINDOWS
#define MESSAGE_WINDOWS
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

namespace Werewolf{
	class Socket{
		private:
			sockaddr_in _addr;
			SOCKET _socket;
			int _af;
			int _type;
			int _protocol;
			bool is_bind=false;
			char _buffer[100];
			void check_addr(const std::string&);
			Socket(SOCKET socket,int af,int type,int protocol,std::string,int port);
		public:
			enum Howto{RD=0,WR=1,BOTH=2};
			Socket();
			Socket(int af,int type,int protocol);
			Socket(int af,int type,int protocol,const std::string &addr,int port);
			Socket(const Socket&) = delete;
			Socket& operator = (const Socket&) = delete;
			Socket(Socket &&);
			Socket& operator = (Socket &&);
			~Socket();

			void assign_SOCKET(SOCKET newsocket);
			//SOCKET getsocket();
			void assign_addr(const std::string &addr,int port);
			int bind();
			int connect(double delay = 0.0);
			int listen();
			Socket accept(double delay=0.0)const;
			int send(const std::string &msg,double delay=0.0)const;
			int recv(char* buf,int len,double delay=0.0)const;
			std::string recv(double delay=0.0)const;
			int broadcast(const std::string &msg);
			int recv_broadcast(char* buf,int len,double delay=0.0);
			std::string recv_broadcast(double delay=0.0);
			int recv_from(char* buf,int len,const std::string &addr,int port,double delay=0.0);
			std::string recv_from(const std::string &addr,int port,double delay=0.0);
			int send_to(const std::string& msg,const std::string &addr,int port);
			void close();
			static std::string get_host_addr();
			bool valid() const;
			void shutdown(Howto);
	};
}

#endif

