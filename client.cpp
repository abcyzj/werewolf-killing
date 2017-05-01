#include"client.h"

using namespace Werewolf;

Client::Client(): _available(false), _sock(){}

Client::Client(Client &&cl){
  if(cl.available()){
    _sock = std::move(cl._sock);
    _available = true;
    cl._available = false;
  }
  else{
    _available = false;
  }
}

Client& Client::operator = (Client &&cl){
  if(this == &cl)
    return *this;

  if(cl.available()){
    _sock = std::move(cl._sock);
    _available = true;
    cl._available = false;
  }
  else{
    _available = false;
  }
  return *this;
}

Client::Client(Socket &&sock): _sock(std::move(sock)){
  if(_sock.valid())
    _available = true;
}

void Client::set_sock(Socket &&sock){
  _sock.close();
  _sock = std::move(sock);
  if(_sock.valid())
    _available = true;
}

bool Client::available(){
  return _available;
}
