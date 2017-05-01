#include"accept.h"
#include<stdexcept>
#include<chrono>
#include<iostream>


using namespace Werewolf;

Accept::Accept(): _over(false){};

Accept::Accept(int port): _sock(AF_INET, SOCK_STREAM, IPPROTO_TCP, "0.0.0.0", port){
  _sock.bind();
  _sock.listen();
}

Accept::Accept(int port, std::vector<Client> *vec): _sock(AF_INET, SOCK_STREAM, IPPROTO_TCP, "0.0.0.0", port), _vec(vec), _over(false){
  _sock.bind();
  _sock.listen();
};

void Accept::set_vector(std::vector<Client> *vec){
  _vec = vec;
}

void Accept::do_accept(){
  while(!_over){
    for(int i = 0; i < 10; i++){
      Socket newsock = _sock.accept(0.001);
      if(newsock.valid())
        _vec->push_back(std::move(newsock));
    }
  }
}

void Accept::start(){
  if(_sock.valid()){
    _th = std::thread(&Accept::do_accept, this);
  }
  else{
    throw std::runtime_error("Accept sock not valid.\n");
  }
}

void Accept::end(){
  if(!_th.joinable())
    return;

  _over = true;
  _th.join();
  _over = false;
}

void Accept::show(std::chrono::milliseconds ms){
  _show_over = false;
  _show_th = std::thread(&Accept::do_show, this, ms);
}

void Accept::end_show(){
  if(!_show_th.joinable())
    return;

  _show_over = true;
  _show_th.join();
  _show_over = false;
}

void Accept::do_show(std::chrono::milliseconds ms){
  while(!_show_over){
    std::cout << _vec->size() << " client(s) have connected.\n";
    std::this_thread::sleep_for(ms);
  }
  return;
}
