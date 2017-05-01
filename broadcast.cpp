#include"broadcast.h"
#include<thread>
#include<iostream>
#include<string>

using namespace Werewolf;

Broadcast::Broadcast(int port): _sock(AF_INET, SOCK_DGRAM, IPPROTO_UDP, "0.0.0.0", port), _over(false){}

Broadcast::~Broadcast(){
  if(_th.joinable())
    end();
}

void Broadcast::do_broadcast(){
  while(1){
    if(_over)
      return;

    for(int i = 0; i < 10; i++)
      _sock.broadcast(_msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}

void Broadcast::start(const std::string &msg){
  _msg = msg;
  _th = std::thread(&Broadcast::do_broadcast, this);
}

void Broadcast::end(){
  if(!_th.joinable())
    return;
  _over = true;
  _th.join();
  _over = false;
}
