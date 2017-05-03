#include"for_client.h"
#include<algorithm>
#include<iostream>

using namespace Werewolf;

Connect::Connect(): _recv_over(false), _show_over(false){}

Connect::Connect(int port): _por(port), _recv_over(false), _show_over(false), _recv_sock(AF_INET, SOCK_DGRAM, 0, "0.0.0.0", port){}

Connect::~Connect(){
  if(_recv_th.joinable())
    end_recv();
  if(_show_th.joinable())
    end_show();
}

void Connect::set_port(int port){
  _por = port;
  _recv_sock = Socket(AF_INET, SOCK_DGRAM, 0, "0.0.0.0", port);
}

void Connect::do_recv(){
  while(!_recv_over){
    for(int i = 0; i < 10; i++){
      std::string msg = _recv_sock.recv_broadcast(0.001);
      if(msg != ""){
        std::unique_lock<std::mutex> lck(_info_mtx);
        if(std::find(_info.begin(), _info.end(), msg) == _info.end())
          _info.push_back(msg);
        lck.unlock();
      }
    }
  }
}

void Connect::do_show(std::chrono::milliseconds ms){
  while(!_show_over){
    std::unique_lock<std::mutex> lck(_info_mtx);
    std::cout << "Found " << _info.size() << " server(s).\n";
    for(auto &i: _info){
      std::cout << i << std::endl;
    }
    lck.unlock();
    std::this_thread::sleep_for(ms);
  }
}

void Connect::recv(){
  if(_recv_th.joinable())
    return;

  _recv_over = false;
  _recv_th = std::thread(&Connect::do_recv, this);
}

void Connect::end_recv(){
  if(!_recv_th.joinable())
    return;

  _recv_over = true;
  _recv_th.join();
  _recv_over = false;
}

void Connect::show(std::chrono::milliseconds ms){
  if(_show_th.joinable())
    return;

  _show_over = false;
  _show_th = std::thread(&Connect::do_show, this, ms);
}

void Connect::end_show(){
  if(!_show_th.joinable())
    return;

  _show_over = true;
  _show_th.join();
  _show_over = false;
}

Socket Connect::connect(int k){
  std::unique_lock<std::mutex> lck(_info_mtx);
  if(k >= _info.size()){
    std::cout << "There's no " << k << "th server.\n";
    return Socket();
  }

  Socket newsock(AF_INET, SOCK_STREAM, 0, get_IP(_info[k]), _por);
  int res = newsock.connect(5);
  if(res > 0)
    return std::move(newsock);
  else
    return Socket();
}


Socket Connect::connect_via_IP(const std::string &IP){
  Socket newsock(AF_INET, SOCK_STREAM, 0, IP, _por);
  int res = newsock.connect(5);
  if(res > 0)
    return std::move(newsock);
  else
    return Socket();
}

std::string Connect::get_IP(std::string msg){
  int a = msg.find("IP:");
  int b = msg.find(';', a);
  return std::string(a, b - a + 1);
}
