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

Client::~Client(){
  //end_thread();
}

bool Client::available(){
  return _available;
}

// void Client::send(const std::string &msg, double delay){
//   _sock.send(msg, delay);
// }

// void Client::lock(){
//   mtx.lock();
// }

// bool Client::try_lock(){
//   return mtx.try_lock();
// }

// void Client::unlock(){
//   mtx.unlock();
// }

// void Client::start_thread(){
//   _commu_over = false;
//   _th = std::thread(&Client::do_commu, this);
// }

// void Client::end_thread(){
//   if(!_th.joinable())
//     return;

//   _commu_over = true;
//   _th.join();
//   _commu_over = false;
// }

// void Client::do_commu(){
//   while(!_commu_over){
//     if(_order_mtx.try_lock()){//检查是否有需要发送的命令
//       if(_order != ""){
//         _sock.send(_order);
//         _order.clear();//发送后清空命令
//       }
//       _order_mtx.unlock();
//     }

//     std::string msg = _sock.recv(0.02);//接受客户端的消息
//     if(msg != ""){
//       _charac->add_order(msg);//在角色类的命令队列中加入收到的消息（防止粘包）
//     }
//   }
// }

void Client::print(std::string msg){
  _sock.send("PRINT:" + msg +";");
}

void Client::turn_on_input(){
  while(_sock.recv(0.0001) != "");//清空缓冲区
  _sock.send("TURN_ON;");
}

void Client::hold_on_input(){
  while(_sock.recv(0.0001) != "");//清空缓冲区
  _sock.send("HOLD_ON;");
}

void Client::turn_off_input(){
  _sock.send("TURN_OFF;");
}

void Client::shut_down(){
  _sock.send("SHUT_DOWN;");
}

std::string Client::recv(double delay){
  return _sock.recv(delay);
}

void Client::set_charac(Character *ch){
  _charac = ch;
}


Character* Client::selfCharacter(){
  return _charac;
}

void Client::changename(std::string nickname){
  _nickname = nickname;
}

std::string Client::get_nickname(){
  return _nickname;
}
